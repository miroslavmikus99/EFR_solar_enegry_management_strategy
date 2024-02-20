#include "cForest.h"
#include <arg/utils/cRandom.h>

#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace arg;

cForest::cForest(cData & data, cModel & model, t_FitnessType fit_type) :
		m_Data(data), m_Model(model), m_FitnessType(fit_type)
{
	m_Records = m_Data.Records();
	m_Inputs = m_Data.Inputs();
	m_Targets = m_Data.Targets();
	m_RowLength = m_Inputs + m_Targets;
	m_Estimates = new double[m_Records * m_Targets];
	m_LeftOutIdx = m_Data.LeaveOutIdx();
	m_Beta = m_Model.Beta();
	m_MaxTreeInstructions = m_Model.MaxTreeInstructions();
	GenenerateTestForest();
}

void cForest::RandEstimates(void)
{
	for (unsigned int i = 0; i < m_Records * m_Targets; i++)
	{
		m_Estimates[i] = arg::cStaticRandom::Next(1);
	}
}

void cForest::StealEstimates(void)
{
	for (unsigned int i = 0; i < m_Records; i++)
	{
		double * targets = m_Data.Targets(i);
		for (unsigned int j = 0; j < m_Targets; j++)
		{
			m_Estimates[i * m_Targets + j] = targets[j] - 0.001;
		}
	}
}

void cForest::GenenerateTestForest(void)
{
	m_Forest.Clear();

	t_Instruction separator;
	separator.type = SEPARATOR_INSTRUCTION;

	for (unsigned int i = 0; i < m_Targets; i++)
	{
		cArrayConst<t_Instruction> tree = m_Model.RandomTree(m_Inputs, m_Targets);

		m_Forest.Add(tree.GetArray(0), tree.Count());
		m_Forest.Add(&separator);
	}
}

bool cForest::ParseForest(char * str)
{
	char * token = strtok(str, " ");

	m_Forest.Clear();

	m_Targets = 0;

	delete[] m_Estimates;

	while (token != NULL)
	{
		// cout << "> " << token << endl;
		t_Instruction inst = m_Model.ParseInstruction(token);
		//cout << "inst: " << inst << "\n";

		if (inst.type == SEPARATOR_INSTRUCTION)
			m_Targets++;

		m_Forest.Append(inst);
		token = strtok(NULL, " ");
	}

	m_Data.TargetCount(m_Targets);

	m_Estimates = new double[m_Records * m_Targets];

	return true;
}

void cForest::Print(void) const
{
	unsigned int rule_start = 0;

	for (unsigned int i = 0; i < m_Forest.Count(); i++)
	{
		if (m_Forest[i].type == SEPARATOR_INSTRUCTION)
		{
			m_Model.Print(m_Forest.GetArray(rule_start), i - rule_start);
			rule_start = i + 1;
			cout << "; ";
		}
	}
	cout << "\n";
}

cIndividual * cForest::Clone(void)
{
	cForest * clone = new cForest(m_Data, m_Model, m_FitnessType);
	clone->m_Beta = m_Beta;
	clone->m_LeftOutIdx = m_LeftOutIdx;

	clone->m_Forest = m_Forest;
	clone->m_Fitness = m_Fitness;
	clone->m_P1 = m_P1;
	clone->m_P2 = m_P2;

	// memcpy
	memcpy(clone->m_Estimates, m_Estimates, sizeof(double) * (m_Records * m_Targets));

	clone->Debug(IsDebugging());

	return clone;
}

void cForest::PrintEstimates(void)
{
	for (unsigned int i = 0; i < m_Records; i++)
	{
		double * targets = m_Data.Targets(i);
		cout << fixed << setprecision(9);
		for (unsigned int j = 0; j < m_Targets; j++)
		{
			cout << targets[j] << "\t" << m_Estimates[i * m_Targets + j] << "\t";
		}

		if (i == m_LeftOutIdx)
			cout << "(LEFT OUT)";

		cout << endl;
	}
}

