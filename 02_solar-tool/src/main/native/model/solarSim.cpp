/*
 * solarSim.cpp
 *
 *  Created on: 9. 8. 2022
 *      Author: Mirek Mikus
 */

#include "solarSim.h"
#include "modelParams.h"

cSolarMdlSim::cSolarMdlSim(cEfrCtrlI *efrContext)
{
    m_efrContext = efrContext;
}

cSolarMdlSim::cSolarMdlSim()
{
    m_efrContext = NULL;
}

cSolarMdlSim::~cSolarMdlSim()
{
    m_dataSet = NULL;
}

void cSolarMdlSim::initSim(void)
{
    m_free(m_outvEngHarv);
    m_free(m_outvEngLost);
    m_free(m_outvSoes);
    m_free(m_outvBuffSize);
    m_free(m_outvNextPeriod);
    m_free(m_outvTxPayload);
    m_free(m_outvBuffLost);
    m_free(m_outvFailM);
    m_free(m_outvFailT);
    m_free(m_outvFailD);
    m_setESSoc(0.5f);
    m_stepId = 0u;
    m_nextTx = 1u;
    m_buffSize = 0u;
    m_buffLost = 0u;
    m_engNewPot = 0;
    m_sysReset = false;
    m_txOk = false;

    m_outvEngHarv = (double*)malloc(m_dataSetLen*sizeof(double));
    m_outvEngLost = (double*)malloc(m_dataSetLen*sizeof(double));
    m_outvSoes = (double*)malloc(m_dataSetLen*sizeof(double));
    m_outvBuffSize = (unsigned int*)malloc(m_dataSetLen*sizeof(unsigned int));
    m_outvNextPeriod = (unsigned short*)malloc(m_dataSetLen*sizeof(unsigned short));
    m_outvTxPayload = (unsigned short*)malloc(m_dataSetLen*sizeof(unsigned short));
    m_outvBuffLost = (unsigned int*)malloc(m_dataSetLen*sizeof(unsigned int));
    m_outvFailM = (bool*)malloc(m_dataSetLen*sizeof(bool));
    m_outvFailT = (bool*)malloc(m_dataSetLen*sizeof(bool));
    m_outvFailD = (bool*)malloc(m_dataSetLen*sizeof(bool));


}

void cSolarMdlSim::initSimEfr(void)
{
    this->initSim();
    if(m_stepId >= m_dataSetLen)
         throw std::out_of_range("Simulation step out of range");

    this->m_evalSolarEnergy();
    this->m_evalMeas();
    this->m_evalTransmit();
    this->m_evalLogging();
}

bool cSolarMdlSim::loadDataFile(const char *fname)
{
    std::ifstream dataFile(fname, ios::in);
    unsigned int line_cnt = 0;
    bool retVar = true;
    string line, cell;
    tm *time = localtime(new time_t());

    m_free(m_dataSet);
    m_dataSetLen = 0;

    if(dataFile.is_open())
    {
        if(std::getline(dataFile, line))
        {
            stringstream str(line);
            std::getline(str, cell, this->m_dataFileDelimiter);
            try
            {
                if(std::getline(str, cell, this->m_dataFileDelimiter))
                {
                    line_cnt = stoi(cell);

                    m_dataSet = (t_DataRow*)malloc(line_cnt*sizeof(t_DataRow));

                    std::getline(dataFile, line); // skip second line, because col headers
                    for(unsigned int i = 0; i < line_cnt; i++)
                    {
                        if(!std::getline(dataFile, line))
                        {
                            retVar = false;
                            break;
                        }

                        stringstream str(line);

                        std::getline(str, cell, this->m_dataFileDelimiter);
                        sscanf(cell.c_str(), "%d.%d.%d", &time->tm_mday, &time->tm_mon, &time->tm_year);
                        std::getline(str, cell, this->m_dataFileDelimiter);
                        sscanf(cell.c_str(), "%d:%d", &time->tm_hour, &time->tm_min);
                        std::getline(str, cell, this->m_dataFileDelimiter);
                        time->tm_year -= 1900;
                        time->tm_mon--;

                        m_dataSet[i].timestamp = mktime(time);
                        m_dataSet[i].val_Pd = stoi(cell);
                    }
                }
            }
            catch(...)
            {
                retVar = false;
            }
            m_dataSetLen = line_cnt;
        }
    }
    else
        retVar = false;

    if(!retVar && m_dataSet != NULL)
    {
        free(m_dataSet);
        m_dataSet = NULL;
        m_dataSetLen = 0;
    }

    return retVar;
}

