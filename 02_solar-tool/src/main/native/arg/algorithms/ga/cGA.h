/**
 * \class arg::cGA
 * \brief An abstract Genetic Algorithm class.
 *
 * To be subclassed by concrete customized implementations of genetic algorithms.
 * Defines basic implementation of some operations and the interface that might
 * be used in ga projects
 *
 * \author Pavel Kromer, (c) 2007 - 2010
 *
 * \b History:
 *	- initial version, 2007, pkromer
 *	- abstractized for AmphorA core library, 25-7-2007, pkromer
 * 	- doxy comments, 26-07-2007, pkromer (non-functional change)
 *  - removed some problems, 12-2009, pkromer
 *
 */
#ifndef __CGA__
#define __CGA__

#include <iostream>
#include <cstdio>

#include "cIndividual.h"

#include <arg/core/cArray.h>
#include <arg/core/cDebuggable.h>

#include <arg/utils/cTimer.h>
#include <arg/utils/cRandom.h>

namespace arg
{
	class cGA : public cDebuggable
	{
		public:
			const static unsigned int SELECT_ROULETTE = 0;
			const static unsigned int SELECT_ELITARY = 1;
			const static unsigned int SELECT_SEMIELITARY = 2;

			const static unsigned int MUT_CLASSIC = 101;
			const static unsigned int MUT_ARITHMETIC = 102;
			const static unsigned int MUT_MATRIX = 103;

			const static unsigned int CROSS_CLASSIC = 201;
			const static unsigned int CROSS_ARITHMETIC = 202;
			const static unsigned int CROSS_MATRIX = 203;
			const static unsigned int CROSS_ONE_POINT = 204;
			const static unsigned int CROSS_TWO_POINT = 205;

			const static unsigned int STEADY_STATE = 301;
			const static unsigned int STEADY_STATE_REVERSE_FITNESS = 302;

		protected:

			cArrayConst<cIndividual*> m_Population; ///< A population of individuals (\ref arg::cIndividual)

			unsigned int m_FirstParent, m_SecondParent; ///< Indexes of selected parents
			cIndividual * m_Son, *m_Daughter; ///< Offspring chromosomes

			/** Types of selection, mutation, crossover and migration. */
			unsigned int m_SelectionType, m_MutationType, m_CrossoverType, m_MigrationType;

			unsigned int m_Generations; ///< Max number of generations to process
			unsigned int m_PopulationSize; ///< The size of the population

			/** Flags and settings. */
			bool m_Minimize; ///< Whether minimize or maximize fitness (by default false)
			bool m_Shuffle; ///< Whether or not to shuffle the population
			bool m_Prevent; ///< Whether or not to prevent insertion of "similar" new individuals

			/** Crossover and mutation probability. */
			double m_CrossoverProbability, m_MutationProbability;

		public:
			cGA(void);

			/** Utility functions. */
			void SortPopulation(void);
			unsigned int SelectIndividual();
			void SortInOne(const unsigned int from, const bool reverse);
			void SortInOne(const unsigned int);
			void MigrateImpl(unsigned int, unsigned int, const bool, cIndividual*);

			/** Particular GA steps. */
			void Select(void);
			void Mutate(const double pM);
			void Recombine(const double pC);
			void Migrate(const bool prevent_stagnation = false);
			void ComputeChildFitness(void);

			/** Our own GA steps. */
			void ShuffleImpl(void);

			/** Getters and setters*/
			inline void PopulationSize(const unsigned int val);
			inline unsigned int PopulationSize(void) const;

			inline void Generations(const unsigned int val);
			inline unsigned int Generations(void) const;

			inline void SelectionType(const unsigned int val);
			inline unsigned int SelectionType(void) const;

			inline void MutationType(const unsigned int val);
			inline unsigned int MutationType(void) const;

			inline void MigrationType(const unsigned int val);
			inline unsigned int MigrationType(void) const;

			inline void CrossoverType(const unsigned int val);
			inline unsigned int CrossoverType(void) const;

			inline void MutationProbability(const double val);
			inline void CrossoverProbability(const double val);

			/** Various flag setters. */
			inline void Minimize(const bool val = true);
			inline void Prevent(const bool val = true);
			inline void Shuffle(const bool val = true);

			/** \returns Best (maximum or minimum) fitness in the population. */
			inline double BestFitness(void) const;
			/** \returns Pointer to the best individual in the current population. */
			inline cIndividual * WinnerPtr(void) const;
			/** Prints population statistics. */
			void PrintPopulationInfo(const char * pattern = NULL);

			virtual ~cGA(void);
	};

	inline void cGA::MutationProbability(const double val)
	{
		m_MutationProbability = val;
	}

	inline void cGA::CrossoverProbability(const double val)
	{
		m_CrossoverProbability = val;
	}

	inline void cGA::PopulationSize(const unsigned int val)
	{
		m_PopulationSize = val;
	}

	inline unsigned int cGA::PopulationSize(void) const
	{
		return m_PopulationSize;
	}

	inline void cGA::Generations(const unsigned int val)
	{
		m_Generations = val;
	}

	inline unsigned int cGA::Generations(void) const
	{
		return m_Generations;
	}

	inline void cGA::SelectionType(const unsigned int val)
	{
		m_SelectionType = val;
	}

	inline unsigned int cGA::SelectionType(void) const
	{
		return m_SelectionType;
	}

	inline void cGA::MutationType(const unsigned int val)
	{
		m_MutationType = val;
	}

	inline unsigned int cGA::MutationType(void) const
	{
		return m_MutationType;
	}

	inline void cGA::MigrationType(const unsigned int val)
	{
		m_MigrationType = val;
	}

	inline unsigned int cGA::MigrationType(void) const
	{
		return m_MigrationType;
	}

	inline void cGA::CrossoverType(const unsigned int val)
	{
		m_CrossoverType = val;
	}

	inline unsigned int cGA::CrossoverType(void) const
	{
		return m_CrossoverType;
	}

	inline cIndividual * cGA::WinnerPtr(void) const
	{
		const unsigned int best_idx = m_Minimize ? m_Population.Count() - 1 : 0;
		return m_Population[best_idx];
	}

	inline void cGA::Minimize(const bool val)
	{
		m_Minimize = val;
	}

	inline void cGA::Shuffle(const bool val)
	{
		m_Shuffle = val;
	}

	inline void cGA::Prevent(const bool val)
	{
		m_Prevent = val;
	}

	inline double cGA::BestFitness(void) const
	{
		return WinnerPtr()->Fitness();
	}
}
#endif
