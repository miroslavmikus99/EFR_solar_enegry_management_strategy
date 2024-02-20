#ifndef EFRCTRLR_H
#define EFRCTRLR_H

#include "solarSim.h"

class cEfrCtrlr : cEfrCtrlI
{
public:
	// average, current, average
    double efrCompute(double soes_avg[], double soes_curr, double e_avg[]) override
    {
        return 0.19f;
    }
};

#endif // EFRCTRLR_H

