#include "cEFRModel.h"
#include <arg/utils/cRandom.h>

using namespace std;

cEFRModel::cEFRModel() : m_Solar(NULL)
{
}

void cEFRModel::DottifyInstruction(const t_Instruction & instruction, cStack<unsigned int> & stack,
		const unsigned int idx)
{
	if (instruction.type != NOOP_INSTRUCTION)
	{
		cout << idx << " [ label = \"";
		PrintInstruction(instruction);
		cout << " \" ]; ";

		switch (instruction.type)
		{
		case INPUT_INSTRUCTION:
		case PAST_INPUT_INSTRUCTION:
		case PAST_OUTPUT_INSTRUCTION:
			break;
		case NOT_INSTRUCTION:
		{
			unsigned int val = stack.Pop();
			cout << idx << " -> " << val << "; ";
			break;
		}
		case AND_INSTRUCTION:
		case OR_INSTRUCTION:
		case SUM_INSTRUCTION:
		case PROD_INSTRUCTION:
		{
			unsigned int val = stack.Pop();
			cout << idx << " -> " << val << "; ";
			val = stack.Pop();
			cout << idx << " -> " << val << "; ";
			break;
		}
		case NOOP_INSTRUCTION:
			break;
		case SEPARATOR_INSTRUCTION:
		default:
			err << "Unknown or unexpected instruction: " << instruction.type << ".\n";
			break;
		}
		stack.Push(idx);
	}
}

bool cEFRModel::ExecuteInstruction(const t_Instruction & instruction, cStack<double> & stack, const double * input,
		const unsigned int row_idx, const unsigned int row_width, const unsigned int input_len, double * estimates)
{
	// cout << "" << instruction.type << "; " << instruction.value << "; "<< instruction.extra_uint << endl;
	const double weight = instruction.weight;

	// dbg << "" << endl;

	//	if (IsDebugging())
	//	{
	//		PrintInstruction(instruction);
	//		cout << endl;
	//	}

	switch (instruction.type)
	{
	case INPUT_INSTRUCTION:
		stack.Push(FuzzyThreshold(input[instruction.value], weight));
		break;
	case PAST_INPUT_INSTRUCTION:
	{
		const unsigned int back_count = instruction.extra_uint;
		double val = 0;
		if (row_idx > back_count)
		{
			// std::cout << "+" << row_idx << "\t" << back_count << "\t" << row_width << std::endl;
			val = (input - back_count * row_width)[instruction.value];
		}
		else
		{
			// std::cout << "*" << std::endl;
			// just take the term value
			val = input[instruction.value];
		}

		stack.Push(FuzzyThreshold(val, weight));
		break;
	}
	case PAST_OUTPUT_INSTRUCTION:
	{
		const unsigned int back_count = instruction.extra_uint;
		const unsigned int targets = row_width - input_len;
		double val = 0;
		if (row_idx > back_count)
		{
			// std::cout << "+" << row_idx << "\t" << back_count << "\t" << targets << std::endl;
			val = (estimates - targets * back_count)[instruction.value];
			// std::cout << "=" << val << endl;
		}
		else
		{
			// std::cout << "*" << row_idx << "\t" << back_count << "\t" << targets << std::endl;
			val = 0.0;
			if (input_len == 0)
			{
				// a pure time-series ...
				val = input[instruction.value];
			}
		}

		stack.Push(FuzzyThreshold(val, weight));
		break;
	}
	case NOT_INSTRUCTION:
	{
		double val = stack.Pop();
		stack.Push(FuzzyThreshold(1 - val, weight));
		break;
	}
	case AND_INSTRUCTION:
	{
		double a = stack.Pop();
		double b = stack.Pop();
		stack.Push(FuzzyThreshold((a < b) ? a : b, weight));
		break;
	}
	case OR_INSTRUCTION:
	{
		double a = stack.Pop();
		double b = stack.Pop();
		stack.Push(FuzzyThreshold((a < b) ? b : a, weight));
		break;
	}
	case SUM_INSTRUCTION:
	{
		double a = stack.Pop();
		double b = stack.Pop();
		// probabilistic sum
		stack.Push(FuzzyThreshold(a + b - a * b, weight));
		break;
	}
	case PROD_INSTRUCTION:
	{
		double a = stack.Pop();
		double b = stack.Pop();
		stack.Push(FuzzyThreshold(a * b, weight));
		break;
	}
	case NOOP_INSTRUCTION:
		break;
	case SEPARATOR_INSTRUCTION:
	default:
		err << "Unknown or unexpected instruction: " << instruction.type << ".\n";
		break;
	}

	return true;
}

