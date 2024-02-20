#ifndef CGENPROG_H_
#define CGENPROG_H_

#include "cData.h"
#include "model/cModel.h"

#include "cForest.h"

#include <arg/algorithms/ga/cGA.h>

class cGenProg : public arg::cGA
{
		cModel & m_Model;
		cData & m_Data;

		cForest::t_FitnessType m_FitnessType;

	public:
		cGenProg(cForest::t_FitnessType fit_type, const unsigned int pop_size, cData & data, cModel & model, const bool debug = false);

		virtual void Shuffle(void);

		virtual ~cGenProg();
};

#endif /* CGENPROG_H_ */
