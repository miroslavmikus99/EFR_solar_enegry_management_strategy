/**
 * \class arg::cRanlux
 * \brief Implementation of pseudo random number generator ranluxd2.
 *
 * Taken from GNU Scientific library 1.9, ranlxd.c. Description available at
 * http://www.gnu.org/software/gsl/manual/html_node/Random-number-generator-algorithms.html
 *
 *
 *	Properties according to randlxd.c:
	\code
	 static const gsl_rng_type ranlxd1_type =
	 {"ranlxd1",
	 0xffffffffUL,                  // RAND_MAX
	 0,                             // RAND_MIN
	 sizeof (ranlxd_state_t),
	 &ranlxd1_set,
	 &ranlxd_get,
	 &ranlxd_get_double};

	 static const gsl_rng_type ranlxd2_type =
	 {"ranlxd2",
	 0xffffffffUL,                  // RAND_MAX
	 0,                             // RAND_MIN
	 sizeof (ranlxd_state_t),
	 &ranlxd2_set,
	 &ranlxd_get,
	 &ranlxd_get_double};
	\endcode
 *
 *
 * \author Pavel Krömer (c) 2012
 *
 */

#ifndef CRANLUX_H_
#define CRANLUX_H_

#include <arg/utils/cRandom.h>

#include <ctime>

namespace arg {

	typedef struct
	{
			double xdbl[12];
			double carry;
			unsigned int ir;
			unsigned int jr;
			unsigned int ir_old;
			unsigned int pr;
	} ranlxd_state_t;

	class cRanlux: public cRandom
	{
		public:
			typedef enum {RANNLUX_D1, RANNLUX_D2} t_RanluxType;

		private:
			t_RanluxType m_Type;
			ranlxd_state_t m_State;

			void increment_state(ranlxd_state_t * state);
			unsigned long int ranlxd_get(void *vstate);
			double ranlxd_get_double(void *vstate);
			void ranlxd_set_lux(void *vstate, unsigned long int s, unsigned int luxury);

			void ranlxd1_set(void *vstate, unsigned long int s);
			void ranlxd2_set(void *vstate, unsigned long int s);

		public:
			cRanlux(const t_RanluxType = RANNLUX_D2);

			virtual void Seed(const unsigned int* seed, const unsigned int seed_len = 1); ///< Seed with some value.
			virtual int Next(void); 															///< Next integer.
			virtual double Next(const double); 												///< Next double lower equal to argument
			virtual double Next(const double, const double); 								///< Next double from a range
			virtual int NextInt(const int); 													///< Next integer lower equal to argument

			virtual ~cRanlux(void);
	};
}

#endif /* CRANLUX_H_ */