void cEFRModel::PrintInstruction(const t_Instruction & instruction)
{
	switch (instruction.type)
	{
	case INPUT_INSTRUCTION:
		cout << "t" << instruction.value << ":" << instruction.weight << " ";
		break;
	case PAST_INPUT_INSTRUCTION:
		cout << "t" << instruction.value << "[" << instruction.extra_uint << "]:" << instruction.weight << " ";
		break;
	case PAST_OUTPUT_INSTRUCTION:
		cout << "o" << instruction.value << "[" << instruction.extra_uint << "]:" << instruction.weight << " ";
		break;
	case NOT_INSTRUCTION:
		cout << "not" << ":" << instruction.weight << " ";
		break;
	case AND_INSTRUCTION:
		cout << "and" << ":" << instruction.weight << " ";
		break;
	case OR_INSTRUCTION:
		cout << "or" << ":" << instruction.weight << " ";
		break;
	case SUM_INSTRUCTION:
		cout << "sum" << ":" << instruction.weight << " ";
		break;
	case PROD_INSTRUCTION:
		cout << "prod" << ":" << instruction.weight << " ";
		break;
	case NOOP_INSTRUCTION:
		cout << "x ";
		break;
	case SEPARATOR_INSTRUCTION:
	default:
		err << "Unknown or unexpected instruction: " << instruction.type << ".\n";
		break;
	}
}

t_Instruction cEFRModel::ParseInstruction(char * token)
{
	t_Instruction instruction;

	unsigned int i = 0;

	switch (token[i])
	{
	case 't':
		instruction.type = INPUT_INSTRUCTION;
		break;
	case 'o':
		if (token[i + 1] == 'r')
		{
			instruction.type = OR_INSTRUCTION;
			i += 2;
		}
		else
		{
			instruction.type = PAST_OUTPUT_INSTRUCTION;
		}
		break;
	case 'x':
		instruction.type = NOOP_INSTRUCTION;
		break;
	case ';':
		instruction.type = SEPARATOR_INSTRUCTION;
		break;
	default: // is operator

		while (token[i] != ':')
			i++;

		token[i] = 0;

		if (strcmp(token, "not") == 0)
		{
			instruction.type = NOT_INSTRUCTION;
		}
		else if (strcmp(token, "and") == 0)
		{
			instruction.type = AND_INSTRUCTION;
		}
		else if (strcmp(token, "or") == 0)
		{
			instruction.type = OR_INSTRUCTION;
		}
		else if (strcmp(token, "prod") == 0)
		{
			instruction.type = PROD_INSTRUCTION;
		}
		else if (strcmp(token, "sum") == 0)
		{
			instruction.type = SUM_INSTRUCTION;
		}
		else
		{
			err << "Unknown operator: \'" << token << "\'.\n";
		}
	}

	if (instruction.type == INPUT_INSTRUCTION || instruction.type == PAST_OUTPUT_INSTRUCTION)
	{
		i++;
		instruction.value = atoi((const char *) &token[i]);

		while (token[i] != ':' && token[i] != '[')
			i++;

		if (token[i] == '[')
		{
			if (instruction.type == INPUT_INSTRUCTION)
				instruction.type = PAST_INPUT_INSTRUCTION;

			i++;
			instruction.extra_uint = atoi((const char*) &token[i]);

			while (token[i] != ':')
				i++;
		}

	}

	// get weight
	i++;
	instruction.weight = atof((const char *) &token[i]);

	return instruction;
}

t_Instruction cEFRModel::RandomInstruction(const unsigned int inputs, const unsigned int targets,
		const double terminal_probability)
{
	t_Instruction instruction;

	double rand = arg::cStaticRandom::Next(1.0);

	// cout << attribute_count << " " << rand << "; " << terminal_probability << endl;
	if (rand < terminal_probability)
	{
		RandomTerminalInstruction(instruction, inputs, targets);
		// cout << attribute_count << " " << rand << "; " << instruction.type << endl;

	}
	else if ((rand = arg::cStaticRandom::Next(1.0)) < 0.2 && m_NotIsAllowed) // gen. unary op
	{
		instruction.type = NOT_INSTRUCTION;
	}
	else // gen. binary op
	{
		rand = arg::cStaticRandom::Next(1.0);

		if (rand < 0.25)
		{
			instruction.type = AND_INSTRUCTION;
		}
		else if (rand < 0.5)
		{
			instruction.type = OR_INSTRUCTION;
		}
		else if (rand < 0.75)
		{
			instruction.type = SUM_INSTRUCTION;
		}
		else
		{
			instruction.type = PROD_INSTRUCTION;
		}
	}

	instruction.weight = arg::cStaticRandom::Next(1.0);

	// PrintInstruction(instruction);
	// cout << "..." << attribute_count << ", " << target_count << endl;

	return instruction;
}