void cSolarMdlSim::saveSimOuts(const char *fname)
{
    char time_str[20];
    std::ofstream dataOutFile(fname);

    dataOutFile << "Time;Pd;E_lost;E_harv;SoES;BuffSize;BuffLost;T_next;Payload;Fail_M;Fail_T;Fail_D\n";
    for(unsigned int i = 0; i < m_dataSetLen; i++)
    {
        std::strftime(time_str, 20, "%d.%m.%Y %H:%M", std::localtime(&m_dataSet[i].timestamp));
        dataOutFile << time_str << ";" << m_dataSet[i].val_Pd << ";" << m_outvEngLost[i] << ";";
        dataOutFile << m_outvEngHarv[i] << ";" << m_outvSoes[i] << ";" << m_outvBuffSize[i] << ";";
        dataOutFile << m_outvBuffLost[i] << ";" << m_outvNextPeriod[i] << ";" << m_outvTxPayload[i] << ";";
        dataOutFile << m_outvFailM[i] << ";" << m_outvFailT[i] << ";" << m_outvFailD[i] << "\n";
    }

    dataOutFile.close();
}

void cSolarMdlSim::simRun(void)
{
    unsigned int i;
    for(i = 0; i < m_dataSetLen; i++)
    {
        simSingleCycle();
    }
}

void cSolarMdlSim::simSingleCycle(void)
{
    double eng_new_pot, eng_new_hrv, eng_new_lost, eng_req_tx, efr_out_Tnext;
    double efr_in_soesAvg[cMdlPars::EfrSoesAvgSize], efr_in_eAvg[cMdlPars::EfrEAvgSize];
    bool meas_ok, tx_ok;
    int buffSize_rem, tx_payload, next_tx_period;

    if(m_stepId >= m_dataSetLen)
         throw std::out_of_range("Simulation step out of range");

    // Potential energy from PV panel
    eng_new_pot = m_dataSet[m_stepId].val_Pd * cMdlPars::S_PV * (1 - cMdlPars::k_SH) * cMdlPars::n_PV * cMdlPars::n_DCDC1 * cMdlPars::T_MEAS;
    m_setESEng(m_esEng + eng_new_pot);
    if(m_esEng > cMdlPars::C_STORE)
    {
        eng_new_lost = m_esEng - cMdlPars::C_STORE;
        eng_new_hrv = eng_new_pot - eng_new_lost;
        m_setESEng(cMdlPars::C_STORE);
    }
    else
    {
        eng_new_hrv = eng_new_pot;
        eng_new_lost = 0;
    }

    // sleep energy
    m_outvFailD[m_stepId] = (m_esEng < (cMdlPars::E_SLEEP/cMdlPars::n_DCDC2));

    m_setESEng(m_esEng - cMdlPars::E_SLEEP/cMdlPars::n_DCDC2);

    // measurment evaluation
    meas_ok = (m_esEng >= ((cMdlPars::E_MEA/cMdlPars::n_DCDC2) + (cMdlPars::E_NVM/cMdlPars::n_DCDC2)));

    if(meas_ok)
    {
        m_setESEng(m_esEng - ((cMdlPars::E_MEA/cMdlPars::n_DCDC2) + (cMdlPars::E_NVM/cMdlPars::n_DCDC2)));
        m_buffSize++;
    }
    else
        m_sysReset = true;

    if(m_buffSize > cMdlPars::BuffSizeMax)
    {
        m_buffSize = cMdlPars::BuffSizeMax;
        m_buffLost++;
    }
    m_outvFailM[m_stepId] = !meas_ok;

    // transmission evaluation
    tx_payload = 0;
    tx_ok = true;
    next_tx_period = 0;

    if(m_stepId >= m_nextTx)
    {
        eng_req_tx = ((m_buffSize / cMdlPars::Smpl_TX32B)*(cMdlPars::E_TX32B/cMdlPars::n_DCDC2));
        buffSize_rem = m_buffSize%cMdlPars::Smpl_TX32B;
        if(buffSize_rem > 0 && buffSize_rem <= 2)
            eng_req_tx += cMdlPars::E_TX8B/cMdlPars::n_DCDC2;
        else if (buffSize_rem > 0)
            eng_req_tx += cMdlPars::E_TX32B/cMdlPars::n_DCDC2;
        //eng_req_tx /= cMdlPars::n_DCDC2;

        tx_ok = (m_esEng >= eng_req_tx) & !m_sysReset;
        if(tx_ok)
        {
            m_setESEng(m_esEng - eng_req_tx);
            tx_payload = m_buffSize;
            m_buffSize = 0u;
        }
        else
        {
            m_sysReset = true;
        }
    }

    // harvested/lost energy evaluation
    if(m_esEng < 0)
    {
            m_setESEng(0);
    }
    m_outvEngHarv[m_stepId] = eng_new_hrv;
    m_outvEngLost[m_stepId] = eng_new_lost;
    m_outvSoes[m_stepId] = m_esSoc;
    m_outvFailT[m_stepId] = !tx_ok;
    m_outvTxPayload[m_stepId] = tx_payload;
    m_outvBuffLost[m_stepId] = m_buffLost;
    m_outvBuffSize[m_stepId] = m_buffSize;

    //compute new TxNext
    if((tx_payload > 0) && tx_ok)
    {
        m_compSoesAvg(efr_in_soesAvg);
        m_compEAvg(efr_in_eAvg);
        efr_out_Tnext = m_efrContext->efrCompute(efr_in_soesAvg, m_esSoc, efr_in_eAvg);
        next_tx_period = (unsigned short)(efr_out_Tnext*cMdlPars::T_TX_MAX + 1);
        next_tx_period = (next_tx_period < cMdlPars::T_TX_MAX) ? next_tx_period : cMdlPars::T_TX_MAX;
        m_nextTx = next_tx_period + m_stepId;
    }
    m_outvNextPeriod[m_stepId] = next_tx_period*cMdlPars::T_MEAS;

    // reset in this cycle?
    if(m_sysReset)
    {
        m_nextTx = cMdlPars::T_TX_MAX + m_stepId;
    }
    m_sysReset = false;

    m_stepId++;
}