void cForest::Evaluate(void)
{
	unsigned int rule_start = 0;
	unsigned int target_idx = 0;

	// do not forget to delete the estimates
	memset(m_Estimates, 0, sizeof(double) * m_Records * m_Targets);

	//Print();

	for (unsigned int i = 0; i < m_Forest.Count(); i++)
	{
		if (m_Forest[i].type == SEPARATOR_INSTRUCTION)
		{
			// cout << "." << rule_start << "; " << &m_Data << "; " << m_Estimates << "; " << &m_Model << " " << flush;
			// cout << m_Forest.Count() << " " << target_idx << "; " << i - rule_start << flush;
			if (m_Model.Execute(&m_Forest[rule_start], i - rule_start, m_Data, m_Estimates, target_idx))
			{
				// cout << "+" << endl;
				rule_start = i + 1;
				target_idx++;
			}
			else
			{
				err << "Problem with rule no. " << target_idx << ".\n";
				Print();
				exit(0);
			}
		}
	}
}

void cForest::Surface(void)
{
	unsigned int rule_start = 0;

	// do not forget to delete the estimates
	memset(m_Estimates, 0, sizeof(double) * m_Records * m_Targets);

	double input[2];

	for (unsigned int i = 0; i < m_Forest.Count(); i++)
	{
		if (m_Forest[i].type == SEPARATOR_INSTRUCTION)
		{
			// cout << "." << rule_start << "; " << &m_Data << "; " << m_Estimates << "; " << &m_Model << " " << flush;
			// cout << m_Forest.Count() << " " << target_idx << "; " << i - rule_start << flush;
			for (double k = 0; k <= 1.05; k += 0.05)
			{
				for (double l = 0; l <= 1.05; l += 0.05)
				{
					input[0] = k;
					input[1] = l;
					double res = ((cEFRModel &) m_Model).ExecuteOnce(&m_Forest[rule_start], i - rule_start, input, 2,
							m_Estimates);
					cout << k << "\t" << l << "\t" << res << endl;
				}
			}
		}
	}
}

void cForest::Dot(void)
{
	unsigned int rule_start = 0;
	unsigned int target_idx = 0;

	for (unsigned int i = 0; i < m_Forest.Count(); i++)
	{
		if (m_Forest[i].type == SEPARATOR_INSTRUCTION)
		{
			cout << "digraph query {" << setprecision(3) << " ";
			m_Model.Dot(&m_Forest[rule_start], i - rule_start);
			rule_start = i + 1;
			target_idx++;
			cout << "}" << endl;
		}
	}
}

double cForest::ComputeFitness(void)
{
	Evaluate();
	if(m_FitnessType != FIT_FSCORE2)
	{
		((cEFRModel&) m_Model).m_Solar->calcFitness(&m_P1, &m_P2);
	}
	// cout << m_P1 << "\t" << m_P2 << endl;

	const double P = (1.0 - m_P1);
	const double R = (1.0 - m_P2);

	// cout << P << "\t" << R << endl;

	if (m_FitnessType == FIT_FSCORE)
	{
		m_Fitness = 0;

		if (P + R != 0)
		{
			//use F2 by CJvanRijsbergen
			m_Fitness = (1 + m_Beta * m_Beta) * P * R / (m_Beta * m_Beta * P + R);
		}
	}
	else if (m_FitnessType == FIT_WAVG)
	{
		m_Fitness = 1 - (m_Beta * m_P1 + m_P2) / (m_Beta + 1);
	}
	else if (m_FitnessType == FIT_FSCORE2)
	{
		m_Fitness = 1 - sqrt(m_P2*20*m_P2*20 + m_P1*m_P1);
		if(m_Fitness < 0)
		{
			m_Fitness = 0.0;
		}
	}

	//lets have some penalty if the no. of instructions is too high
	if (m_Forest.Count() > m_MaxTreeInstructions)
	{
		m_Fitness = m_Fitness / ((double) m_Forest.Count() / m_MaxTreeInstructions);
		// cout << "F*" << endl;
	}

	// cout << m_Fitness << endl;

	return m_Fitness;
}

