/*
 * cData.cpp
 *
 *  Created on: Nov 28, 2015
 *      Author: pavel
 */

#include "cData.h"

cData::cData() : m_Targets(1), m_Inputs(1), m_LeaveOutIdx((unsigned int) -1)
{
}

cData::cData(const unsigned x, const unsigned y) : m_Targets(1), m_Inputs(1), m_LeaveOutIdx((unsigned int) -1)
{
	m_Data.Resize(x, y, true);
}

void cData::UpdateSigmas(void)
{
	m_Sigmas.Clear();

	for (unsigned int i = 0; i < m_Targets; i++)
	{
		double sigma = 0;
		unsigned int idx = m_Data.Cols() - m_Targets + i;

		for (unsigned int j = 0;j < m_Data.Rows(); j++)
		{
			if (j != m_LeaveOutIdx)
				sigma += m_Data(j, idx);
		}
		m_Sigmas.Append(sigma);
	}
}

bool cData::Load(const char * fname)
{
	bool success = m_Data.Load(fname);
	UpdateSigmas();
	return success;
}

cData::~cData()
{
}