bool cSolarMdlSim::simSingleCycleEfr(double nextTx)
{

    // use value from EFR controller only if transmit was successful
    unsigned short next_tx_period = 0;
    if(m_txOk)
    {
        next_tx_period = (unsigned short)(nextTx*cMdlPars::T_TX_MAX + 1);
        next_tx_period = (next_tx_period < cMdlPars::T_TX_MAX) ? next_tx_period : cMdlPars::T_TX_MAX;
        m_nextTx = next_tx_period + m_stepId;
    }
    m_outvNextPeriod[m_stepId] = next_tx_period*cMdlPars::T_MEAS;

    // reset in this cycle?
    if(m_sysReset)
    {
        m_nextTx = cMdlPars::T_TX_MAX + m_stepId;
    }
    m_sysReset = false;

    m_stepId++;

    if(m_stepId >= m_dataSetLen)
         throw std::out_of_range("Simulation step out of range");

    this->m_evalSolarEnergy();
    this->m_evalMeas();
    this->m_evalTransmit();
    this->m_evalLogging();

    return m_txOk;
}

void cSolarMdlSim::finishSimEfr(void)
{
    m_outvNextPeriod[m_stepId] = 0;
    m_stepId++;
}

void cSolarMdlSim::getCtrlrInputs(double* soesAvg, double* soesCurr, double* eAvg)
{
    this->m_compSoesAvg(soesAvg);
    this->m_compEAvg(eAvg);
    *soesCurr = m_esSoc;
}

unsigned int cSolarMdlSim::getDataLength(void)
{
    return m_dataSetLen;
}

void cSolarMdlSim::m_setESEng(double eng)
{
    if(eng < 0)
        eng = 0;
    m_esEng = eng;
    m_esSoc = eng/cMdlPars::C_STORE;
}

void cSolarMdlSim::m_setESSoc(double soc)
{
    if(soc < 0)
        soc = 0;
    m_esSoc = soc;
    m_esEng = soc*cMdlPars::C_STORE;
}

void cSolarMdlSim::m_free(void* ptr)
{
    if(ptr != NULL)
    {
        free(ptr);
        ptr = NULL;
    }
}

void cSolarMdlSim::m_compSoesAvg(double soesAvg[])
{
    int dataId, cnt;
    dataId = m_stepId;
    for (int soesAvgId = 0; soesAvgId < cMdlPars::EfrSoesAvgSize; soesAvgId++)
    {
        soesAvg[soesAvgId] = 0;
        cnt = 0;

        for(; dataId >= 0 && cnt < cMdlPars::EfrSoesAvgSmpls; dataId--, cnt++)
        {
            soesAvg[soesAvgId] += m_outvSoes[dataId];
        }
        if(cnt != 0)
        {
            soesAvg[soesAvgId] /= cnt;
        }
    }
}

