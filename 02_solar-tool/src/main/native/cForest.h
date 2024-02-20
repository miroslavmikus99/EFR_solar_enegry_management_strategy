#ifndef CFOREST_H_
#define CFOREST_H_

#include "model/efr/cEFRModel.h"
#include <arg/core/cArray.h>

#include <arg/algorithms/ga/cIndividual.h>

class cForest : public arg::cIndividual
{
	public:
		typedef enum {
			FIT_FSCORE = 0,
			FIT_WAVG,
			FIT_FSCORE2 = 2,
		} t_FitnessType;

	private:
		cData & m_Data;
		double * m_Estimates;

		double m_Beta;

		double m_P1;
		double m_P2;

		unsigned int m_Targets;
		unsigned int m_Inputs;
		unsigned int m_Records;
		unsigned int m_RowLength;
		unsigned int m_LeftOutIdx;
		unsigned int m_MaxTreeInstructions;

		cModel & m_Model;

		t_FitnessType m_FitnessType;

		// a forest of rules, each in reverse polish notation
		arg::cArrayConst<t_Instruction> m_Forest;

		inline unsigned int SubtreeLeft(const unsigned int right, const unsigned int arity);
		inline unsigned int SelectiveCopy(t_Instruction * from, arg::cArrayConst<t_Instruction> & to, const t_InstructionType ignore = NOOP_INSTRUCTION, const t_InstructionType stop = SEPARATOR_INSTRUCTION);
		inline unsigned int SelectiveCopyN(t_Instruction * from, arg::cArrayConst<t_Instruction> & to, const unsigned int N, const t_InstructionType ignore = NOOP_INSTRUCTION);
		inline unsigned int NextInstruction(t_Instruction * from, const t_InstructionType target);

	public:
		cForest(cData & data, cModel & model, t_FitnessType = FIT_FSCORE);

		virtual double ComputeFitness(void);
		virtual void Mutate(const unsigned int, const double pM);
		virtual void Crossover(const unsigned int, arg::cIndividual & other, const double pM);
		virtual void Print(void) const;
		virtual arg::cIndividual * Clone(void);
		virtual int Length(void){return m_Forest.Count();};

		void Evaluate(void);
		void Surface(void);
		void Dot(void);
		bool ParseForest(char * str);

		void PrintEstimates(void);
		void Beta(const double val) {m_Beta = val;};

		// For testing only
		void StealEstimates(void);
		void RandEstimates(void);
		void GenenerateTestForest(void);

		void Compact(void);

		void SetParams(const double * params, const unsigned int size);
		void GetParams(double * params, const unsigned int size);

		double P1() {return m_P1;};
		double P2() {return m_P2;};

		unsigned int ParamCount(void);

		virtual ~cForest();
};

inline void cForest::Compact(void)
{
	m_Model.Compact(m_Forest);
}

inline unsigned int cForest::ParamCount(void)
{
	unsigned int cnt = 0;
	for (unsigned int i = 0; i < m_Forest.Count(); i++)
	{
		if (m_Forest[i].type != NOOP_INSTRUCTION && m_Forest[i].type != SEPARATOR_INSTRUCTION)
			cnt++;
	}
	return cnt;
}

inline void cForest::SetParams(const double * params, const unsigned int size)
{
	unsigned int j = 0;
	for (unsigned int i = 0; i < m_Forest.Count(); i++)
	{
		if (m_Forest[i].type != NOOP_INSTRUCTION && m_Forest[i].type != SEPARATOR_INSTRUCTION)
		{
			m_Forest[i].weight = params[j++];
			if (j == size) // this should not happen ...
				break;
		}
	}
}

inline unsigned int cForest::SubtreeLeft(const unsigned int right, const unsigned int arity)
{
	// this will work without tests for well-formed trees
	// we will replace [from, i] in the original tree
	unsigned int required = arity;
	unsigned int from = right;

	while (required > 0)
	{
		from--;
		unsigned int curr_arity = m_Forest[from].type / 100;

		if (m_Forest[from].type != NOOP_INSTRUCTION)
		{
			required--;
			required += curr_arity;
		}
	}
	return from;
}

inline unsigned int cForest::SelectiveCopy(t_Instruction * from, arg::cArrayConst<t_Instruction> & to, const t_InstructionType ignore, const t_InstructionType stop)
{
	unsigned int i = 0;
	do
	{
		if (from[i].type != ignore)
		{
			to.Append(from[i]);
		}
		i++;
	} while (from[i - 1].type != stop);
	return i;
}

inline unsigned int cForest::SelectiveCopyN(t_Instruction * from, arg::cArrayConst<t_Instruction> & to, const unsigned int N, const t_InstructionType ignore)
{
	unsigned int j = 0;

	for (unsigned int i = 0; i < N; i++)
	{
		if (from[i].type != ignore)
		{
			to.Append(from[i]);
			j++;
		}
	}
	return j;
}

inline unsigned int cForest::NextInstruction(t_Instruction * from, const t_InstructionType stop)
{
	unsigned int i = 0;
	while (from[i].type != stop) i++;
	return i;
}

#endif /* CRULE_H_ */
