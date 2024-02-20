/**
 * \class arg::cStaticRngAdaptor
 * \brief A simple adaptor to the static random number generator.
 *
 * For the use with thread-safe classes such as \ref arg::cDE, when used in single-threaded environment with the need to use the static RNG
 * (e.g. when plugging in custom RNG via the function \ref arg::cStaticRandom::SetStaticGenerator) .
 *
 *
 *
 * \author Pavel Krömer (c) 2014
 *
 */
#ifndef CSTATICRNGADAPTOR_H_
#define CSTATICRNGADAPTOR_H_

#include <arg/utils/cRandom.h>

class cStaticRngAdaptor: public arg::cRandom
{
	public:
		cStaticRngAdaptor();

		virtual void Seed(const unsigned int* seed, const unsigned int seed_len); 		///< Seed with some value.
		virtual int Next(void); 														///< Next integer.
		virtual double Next(const double); 												///< Next double lower equal to argument
		virtual double Next(const double, const double); 								///< Next double from a range
		virtual int NextInt(const int); 												///< Next integer lower equal to argument

		virtual ~cStaticRngAdaptor();

};

#endif /* CSTATICRNGADAPTOR_H_ */