void cForest::Mutate(const unsigned int ignore, const double mut_probability)
{
	unsigned int rule_size = m_Forest.Count();

	if (IsDebugging())
	{
		dbg << "Mutating :\n ";
		Print();
		_dbg << " ->\n ";
	}

	for (unsigned int i = 0; i < rule_size; i++)
	{
		//cout << i << "\t(" << rule_size << ")" << endl;
		if (m_Forest[i].type == SEPARATOR_INSTRUCTION || m_Forest[i].type == NOOP_INSTRUCTION)
			continue;

		double rnd = arg::cStaticRandom::Next(1.0);
		//dbg << rnd << endl;

		if (rnd < mut_probability) // do the mutation
		{
			t_Instruction & current = m_Forest[i];
			unsigned int arity = current.type / 100;

			if ((rnd = arg::cStaticRandom::Next(1.0)) < 0.5) // mutate single instruction (weight and inp. spec. things)
			{
				m_Model.MutateInstruction(current, m_Inputs, m_Targets);
				_dbg << "w  : " << i << endl;
			}
			else if (rnd < 0.6 && m_Model.NotIsAllowed()) // insert unary
			{
				t_Instruction un_op = m_Model.RandomInstruction(1);

				// cout << un_op.type << " " << m_Rule.GetArray(0) << endl;;

				// cout << "iN : " << i << " (" << rule_size << ")" << flush;

				// insert to right of this instruction (it is reverese polish notation)
				if (i + 1 < rule_size && m_Forest[i + 1].type == NOOP_INSTRUCTION)
				{
					m_Forest[i + 1] = un_op;
				}
				else
				{
					m_Forest.Insert(i + 1, &un_op);
					rule_size++;
				}
				// do not mutate this newly inserted node
				i += 1;

				// cout << " -> " << i << " (" << rule_size << ")" << endl;
			}
			else // modify node
			{
				rnd = arg::cStaticRandom::Next(1.0);

				if (rnd < 0.8) // delete unary op or modify single node
				{
					if (arity == 1 && (rule_size > 3 || !m_Model.Nontrivial())) // if unary, delete
					{
						current.type = NOOP_INSTRUCTION;
						_dbg << "dN : " << i << " (" << rule_size << ")" << endl;
					}
					else // replace by compatible (same arity)
					{
						//cout << current.type << " ";
						current = m_Model.RandomInstruction(arity, m_Inputs, m_Targets);
						_dbg << "rS : " << i << " (" << arity << ")" << endl;
					}
				}
				else // replace branch ...
				{
					const unsigned int from = SubtreeLeft(i, arity);

					// this can be optimized if the new tree is bigger than old one - just overwrite and fill with NOOPS
					cArrayConst<t_Instruction> rand_tree = m_Model.RandomTree(m_Inputs, m_Targets);

					_dbg << "rB : " << from << " " << i << " " << rand_tree.Count() << " (" << rule_size << ")" << endl;

					m_Forest.Replace(from, i, rand_tree.GetArray(0), rand_tree.Count());

					rule_size = rule_size - (i + 1 - from) + rand_tree.Count();
					// cout << "   : (" << rule_size << ")" << endl;
				}
			}
		}
	}
	if (IsDebugging())
	{
		Print();
		_dbg << endl;
	}
}

