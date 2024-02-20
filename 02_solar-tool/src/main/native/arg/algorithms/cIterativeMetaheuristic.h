#ifndef CITERATIVEMETAHEURISTIC_H_
#define CITERATIVEMETAHEURISTIC_H_

#include <arg/core/cDebuggable.h>
#include <arg/core/cArray.h>

namespace arg
{
	typedef struct {double fit; unsigned int idx;} t_FitnessIndexPair;

	class cIterativeMetaheuristic : public cDebuggable
	{
		protected:
			arg::cArrayConst<unsigned int> m_Filter;

		public:
			virtual void PopulationInfo(cArrayConst<t_FitnessIndexPair> &indexes) = 0;

			virtual void Init(void) = 0;
			virtual bool Iterate(const unsigned int iterations) = 0;
			virtual void Finalize(void) = 0;
			virtual double BestFitness(void) const = 0;
			virtual void ResetPopulationFitness(void) = 0;

			virtual cArrayConst<double> Winner(void) = 0;
			virtual double * WinnerPtr(void) = 0;

			virtual unsigned int Dimension() const = 0;
			virtual unsigned int PopulationSize() const = 0;

			virtual void SetInitialPopulation(const double * initpop) = 0;
			virtual double RangeMin(void) = 0;
			virtual double RangeMax(void) = 0;

			virtual void Replace(const unsigned int idx, const double * vector, const double fitness) = 0;
			virtual double * VectorPtr(const unsigned int idx) = 0;

			void Filter(arg::cArrayConst<unsigned int> & filter) { m_Filter = filter; };
			cArrayConst<unsigned int> & Filter(void) { return m_Filter; };

			virtual ~cIterativeMetaheuristic(){};
	};

}
#endif /* CITERATIVEMETAHEURISTIC_H_ */
