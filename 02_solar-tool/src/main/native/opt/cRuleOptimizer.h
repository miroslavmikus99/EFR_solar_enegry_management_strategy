#ifndef CRULEOPTIMIZER_H_
#define CRULEOPTIMIZER_H_

#include <algorithm>

template<class Base>
class cRuleOptimizer: public Base
{

	private:
		cForest & m_Forest;
		cData & m_Data;

		double m_BestUnusedBW;
		double m_BestLostBytes;
		double m_BestFitness;

	public:
		cRuleOptimizer(cForest & forest, cData & data, const unsigned int population_size) :
				Base(population_size, forest.ParamCount()), m_Forest(forest), m_Data(data)
		{
			m_BestFitness = 0;
			m_BestUnusedBW = m_BestLostBytes = 2;
			Base::SetRange(0, 1);
		}

		virtual double ComputeFitness(const double * individual, const unsigned int size)
		{
			// here we need to minimize
			m_Forest.SetParams(individual, size);
			double fit = 1.0 / m_Forest.ComputeFitness();

			if (fit < Base::BestFitness())
			{
				m_BestFitness = m_Forest.Fitness();
				m_BestUnusedBW = m_Forest.UnusedBW();
				m_BestLostBytes = m_Forest.LostBytes();
			}

			return fit;
		}

		virtual void PrintPopInfo(int cnt)
		{
			std::cout << std::fixed << "[" << (this) << "]\t" << cnt << "\t" << Base::m_CurrentEvaluation << "\t"
					<< Base::m_Timer.CpuStop().CpuMillis() << "\t" << Base::BestFitness()
					<< "\t" << 1.0 / Base::BestFitness() // << "(" << m_BestFitness << ")"
					<< "\t" << m_BestLostBytes
					<< "\t" << m_BestUnusedBW
					<< std::endl;
		}

		virtual ~cRuleOptimizer()
		{
		}
};

#endif
