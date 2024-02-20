#include "cModel.h"
#include <arg/utils/cRandom.h>

#include <iostream>

using namespace std;

cModel::cModel()
{
	m_NotIsAllowed = true;
	m_PastInputLimit = 0;
	m_PastOutputLimit = 0;
	m_Beta = 1.0;
	m_Nontrivial = false;
	m_MaxTreeInstructions = 2000;
}

void cModel::RandomTerminalInstruction(t_Instruction & instruction, const unsigned int attribute_count, const unsigned int target_count)
{

	instruction.type = INPUT_INSTRUCTION;
	instruction.value = RandomIndex(attribute_count);

	double rand = 0;

	if (attribute_count == 0)
	{
		rand = 0.3;
		// cout << ">" << rand << endl;
	}
	else
	{
		rand = arg::cStaticRandom::Next(1.0);
	}
	// dbg << "AC: " << attribute_count << ", iv: " << instruction.value << endl;

	// cout << rand << " " << m_PastInputLimit << " " << m_PastOutputLimit << endl;

	if (rand < 0.25 && m_PastInputLimit > 0) // make it past input
	{
		// cout << "a" << endl;
		instruction.extra_uint = 1 + arg::cStaticRandom::NextInt(m_PastInputLimit - 2);
		instruction.type = PAST_INPUT_INSTRUCTION;
	}
	else if (rand < 0.5 && m_PastOutputLimit > 0) // make it past output, can be any target
	{
		// cout << "b" << endl;
		instruction.extra_uint = 1 + arg::cStaticRandom::NextInt(m_PastOutputLimit - 2);
		instruction.value = RandomIndex(target_count);
		instruction.type = PAST_OUTPUT_INSTRUCTION;
	}

	// dbg << "I: " << instruction.type << ", " << instruction.value << endl;
}

bool cModel::Execute(const t_Instruction * start, const unsigned int len, cData & data, double * estimates,
		const unsigned int target_idx)
{
	const unsigned int M = data.Records();
	const unsigned int row_width = data.Inputs() + data.Targets();
	const unsigned int input_len = data.Inputs();

	if (IsDebugging())
	{
		dbg << "Executing: \n ";
		Print(start, len);
		_dbg << endl;
	}

	for (unsigned int row_idx = 0; row_idx < M; row_idx++)
	{
		const double * input = data.Inputs(row_idx);

		m_Stack.Clear();
		unsigned int current = 0;

		do
		{
			// cout << ">> " << input_len << "; " << row_width << "; " << data.Targets() << endl;
			ExecuteInstruction(start[current], m_Stack, input, row_idx, row_width, input_len,
					&estimates[row_idx * data.Targets()]);
			current++;
		} while (current < len);

		estimates[row_idx * data.Targets() + target_idx] = m_Stack.Pop();

		if (m_Stack.Count() > 0)
		{
			err << "Something went wrong. Stack size is " << m_Stack.Count() << " instead of 0.\n";
			return false;
		}
	}

	return true;
}

void cModel::Print(const t_Instruction * start, const unsigned int len)
{
	unsigned int current = 0;
	do
	{
		// cout << current << ">";
		PrintInstruction(start[current]);
		current++;
	} while (current < len);
}

void cModel::Dot(const t_Instruction * start, const unsigned int len)
{
	cStack<unsigned int> stack;

	unsigned int current = 0;

	do
	{
		DottifyInstruction(start[current], stack, current);
		current++;
	} while (current < len);
}

arg::cArrayConst<t_Instruction> cModel::RandomTree(const unsigned int attribute_count, const unsigned int target_count)
{
	cStack<t_Instruction> stack;

	RandomTree(attribute_count, target_count, 0.2, stack, true);

	arg::cArrayConst<t_Instruction> tree;
	tree.Add(stack.TopPtr(), stack.Count());

	return tree;
}

void cModel::RandomTree(const unsigned int attribute_count, const unsigned int target_count,
		double terminal_probability, cStack<t_Instruction> & stack, const bool is_first)
{
	t_Instruction node;

	if (m_Nontrivial && is_first)
	{
		// generate at least one non-terminal
		node = RandomInstruction(attribute_count, target_count, 0.0);
	}
	else
	{
		// do not care
		node = RandomInstruction(attribute_count, target_count, terminal_probability);
	}

	unsigned int node_arity = node.type / 100;

	for (unsigned int i = 0; i < node_arity; i++)
	{
		RandomTree(attribute_count, target_count, terminal_probability * 1.1, stack, false);
	}

	stack.Push(node);
}

void cModel::Compact(arg::cArrayConst<t_Instruction> & instructions)
{
	arg::cArrayConst<t_Instruction> compact;

	for (unsigned int i = 0; i < instructions.Count(); i++)
	{
		if (instructions[i].type != NOOP_INSTRUCTION)
		{
			compact.Append(instructions[i]);
		}
	}
	instructions = compact;
}

cModel::~cModel()
{
}
