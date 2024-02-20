/**
 * \class arg::cStandardRng
 * \brief A thread-safe wrapper around std random number generator.
 *
 * It should be a rng with period of 2^32
 *
 * \author Pavel Krömer (c) 2012
 *
 */

#ifndef CSTANDARDRNG_H_
#define CSTANDARDRNG_H_

#include <ctime>
#include <cstdlib>

// #include <random>

#include <arg/utils/cRandom.h>

namespace arg
{
	class cStandardRng : public cRandom
	{
		private:
			unsigned int m_State;

		public:
			cStandardRng();

			virtual void Seed(const unsigned int* seed, const unsigned int seed_len = 1); ///< Seed with some value.
			virtual int Next(void); 															///< Next integer.
			virtual double Next(const double); 												///< Next double lower equal to argument
			virtual double Next(const double, const double); 								///< Next double from a range
			virtual int NextInt(const int); 													///< Next integer lower equal to argument

			virtual ~cStandardRng();
	};

/*
	// C++11

	class cRanlux48 : public cRandom
	{
		private:
			std::ranlux48 m_Generator;

		public:
			cRanlux48();

			virtual void Seed(const unsigned int* seed, const unsigned int seed_len = 1); ///< Seed with some value.
			virtual int Next(void); 															///< Next integer.
			virtual double Next(const double); 												///< Next double lower equal to argument
			virtual double Next(const double, const double); 								///< Next double from a range
			virtual int NextInt(const int); 													///< Next integer lower equal to argument

			virtual ~cRanlux48();
	};
*/
}

#endif /* CSTANDARDRNG_H_ */
