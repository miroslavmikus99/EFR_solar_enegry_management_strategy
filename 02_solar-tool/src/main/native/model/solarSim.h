/*
 * solarSim.h
 *
 *  Created on: 9. 8. 2022
 *      Author: Mirek Mikus
 */

#ifndef SIMULATOR_SOLARSIM_H_
#define SIMULATOR_SOLARSIM_H_

#include <fstream>
#include <ctime>
#include <sstream>
#include <iostream>

using namespace std;

class cEfrCtrlI
{
public:
    virtual double efrCompute(double soes_avg[], double soes_curr, double e_avg[]) = 0;
};

class cSimStats
{
public:
    unsigned int FailM;
    unsigned int FailT;
    double BuffSizeAvg;
    double E_Unused;
    unsigned int OvchCnt;
    unsigned int BuffLost;

    unsigned int FailD;
    unsigned int MeasOk;
    unsigned int TransOk;


    cSimStats(void){};
    void print(void)
    {
        std::cout << "FailD = " << this->FailD << "\n";
        std::cout << "FailT = " << this->FailT << "\n";
        std::cout << "FailM = " << this->FailM << "\n";
        std::cout << "TransOk = " << this->TransOk << "\n";
        std::cout << "MeasOk = " << this->MeasOk << "\n";
        std::cout << "OvchCnt = " << this->OvchCnt << "\n";
        std::cout << "E_unused = " << this->E_Unused << "\n";
        std::cout << "BuffLost = " << this->BuffLost << "\n";
        std::cout << "BuffSizeAvg = " << this->BuffSizeAvg << "\n";
    };

};

class cSolarMdlSim
{
public:
    cSolarMdlSim(cEfrCtrlI *efrContext);
    cSolarMdlSim();

    void initSim(void);
    void initSimEfr(void);
    bool loadDataFile(const char *fname);
    void saveSimOuts(const char *fname);
    void simRun(void);
    void simSingleCycle(void);
    bool simSingleCycleEfr(double nextTx);
    void finishSimEfr(void);
    void getCtrlrInputs(double* soesAvg, double* soesCurr, double* eAvg);
    unsigned int getDataLength(void);
    void calcFitness(double *p1, double *p2);
    void calcFitness2(double *p1, double *p2);
    cSimStats* calcStats(void);
    void calcStats(cSimStats*);

    ~cSolarMdlSim(void);

private:

    const char m_dataFileDelimiter = ';';
    cEfrCtrlI *m_efrContext;

    struct t_DataRow
    {
        time_t timestamp;
        unsigned short val_Pd;
    };

    t_DataRow *m_dataSet = NULL;
    unsigned int m_dataSetLen = 0;

    /* sim vars */
    double m_esSoc;
    double m_esEng;
    unsigned int m_stepId;
    unsigned int m_nextTx;
    unsigned int m_buffSize;
    unsigned int m_buffLost;
    bool m_sysReset;
    double m_engNewPot;
    bool m_txOk;

    /* out vars */
    double *m_outvEngLost = NULL;
    double *m_outvEngHarv = NULL;
    double *m_outvSoes = NULL;
    unsigned int *m_outvBuffSize = NULL;
    unsigned short *m_outvNextPeriod = NULL;
    unsigned short *m_outvTxPayload = NULL;
    unsigned int *m_outvBuffLost = NULL;
    bool *m_outvFailM = NULL;
    bool *m_outvFailT = NULL;
    bool *m_outvFailD = NULL;


    void m_setESEng(double eng);
    void m_setESSoc(double soc);
    void m_free(void* ptr);
    void m_compSoesAvg(double soesAvg[]);
    void m_compEAvg(double eAvg[]);
    void m_evalSolarEnergy(void);
    void m_evalMeas(void);
    void m_evalTransmit(void);
    void m_evalLogging(void);


};

#endif /* SIMULATOR_SOLARSIM_H_ */