void cSolarMdlSim::m_compEAvg(double eAvg[])
{
    int dataId, cnt;
    dataId = m_stepId;
    for(int eavgId = 0; eavgId < cMdlPars::EfrEAvgSize; eavgId++)
    {
        eAvg[eavgId] = 0;
        cnt = 0;

        for(; dataId >= 0 && cnt < cMdlPars::EfrEAvgSmpls; dataId--, cnt++)
        {
            eAvg[eavgId] += (m_outvEngHarv[dataId] + m_outvEngLost[dataId]);
        }
        if(cnt != 0)
        {
            eAvg[eavgId] /= cnt;
        }
        eAvg[eavgId] /= cMdlPars::EfrEAvgMaxVal;
    }
}

void cSolarMdlSim::calcFitness(double *p1, double *p2)
{
    unsigned int dayCnt, smplPerDay, startID, endID;
    double failMday, buffSizeAvg = 0;
    bool failM;

    if(m_dataSetLen == 0)
        throw std::length_error("Zero data size");

    // P1 = (avg_buff_size - 1)/BuffSizeMax
    for(unsigned int i = 0; i < m_dataSetLen; i++)
    {
        buffSizeAvg += m_outvBuffSize[i];
    }
    buffSizeAvg /= m_dataSetLen;
    *p1 = buffSizeAvg/cMdlPars::BuffSizeMax;

    // P2 = FailM_day/365
    failMday = 0;
    smplPerDay = (24*60*60)/cMdlPars::T_MEAS;
    dayCnt = (m_dataSetLen/smplPerDay)+(m_dataSetLen%smplPerDay > 0);
    for(unsigned int i = 0; i < dayCnt; i++)
    {
        failM = false;
        startID = i*smplPerDay;
        endID = (i+1)*smplPerDay;
        endID = (endID <= (m_dataSetLen+1)) ? endID : (m_dataSetLen+1);

        for(unsigned int y = startID; y < endID; y++)
        {
            if(m_outvFailM[y])
            {
                failM = true;
                break;
            }
        }
        failMday += failM;
    }
    failMday /= dayCnt;
    *p2 = failMday;
}

/*void cSolarMdlSim::calcFitness2(double *p1, double *p2)
{
	unsigned int ovch_cnt = 0, dev_fail = 0;
	for(unsigned int i = 0; i < m_dataSetLen; i++)
	{
		ovch_cnt += (m_outvEngLost[i] > 0);
		dev_fail += m_outvFailD[i];
	}
	*p1 = ((float)ovch_cnt)/m_dataSetLen;
	*p2 = ((float)dev_fail)/m_dataSetLen;
}*/

cSimStats* cSolarMdlSim::calcStats(void)
{
    cSimStats* stats = new cSimStats();

    stats->BuffSizeAvg = 0;
    stats->FailM = 0;
    stats->FailT = 0;
    stats->OvchCnt = 0;
    stats->E_Unused = 0;
    stats->MeasOk = 0;
    stats->TransOk = 0;
    stats->FailD = 0;

    for (unsigned int i = 0; i < m_dataSetLen; i++)
    {
        stats->BuffSizeAvg += m_outvBuffSize[i];
        stats->FailM += m_outvFailM[i];
        stats->FailT += m_outvFailT[i];
        stats->OvchCnt += (m_outvEngLost[i] > 0);
        stats->E_Unused += m_outvEngLost[i];
        stats->TransOk += (m_outvTxPayload[i] > 0);
        stats->MeasOk += !m_outvFailM[i];
        stats->FailD += m_outvFailD[i];
    }
    stats->BuffSizeAvg /= m_dataSetLen;
    stats->BuffLost = m_outvBuffLost[m_dataSetLen-1];

    return stats;
}

