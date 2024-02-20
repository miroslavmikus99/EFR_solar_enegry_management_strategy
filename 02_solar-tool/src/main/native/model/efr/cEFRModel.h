#ifndef CEFRMODEL_H_
#define CEFRMODEL_H_

#include "../cModel.h"
#include "../solarSim.h"

class cEFRModel: public cModel
{
	public:
		cSolarMdlSim * m_Solar;

	private:
		virtual bool ExecuteInstruction(const t_Instruction & instruction, cStack<double> & stack, const double * input, const unsigned int row_idx, const unsigned int row_width, const unsigned int input_len, double * estimates);
		virtual void PrintInstruction(const t_Instruction & instruction);
		virtual t_Instruction RandomInstruction(const unsigned int inputs, const unsigned int targets, const double terminal_probability);
		virtual void DottifyInstruction(const t_Instruction & instruction, cStack<unsigned int> & stack, const unsigned int idx);

		inline double FuzzyThreshold(const double d, const double weight);

	public:
		cEFRModel(void);

		void SolarModel(cSolarMdlSim * model) {m_Solar = model;};
		cSolarMdlSim * VideoModel(void) {return m_Solar;};

		virtual t_Instruction RandomInstruction(const unsigned int arity, const unsigned int inputs = 0, const unsigned int targets = 0);
		virtual t_Instruction ParseInstruction(char* token);
		virtual void MutateInstruction(t_Instruction & instruction, const unsigned int inputs, const unsigned int targets);

		virtual bool Execute(const t_Instruction * start, const unsigned int len, cData & data, double * estimates, const unsigned int target_idx);

		double ExecuteOnce(const t_Instruction * start, const unsigned int len, const double * input, const unsigned int input_len, double * estimates);

		virtual ~cEFRModel(void);
};

inline double cEFRModel::FuzzyThreshold(const double d, const double a)
{
	double g = d;

	double P_a = (1 + a) / 2.0;
	double Q_a = (1 - a * a) / 4.0;

	//threshold
	if (a > d)
		g = (double) (P_a * d) / a;
	else
		g = P_a + Q_a * ((double) (d - a) / (1.0 - a));

	return g;
}


#endif /* CEFRMODEL_H_ */