t_Instruction cEFRModel::RandomInstruction(const unsigned int arity, const unsigned int inputs,
		const unsigned int targets)
{
	t_Instruction instruction;
	instruction.weight = arg::cStaticRandom::Next(1.0);

	double rand;

	switch (arity)
	{
	case 0:
		RandomTerminalInstruction(instruction, inputs, targets);
		break;
	case 1:
		instruction.type = NOT_INSTRUCTION;
		break;
	case 2:
		rand = arg::cStaticRandom::Next(1.0);

		if (rand < 0.25)
		{
			instruction.type = AND_INSTRUCTION;
		}
		else if (rand < 0.5)
		{
			instruction.type = OR_INSTRUCTION;
		}
		else if (rand < 0.75)
		{
			instruction.type = SUM_INSTRUCTION;
		}
		else
		{
			instruction.type = PROD_INSTRUCTION;
		}
	}
	return instruction;
}

void cEFRModel::MutateInstruction(t_Instruction & instruction, const unsigned int input_count,
		const unsigned int target_count)
{
	double rnd = arg::cStaticRandom::Next(1.0);

	dbg << rnd << " " << input_count << endl;

	if (rnd < 0.5)
	{
		instruction.weight = arg::cStaticRandom::Next(1.0);
	}
	else
	{
		if (instruction.type == INPUT_INSTRUCTION)
		{
			instruction.value = RandomIndex(input_count);
		}
		else if (instruction.type == PAST_INPUT_INSTRUCTION)
		{
			if (rnd < 0.6)
				instruction.extra_uint = 1 + arg::cStaticRandom::NextInt(m_PastInputLimit - 2);
			else
				instruction.value = RandomIndex(input_count);
		}
		else if (instruction.type == PAST_OUTPUT_INSTRUCTION)
		{
			if (rnd < 0.6)
				instruction.extra_uint = 1 + arg::cStaticRandom::NextInt(m_PastOutputLimit - 2);
			else
				instruction.value = RandomIndex(target_count);
		}
		else // only mutate weight
		{
			instruction.weight = arg::cStaticRandom::Next(1.0);
		}
	}
}

bool cEFRModel::Execute(const t_Instruction * start, const unsigned int len, cData & data, double * estimates,
		const unsigned int target_idx)
{
	// MAKE SURE THAT data has dimension ROWS x 4
	// and Targets is 1

	(void) target_idx; // this is just to remove the warning

	const unsigned int M = m_Solar->getDataLength() - 1;
	const unsigned int row_width = 4;
	const unsigned int input_len = 3;

	double soesAvg, soesCurr, eAvg;

	if (IsDebugging())
	{
		dbg << "Executing: \n ";
		Print(start, len);
		_dbg << endl;
	}

	m_Solar->initSimEfr();

	double nextTx;

	for (unsigned int row_idx = 0; row_idx < M; row_idx++)
	{

		m_Solar->getCtrlrInputs(&soesAvg, &soesCurr, &eAvg);

		double * input = data.Inputs(row_idx);
		input[0] = soesAvg;
		input[1] = soesCurr;
		input[2] = eAvg;
	
		m_Stack.Clear();
		unsigned int current = 0;
		do
		{
			ExecuteInstruction(start[current], m_Stack, input, row_idx, row_width, input_len, &estimates[row_idx]);
			current++;
		} while (current < len);

		estimates[row_idx] = m_Stack.Top();
		
		nextTx = m_Stack.Pop();
		input[3] = nextTx;

		m_Solar->simSingleCycleEfr(nextTx);

		if (m_Stack.Count() > 0)
		{
			err << "Something went wrong. Stack size is " << m_Stack.Count() << " instead of 0.\n";
			return false;
		}
	}

	m_Solar->finishSimEfr();
	return true;
}

// note to self: used just for drawing the surface
double cEFRModel::ExecuteOnce(const t_Instruction * start, const unsigned int len, const double * input,
		const unsigned int input_len, double * estimates)
{
	/*
	unsigned int comp_levels = m_Video->m_Video.Rows();
	unsigned int c_level;

	double dat_input[2];
	dat_input[0] = 1 - input[0];
	dat_input[1] = input[1];

	m_Stack.Clear();
	unsigned int current = 0;
	do
	{
		ExecuteInstruction(start[current], m_Stack, dat_input, 0, 2, input_len, &estimates[0]);
		current++;
	} while (current < len);

	c_level = (unsigned int) (m_Stack.Pop() * comp_levels);

	if (m_Stack.Count() > 0)
	{
		err << "Something went wrong. Stack size is " << m_Stack.Count() << " instead of 0.\n";
		return false;
	}
	return c_level;
	*/

	cerr << "ExecuteOnce not implemented!!!" << endl;
	exit(-1);
	return 0;
}

cEFRModel::~cEFRModel()
{
	if (m_Solar != NULL)
		delete m_Solar;
}