void cSolarMdlSim::calcStats(cSimStats * stats)
{
    stats->BuffSizeAvg = 0;
    stats->FailM = 0;
    stats->FailT = 0;
    stats->OvchCnt = 0;
    stats->E_Unused = 0;
    stats->MeasOk = 0;
    stats->TransOk = 0;
    stats->FailD = 0;

    for (unsigned int i = 0; i < m_dataSetLen; i++)
    {
        stats->BuffSizeAvg += m_outvBuffSize[i];
        stats->FailM += m_outvFailM[i];
        stats->FailT += m_outvFailT[i];
        stats->OvchCnt += (m_outvEngLost[i] > 0);
        stats->E_Unused += m_outvEngLost[i];
        stats->TransOk += (m_outvTxPayload[i] > 0);
        stats->MeasOk += !m_outvFailM[i];
        stats->FailD += m_outvFailD[i];
    }
    stats->BuffSizeAvg /= m_dataSetLen;
    stats->BuffLost = m_outvBuffLost[m_dataSetLen-1];
}

void cSolarMdlSim::m_evalSolarEnergy(void)
{
    double eng_new_lost, eng_new_hrv;

    // Potential energy from PV panel
    m_engNewPot = m_dataSet[m_stepId].val_Pd * cMdlPars::S_PV * (1 - cMdlPars::k_SH) * cMdlPars::n_PV * cMdlPars::n_DCDC1 * cMdlPars::T_MEAS;
    m_setESEng(m_esEng + m_engNewPot);

    if(m_esEng > cMdlPars::C_STORE)
    {
        eng_new_lost = m_esEng - cMdlPars::C_STORE;
        eng_new_hrv = m_engNewPot - eng_new_lost;
        m_setESEng(cMdlPars::C_STORE);
    }
    else
    {
        if(m_esEng < 0)
        {
            m_setESEng(0);
        }
        eng_new_hrv = m_engNewPot;
        eng_new_lost = 0;
    }
    m_outvEngHarv[m_stepId] = eng_new_hrv;
    m_outvEngLost[m_stepId] = eng_new_lost;

    // sleep energy
    m_outvFailD[m_stepId] = (m_esEng < (cMdlPars::E_SLEEP/cMdlPars::n_DCDC2));

    m_setESEng(m_esEng - cMdlPars::E_SLEEP/cMdlPars::n_DCDC2);

}

void cSolarMdlSim::m_evalMeas(void)
{
    // measurment evaluation
    bool meas_ok = (m_esEng >= ((cMdlPars::E_MEA/cMdlPars::n_DCDC2) + (cMdlPars::E_NVM/cMdlPars::n_DCDC2)));

    if(meas_ok)
    {
        m_setESEng(m_esEng - ((cMdlPars::E_MEA/cMdlPars::n_DCDC2) + (cMdlPars::E_NVM/cMdlPars::n_DCDC2)));
        m_buffSize++;
    }
    else
        m_sysReset = true;

    if(m_buffSize > cMdlPars::BuffSizeMax)
    {
        m_buffSize = cMdlPars::BuffSizeMax;
        m_buffLost++;
    }
    m_outvFailM[m_stepId] = !meas_ok;
}

void cSolarMdlSim::m_evalTransmit(void)
{
    // transmission evaluation
    int tx_payload = 0;
    bool tx_ok = true;

    if(m_stepId >= m_nextTx)
    {
        double eng_req_tx = ((m_buffSize / cMdlPars::Smpl_TX32B)*(cMdlPars::E_TX32B/cMdlPars::n_DCDC2));
        int buffSize_rem = m_buffSize%cMdlPars::Smpl_TX32B;
        if(buffSize_rem > 0 && buffSize_rem <= 2)
            eng_req_tx += cMdlPars::E_TX8B/cMdlPars::n_DCDC2;
        else if (buffSize_rem > 0)
            eng_req_tx += cMdlPars::E_TX32B/cMdlPars::n_DCDC2;

        tx_ok = (m_esEng >= eng_req_tx) & !m_sysReset;
        if(tx_ok)
        {
            m_setESEng(m_esEng - eng_req_tx);
            tx_payload = m_buffSize;
            m_buffSize = 0u;
        }
        else
        {
            m_sysReset = true;
        }
    }

    m_outvSoes[m_stepId] = m_esSoc;
    m_txOk = tx_ok && (tx_payload > 0);
    m_outvTxPayload[m_stepId] = tx_payload;
    m_outvFailT[m_stepId] = !tx_ok;
}

void cSolarMdlSim::m_evalLogging(void)
{
    m_outvBuffLost[m_stepId] = m_buffLost;

    m_outvBuffSize[m_stepId] = m_buffSize;
    m_outvSoes[m_stepId] = m_esSoc;
}
