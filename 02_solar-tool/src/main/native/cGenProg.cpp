#include "cGenProg.h"

cGenProg::cGenProg(cForest::t_FitnessType fit_type, const unsigned int pop_size, cData & data, cModel & model, const bool debug) : m_Model(model), m_Data(data)
{
	m_FitnessType = fit_type;
	Debug(debug);

	m_Population.Clear();

	for (unsigned int i = 0; i < pop_size; i++)
	{
		cForest * individual = new cForest(m_Data, m_Model, m_FitnessType);
		individual->Debug(IsDebugging());
		individual->ComputeFitness();
		m_Population.Append(individual);

		if (IsDebugging())
		{
			individual->Print();
		}
	}
	m_SelectionType = 0;
	SortPopulation();
}

void cGenProg::Shuffle(void)
{
	int last = m_Population.Count() - 1;
	//if (BestFitness() == m_Population[last]->Fitness())
	if ((BestFitness() - m_Population[last]->Fitness()) < 1e-3)
	{
		dbg << "Best and Worst fitness are equal, the population might stagnate. Shuffling.\n";
		for (unsigned int i = m_Population.Count() / 2; i < m_Population.Count(); i++)
		{
			delete m_Population[i];
			m_Population[i] = new cForest(m_Data, m_Model, m_FitnessType);
			m_Population[i]->Debug(IsDebugging());
			m_Population[i]->ComputeFitness();
		}
		SortPopulation();
	}
}

cGenProg::~cGenProg()
{
}
