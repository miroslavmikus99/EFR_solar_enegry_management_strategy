#include "cGA.h"

using namespace arg;

cGA::cGA()
{
	m_Daughter = m_Son = NULL;
	m_MutationType = cGA::MUT_CLASSIC;
	m_SelectionType = cGA::SELECT_ELITARY;
	m_CrossoverType = cGA::CROSS_CLASSIC;
	m_MigrationType = cGA::STEADY_STATE;
	m_Prevent = m_Shuffle = m_Minimize = false;
	m_SecondParent = m_FirstParent = 0;
	m_CrossoverProbability = 0.8;
	m_MutationProbability = 0.02;
	m_PopulationSize = 100;
	m_Generations = 100;
}

/**
 * The selection of a pair of parent chromosomes.
 * Three variants of selection are supported:
 * 		- ELITARY selection 		- selects two best chromosomes.
 * 		- ROULETTE WHEEL selection  - selects both parents stochastically
 * 		- SEMI_ELITARY selection	- selects best chromosome + one by roulette wheel
 *
 * The selection type is determined by m_SelectionType.
 */
void cGA::Select(void)
{
	//first best chromosome
	m_FirstParent = m_Minimize ? m_Population.Count() - 1 : 0;
	//second best chromosome
	m_SecondParent = m_Minimize ? m_Population.Count() - 2 : 1;

	dbg << "Selection type is " << m_SelectionType << ".\n";

	if (m_SelectionType != cGA::SELECT_ELITARY)
	{

		if (m_SelectionType == cGA::SELECT_ROULETTE)
		{
			//roulette wheel selection: select first parent stochastically
			m_FirstParent = SelectIndividual();
		} //otherwise SEMI_ELITARY selection will be performed

		unsigned int failed = 0;
		do
		{
			m_SecondParent = SelectIndividual();
			failed++;
		} while (m_FirstParent == m_SecondParent && failed < 10);

		if (m_FirstParent == m_SecondParent)
		{
			dbg << "Failed to select fine second parent stochastically.\n";
			m_SecondParent = (m_FirstParent + 1) % m_Population.Count();
		}
	}
	m_Son = m_Population[m_FirstParent]->Clone();
	m_Daughter = m_Population[m_SecondParent]->Clone();
	dbg << "Selected " << m_FirstParent << " and " << m_SecondParent << ".\n";
}

void cGA::Mutate(const double pM)
{
	m_Son->Mutate(m_MutationType, pM);
	m_Daughter->Mutate(m_MutationType, pM);
}

void cGA::Recombine(const double pC)
{
	double rand = cStaticRandom::Next(1);
	if (rand < pC)
	{
		dbg << "Recombining the chromosomes.\n";
		m_Son->Crossover(m_CrossoverType, *m_Daughter, pC);
	}
}

void cGA::MigrateImpl(unsigned int worst_idx, unsigned int best_idx, const bool prevent_stagnation,
		cIndividual* mChromosome)
{
	// replace according to reverse fitness or simply replace the worst individual
	// just for one moment change the 'polarity' of selection (i.e. when minimizing, select like it was maximizing etc.)
	if (m_MigrationType == cGA::STEADY_STATE_REVERSE_FITNESS)
	{
		m_Minimize = !m_Minimize;
		worst_idx = SelectIndividual();
		m_Minimize = !m_Minimize;
	}

	const cIndividual * mSelected = m_Population[worst_idx];

	if ((mSelected->Fitness() < mChromosome->Fitness() && !m_Minimize)
			|| (mSelected->Fitness() > mChromosome->Fitness() && m_Minimize))
	{
		if ((prevent_stagnation && mChromosome->Fitness() == m_Population[best_idx]->Fitness())
				|| (prevent_stagnation && m_Population[best_idx]->Equals(*mChromosome)))
		{
			dbg << "The fitness of new chromosome and best chromosome is the same and they are probably identical."
					<< "New chromosome will not be added to the population.\n";
			delete mChromosome;
		}
		else
		{
			dbg << "Adding chromosome to the population.\n";
			delete mSelected;
			m_Population[worst_idx] = mChromosome;
			SortInOne(worst_idx);
		}
	}
	else
	{
		dbg << "Removing chromosome from the GA.\n";
		delete mChromosome;
	}
}

void cGA::Migrate(const bool prevent_stagnation)
{
	int idx = m_Minimize ? 0 : m_Population.Count() - 1;
	int best_idx = m_Minimize ? m_Population.Count() - 1 : 0;
	dbg << "Migration idx " << idx << " migration best_idx " << best_idx << ".\n";
	dbg << "Migrating son to the population.\n";
	MigrateImpl(idx, best_idx, prevent_stagnation, m_Son);
	dbg << "Migrating daughter to the population.\n";
	MigrateImpl(idx, best_idx, prevent_stagnation, m_Daughter);
}

