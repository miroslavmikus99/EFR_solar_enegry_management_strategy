/*
 * solarSim.h
 *
 *  Created on: 9. 8. 2022
 *      Author: Mirek Mikus
 */

#ifndef SIMULATOR_MODEL_PARAMS_H_
#define SIMULATOR_MODEL_PARAMS_H_

class cMdlPars
{
public:
    constexpr static double k_SH = 0.5L;			/* 50 % - Shading coefficient */
    constexpr static double S_PV = 0.000108L;		/* 1,08 cm^2 - Active area of the photovoltaic panel */
    constexpr static double n_PV = 0.21L;			/* 21 % - Efficiency of the photovoltaic panel */
    constexpr static double n_DCDC1 = 0.6L;			/* 60 % - Efficiency of DCDC1 converter */
    constexpr static double C_STORE = 60.0L;		/* 60 J - Capacity of the energy storage */
    constexpr static double n_DCDC2 = 0.5L;			/* 50 % - Efficiency of DCDC2 converter */

    constexpr static double E_SLEEP = 0.016L;       /* 16 mJ - MCU sleep mode (10 min)*/
    constexpr static double E_NVM = 0.000011L;		/* 11 uJ - MCU write to NVM */
    constexpr static double E_MEA = 0.0185L;		/* 100 mJ - Energy for measurement */
    constexpr static double E_TX8B = 0.26L;			/* 260 mJ - LoRaWAN transmission (payload 8 B) */
    constexpr static double E_TX32B = 0.32L;		/* 320 mJ - LoRaWAN transmission (payload 32 B) */


    constexpr static double T_MEAS = 600.0L;		/* 10 min - Measurement period */;
    const static unsigned int Smpl_TX32B = 14u;     /* 14 smpl - Nuzmber of meas samples in 32B packet */
    const static unsigned int Smpl_TX8B = 2u;       /* 2 smpl - Number of meas samples in 8B packet */
    const static unsigned short T_TX_MAX = 144;     /* 144*T_MEAS (1440 min) - Maximal value of transmission period */
    const static unsigned int BuffSizeMax = 434;    /* 434 smpl - Maximal size of data buffer */
    const static unsigned short EfrSoesAvgSize = 1; /* 1 value of hourly averages */
    const static unsigned short EfrEAvgSize = 1;    /* 1 value of daily averages */

    const static unsigned short EfrSoesAvgSmpls = 6; /* 6 smpls*10min = 1 hour, number of samples for average value */
    const static unsigned short EfrEAvgSmpls = 144; /* 144 smpls*10min = 1day, number of samples for average value */
    constexpr static double EfrEAvgMaxVal = 12;     /* Max value of harvested energy */

};

#endif /* SIMULATOR_MODEL_PARAMS_H_ */
