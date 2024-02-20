/**
 * \class cModel
 * \brief A generic data mining 'model' based on a stack representation of a tree-like classifier.
 *
 *  The cModel defines an interface for
 *
 *  \author Pavel Kromer, (c) - 2016
 */

#ifndef CMODEL_H_
#define CMODEL_H_

#include "../cStack.h"
#include "../cData.h"
#include <arg/core/cDebuggable.h>
#include <arg/utils/cRandom.h>

// lets encode the arity into node codes (integer div by 100)
enum t_InstructionType
{
	//special
	NOOP_INSTRUCTION = -2,
	SEPARATOR_INSTRUCTION = -1,

	// zero arity
	INPUT_INSTRUCTION = 0,
	PAST_INPUT_INSTRUCTION,
	PAST_OUTPUT_INSTRUCTION,

	// unary
	NOT_INSTRUCTION = 100,

	// for instruction that procsses everything on stack use arity 1 (at least one input required),
	// used in FNT
	PROCESS_ALL_INSTRUCTION = 199,

	// binary
	AND_INSTRUCTION = 200,
	OR_INSTRUCTION,
	SUM_INSTRUCTION,
	PROD_INSTRUCTION,

	// PROCESS_ALL_INSTRUCTION = 299,
};

struct t_Instruction
{
	t_InstructionType type;
	unsigned int value;
	unsigned int extra_uint;
	double weight;
	double extra_dbl;
	double extra_dbl_2;
};

class cModel : public arg::cDebuggable
{
	protected:
		double m_Beta;

		bool m_NotIsAllowed;
		bool m_Nontrivial;


		unsigned int m_PastInputLimit;
		unsigned int m_PastOutputLimit;
		unsigned int m_MaxTreeInstructions;

		cStack<double> m_Stack;

		/** Execute single instruction. */
		virtual bool ExecuteInstruction(const t_Instruction & instruction, cStack<double> & stack, const double * input, const unsigned int row_idx, const unsigned int row_width, const unsigned int input_len, double * estimates) = 0;

		/** Print single instruction. */
		virtual void PrintInstruction(const t_Instruction & instruction) = 0;

		/** Generate random instruction. */
		virtual t_Instruction RandomInstruction(const unsigned int inputs, const unsigned int targets, const double terminal_probability) = 0;

		/** Represent model instruction in the Dot language. */
		virtual void DottifyInstruction(const t_Instruction & instruction, cStack<unsigned int> & stack, const unsigned int idx) = 0;

		/** Generate random (sub)tree, it store to stack */
		void RandomTree(const unsigned int inputs, const unsigned int targets, double terminal_probability, cStack<t_Instruction> & stack, const bool is_first);

		void RandomTerminalInstruction(t_Instruction & instruction, const unsigned int inputs, const unsigned int targets);
		inline unsigned int RandomIndex(const unsigned int max_val);

	public:
		cModel(void);

		void Beta(const double val) {m_Beta = val;};
		double Beta(void) {return m_Beta;};

		void NotIsAllowed(const bool val) { m_NotIsAllowed = val;};
		bool NotIsAllowed() { return m_NotIsAllowed;};
		void Nontrivial(const bool val) { m_Nontrivial = val;};
		bool Nontrivial(void) { return m_Nontrivial;};

		unsigned int MaxTreeInstructions(void){return m_MaxTreeInstructions;};
		void MaxTreeInstructions(unsigned int val){m_MaxTreeInstructions = val;};

		void PastInputLimit(const unsigned int val) { m_PastInputLimit = val;};
		void PastOutputLimit(const unsigned int val) { m_PastOutputLimit = val;};

		virtual bool Execute(const t_Instruction * start, const unsigned int len, cData & data, double * estimates, const unsigned int target_idx);
		void Dot(const t_Instruction * start, const unsigned int len);

		void Print(const t_Instruction * start, const unsigned int len);

		virtual t_Instruction RandomInstruction(const unsigned int arity, const unsigned int inputs = 0, const unsigned int targets = 0) = 0;
		virtual t_Instruction ParseInstruction(char* token) = 0;
		virtual void MutateInstruction(t_Instruction & instruction, const unsigned int inputs, const unsigned int targets) = 0;

		arg::cArrayConst<t_Instruction> RandomTree(const unsigned int inputs, const unsigned int targets);

		virtual void Compact(arg::cArrayConst<t_Instruction> & instructions);

		virtual ~cModel();
};

inline unsigned int cModel::RandomIndex(const unsigned int max_val)
{
	if (max_val < 2)
		return 0;
	else
		return arg::cStaticRandom::NextInt(max_val - 1);
}

#endif /* CMODEL_H_ */