void cGA::ComputeChildFitness(void)
{
	dbg << "Preparing to compute child fitness.\n";
	const double fit1 = m_Son->ComputeFitness();
	const double fit2 = m_Daughter->ComputeFitness();
	dbg << "Child fitness computed: " << fit1 << " " << fit2 << ".\n";
}

void cGA::SortPopulation(void)
{
	cIndividual * mHelp;
	bool change;
	unsigned int i;

	do
	{
		change = false;
		for (i = 0; i < m_Population.Count() - 1; i++)
		{
			double fit1 = m_Population[i]->Fitness();
			double fit2 = m_Population[i + 1]->Fitness();
			if (fit1 < fit2)
			{
				mHelp = m_Population[i];
				m_Population[i] = m_Population[i + 1];
				m_Population[i + 1] = mHelp;
				change = true;
			}
		}
	} while (change != false);
}

//Bubbles population once
void cGA::SortInOne(const unsigned int from, const bool reverse)
{
	cIndividual * mHelp;

	if (reverse)
	{
		for (unsigned int i = from; i < m_Population.Count() - 1; i++)
		{
			double fit1 = m_Population[i]->Fitness();
			double fit2 = m_Population[i + 1]->Fitness();
			if (fit1 < fit2)
			{
				mHelp = m_Population[i];
				m_Population[i] = m_Population[i + 1];
				m_Population[i + 1] = mHelp;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		for (unsigned int i = from; i >= 1; i--)
		{
			double fit1 = m_Population[i]->Fitness();
			double fit2 = m_Population[i - 1]->Fitness();
			if (fit1 > fit2)
			{
				mHelp = m_Population[i];
				m_Population[i] = m_Population[i - 1];
				m_Population[i - 1] = mHelp;
			}
			else
			{
				break;
			}
		}
	}
}

//Bubbles population once
void cGA::SortInOne(const unsigned int position)
{
	if (position == m_Population.Count() - 1)
	{
		SortInOne(position, false);
	}
	else if (m_Population[position]->Fitness() > m_Population[position + 1]->Fitness())
	{
		SortInOne(position, false);
	}
	else
	{
		SortInOne(position, true);
	}
}

unsigned int cGA::SelectIndividual()
{
	double fitSum = 0;

	double maxFitness = m_Population[(unsigned int) 0]->Fitness();

	for (unsigned int i = 0; i < m_Population.Count(); i++)
	{
		double fit = m_Population[i]->Fitness();
		if (m_Minimize)
		{
			fitSum += (maxFitness / fit);
		}
		else // normal
		{
			fitSum += fit;
		}
	}

	dbg << "FitSum = " << fitSum << ".\n";

	double score = cStaticRandom::Next(fitSum);
	dbg << "Score = " << score << ".\n";
	dbg << "Minimize = " << m_Minimize << ".\n";

	int selected = 0;
	fitSum = 0;
	for (unsigned int i = 0; i < m_Population.Count(); i++)
	{
		double bit = 0;
		if (m_Minimize)
			bit = maxFitness / m_Population[i]->Fitness();
		else
			bit = m_Population[i]->Fitness();

		fitSum += bit;

		_dbg << bit << "(" << fitSum << ") " << std::flush;

		if (score < fitSum)
		{
			selected = i;
			break;
		}
	}
	_dbg << std::endl;
	dbg << "Selected individual " << selected << ".\n";

	return selected;
}

void cGA::PrintPopulationInfo(const char * pattern)
{
	unsigned int best_idx = m_Minimize ? m_Population.Count() - 1 : 0;
	unsigned int worst_idx = m_Minimize ? 0 : m_Population.Count() - 1;

	double maxFitness = m_Population[best_idx]->Fitness();
	double minFitness = m_Population[worst_idx]->Fitness();
	double avgFitness = 0;

	for (unsigned int i = 0; i < m_Population.Count(); i++)
	{
		cIndividual * mChromosome = m_Population[i];
		avgFitness += mChromosome->Fitness();
	}

	avgFitness = (double) avgFitness / m_Population.Count();

	if (pattern != NULL)
	{
		printf(pattern, maxFitness, minFitness, avgFitness);
	}
	else
	{
		std::cout << maxFitness << "\t" << minFitness << "\t" << avgFitness << "\n";
	}
}

void cGA::ShuffleImpl(void)
{
	unsigned int last = m_Population.Count() - 1;
	if (m_Population[(unsigned int) 0]->Fitness() == m_Population[last]->Fitness())
	{
		//PrintPopulation();
		dbg << "Best and Worst fitness are equal, the population might stagnate. Shuffling.\n";
		for (unsigned int i = 1; i < m_Population.Count(); i++)
		{
			m_Population[i]->Mutate(1, 1);
			m_Population[i]->ComputeFitness();
		}
		SortPopulation();
		//PrintPopulation();
	}
}

cGA::~cGA(void)
{
	for (unsigned int i = 0; i < m_Population.Count(); i++)
	{
		delete m_Population[i];
	}
	m_Population.Clear();
}