void cForest::Crossover(const unsigned int ignore, arg::cIndividual & o, const double cross_probability)
{
	cArrayConst<t_Instruction> offspring1;
	cArrayConst<t_Instruction> offspring2;

	cForest & other = (cForest &) o;

	unsigned int parent1_i = 0;
	unsigned int parent2_i = 0;

	for (unsigned int i = 0; i < m_Targets; i++)
	{
		if (arg::cStaticRandom::Next(1.0) < cross_probability)
		{
			unsigned int p1_j = parent1_i + NextInstruction(m_Forest.GetArray(parent1_i), SEPARATOR_INSTRUCTION);
			unsigned int p2_j = parent2_i + NextInstruction(other.m_Forest.GetArray(parent2_i), SEPARATOR_INSTRUCTION);

			// unsigned int rand1 = parent1_i;
			// unsigned int rand2 = parent2_i;

			unsigned int rand1 = parent1_i + arg::cStaticRandom::NextInt(p1_j - parent1_i - 1);
			unsigned int rand2 = parent2_i + arg::cStaticRandom::NextInt(p2_j - parent2_i - 1);

			// dbg << rand1 << " " << rand2 << endl;

			if (m_Forest[rand1].type == NOOP_INSTRUCTION || other.m_Forest[rand2].type == NOOP_INSTRUCTION)
			{
				// dbg << "A" << endl;
				// if NOOP was hit, lets skip crossover for now and copy all to offspring
				parent1_i += SelectiveCopy(m_Forest.GetArray(parent1_i), offspring1);
				parent2_i += SelectiveCopy(other.m_Forest.GetArray(parent2_i), offspring2);
			}
			else
			{
				// dbg << "Arity 1: " << m_Forest[rand1].type / 100 << " (" << m_Forest[rand1].type << ")"  << endl;
				// dbg << "Arity 2: " << other.m_Forest[rand2].type / 100 << " (" << other.m_Forest[rand2].type << ")"<< endl;

				unsigned int left1 = SubtreeLeft(rand1, m_Forest[rand1].type / 100);
				unsigned int left2 = other.SubtreeLeft(rand2, other.m_Forest[rand2].type / 100);

				if (m_Forest[rand1].type == PROCESS_ALL_INSTRUCTION)
				{
					left1 = parent1_i;
				}

				if (other.m_Forest[rand2].type == PROCESS_ALL_INSTRUCTION)
				{
					left2 = parent2_i;
				}

				// dbg << "C" << endl;
				_dbg << " " << parent1_i << ", " << p1_j << " (" << rand1 << ") -> " << left1 << ", " << rand1 << endl;
				_dbg << " " << parent2_i << ", " << p2_j << " (" << rand2 << ") -> " << left2 << ", " << rand2 << endl;

				if (m_Model.Nontrivial())
				{
					const unsigned int length1 = rand1 - left1;
					const unsigned int length2 = rand2 - left2;
					if (((length1 == 0) && (length2 == p2_j - parent2_i - 1))
							|| ((length2 == 0) && (length1 == p1_j - parent1_i - 1)))
					{
						// invalid crossover, lets skip crossover for now and copy all to offspring, like with NOOPS
						parent1_i += SelectiveCopy(m_Forest.GetArray(parent1_i), offspring1);
						parent2_i += SelectiveCopy(other.m_Forest.GetArray(parent2_i), offspring2);
						continue;
					}
				}

				// copy part before crossover
				if (parent1_i != left1)
					// cout << ">" << (left1 - parent1_i - 1) << endl;
					SelectiveCopyN(m_Forest.GetArray(parent1_i), offspring1, left1 - parent1_i);

				//cout << ">>" << endl;
				if (parent2_i != left2)
					SelectiveCopyN(other.m_Forest.GetArray(parent2_i), offspring2, left2 - parent2_i);

				// dbg << "1" << endl;

				// copy crossover part
				SelectiveCopyN(other.m_Forest.GetArray(left2), offspring1, rand2 - left2 + 1);
				// dbg << "2" << endl;
				SelectiveCopyN(m_Forest.GetArray(left1), offspring2, rand1 - left1 + 1);
				// dbg << "3" << endl;
				// copy after crossover part
				SelectiveCopy(m_Forest.GetArray(rand1 + 1), offspring1);
				// dbg << "4" << endl;
				SelectiveCopy(other.m_Forest.GetArray(rand2 + 1), offspring2);
				// dbg << "5" << endl;

				parent1_i = p1_j + 1;
				parent2_i = p2_j + 1;
			}
			//cout << "OK " << i << endl;
		}
		else
		{
			// we do this piece by piece to eliminate NOOPs
			parent1_i += SelectiveCopy(m_Forest.GetArray(parent1_i), offspring1);
			parent2_i += SelectiveCopy(other.m_Forest.GetArray(parent2_i), offspring2);
		}
	}

	if (IsDebugging())
	{
		Print();
		_dbg << "x\n";
		other.Print();
		_dbg << "=\n";
	}

	if (!m_Model.Nontrivial() || (offspring1.Count() > 2 && offspring2.Count() > 2))
	{
		m_Forest = offspring1;
		other.m_Forest = offspring2;
	} //otherwise simply skip

	if (IsDebugging())
	{
		Print();
		_dbg << "+\n";
		other.Print();
	}

//	commented for performance, should not happen
//	if (m_Model.Nontrivial() && (other.m_Forest.Count() == 2 || m_Forest.Count() == 2))
//	{
//		err << "This is error." << endl;
//		exit(0);
//	}
}

void cForest::GetParams(double * params, const unsigned int size)
{
	unsigned int j = 0;
	for (unsigned int i = 0; i < m_Forest.Count(); i++)
	{
		if (m_Forest[i].type != NOOP_INSTRUCTION && m_Forest[i].type != SEPARATOR_INSTRUCTION)
		{
			params[j++] = m_Forest[i].weight;
			if (j == size) // this should not happen ...
				break;
		}
	}
}

cForest::~cForest()
{
	delete[] m_Estimates;
}
