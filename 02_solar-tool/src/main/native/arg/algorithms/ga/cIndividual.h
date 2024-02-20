/**
 * \class arg::cIndividual
 * \brief An abstract individual for genetic algorithms.
 *
 * To be subclassed by concrete customized implementations of GA individuals.
 * Used by abstract genetic algorithm implementation cGA.
 *
 * \author Pavel Kromer, (c) 2007 - 2010
 *
 * \b History:
 *	- initial version, 2007, pkromer
 *	- abstractized for AmphorA core library, 25-7-2007, pkromer
 * 	- doxygen comments, 26-07-2007, pkromer (non-functional change)
 *
 */
#ifndef __cINDIVIDUAL__
#define __cINDIVIDUAL__

#include <iostream>
#include <arg/core/cDebuggable.h>

namespace arg
{
	class cIndividual: public cDebuggable
	{
		protected:
			double m_Fitness; ///< The fitness of an individual

		public:
			virtual double ComputeFitness(void) = 0;
			virtual void Print(void) const = 0;
			virtual void Init(){};
			virtual double Fitness() const {return m_Fitness;};
			virtual void Mutate(const unsigned int, const double) = 0;
			virtual void Crossover(const unsigned int, cIndividual&, const double) = 0;
			virtual cIndividual * Clone(void) = 0;
			virtual int Length(void){return -1;};
			virtual bool Save(const char*){return false;};
			virtual bool Equals(cIndividual & other) {return m_Fitness == other.Fitness();};
			virtual ~cIndividual(void){};
	};
}
#endif
