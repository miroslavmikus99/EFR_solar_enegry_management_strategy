#ifndef CRANDOM_H_
#define CRANDOM_H_

#include <memory>

#ifndef _MSC_VER
	#ifdef __MINGW32__
		#define _MSC_VER
	#endif
#endif


#ifndef _MSC_VER
	#ifndef UNREFERENCED_PARAMETER
		#define UNREFERENCED_PARAMETER(x) (void)x
	#endif
	#include <unistd.h>
	#define RAND rand_r(&m_State)
	#define _getpid getpid
#else
	#define NOMINMAX
	#include <windows.h>
	#include <process.h>

	#define RAND rand()
	#define UNREFERENCED_PARAMETER(x) (void)x
#endif

#define THREADSAFE_SEED ((unsigned int) time(NULL)) * ((unsigned int) _getpid())

namespace arg
{
	/**
	 * \class arg::cRandom
	 * \brief A simple interface for adjustable pseudo-random number generator.
	 *
	 * This version is not static and thread safe, iff every thread uses private cRandom
	 * object. In the OpenMP framework, one can use the cRandom like this:
	 * \code
	 * #pragma omp parallel
	 * {
	 *		const int tid = omp_get_thread_num();
	 *		arg::cRandom rnd(tid*time(NULL));
	 *		...
	 * }
	 * \endcode
	 * \author Pavel Krömer (pkromer), (c) 2011 - 2013
	 *
	 * \b History:
	 *		- 2011-09,	pkromer,	Created as extension of mersene twister.
	 *		- 2012-11,	pkromer,	Transformed to interface
	 */
	class cRandom
	{
		public:
			typedef enum { RNG_MERSENNE_TWISTER, RNG_RANLUXD1, RNG_RANLUXD2, RNG_STANDARD, RNG_STATIC_ADAPTOR} t_RngType;

			cRandom() {};

			virtual void Seed(const unsigned int* seed, const unsigned int seed_len = 1) = 0; 			///< Seed with some value.
			virtual int Next(void) = 0; 								///< Next integer.
			virtual double Next(const double) = 0; 					///< Next double lower equal to argument
			virtual double Next(const double, const double) = 0; 	///< Next double from a range
			virtual int NextInt(const int) = 0; 						///< Next integer lower equal to argument

			static cRandom* GetInstance(const t_RngType type = cRandom::RNG_MERSENNE_TWISTER);		///< Get instance of selected rng.

			virtual ~cRandom() {};
	};


	/**
	 * \class arg::cStaticRandom
	 * \brief An implementation of simple adjustable pseudo-random number generator.
	 *
	 * Provides static utility methods for pseudo rng. Uses a \b static cMerseneTwister
	 * in background. I.e. every component that uses this class gets pseudo random
	 * numbers from the same source.
	 *
	 * \author Pavel Krömer (pkromer), (c) 2005 - 2013
	 *
	 * \b History:
	 *		- 2005,		pkromer,	initial version
	 *		- 2006,		pkromer, 	more methods
	 *		- 2011-02,	pkromer,	changed to a facade to a static cMersenneTwister
	 *		- 2011-11,	pkromer,	Support for mersenne twister and ranlux
	 *
	 */
	class cStaticRandom
	{
			static std::unique_ptr<cRandom> m_Generator;

		public:
			inline static void Seed(const unsigned int); 			///< Seed with some value.
			inline static void Seed(const unsigned int *,const unsigned int seed_len);
			inline static int Next(void); 							///< Next integer.
			inline static double Next(const double); 				///< Next double lower equal to argument
			inline static double Next(const double, const double);	///< Next double from a range
			inline static int NextInt(const int); 					///< Next integer lower equal to argument

			/**
			 * \brief A method to select PRNG implemented within AmphorA by type (\ref t_RngType).
			 *
			 * \param[in] type	- type of PRNG.
			 */
			static void SetStaticGenerator(const cRandom::t_RngType type);

			/**
			 * \brief A method to select PRNG implemented within AmphorA by name.
			 *
			 * \param[in] type	- name of the prng. Currently supports 'mersenne', 'ranluxd1', 'ranluxd2', 'std'
			 * \return success	- true if corresponding PRNG was found.
			 */
			static bool SetStaticGenerator(const char* type);

			/**
			 * \brief A method to plug in own implementation of RNG that extends \ref arg::cRandom.
			 *
			 * \param[in] rng	- pointer to PRNG instance. Instance will be freed by cStaticRandom.
			 */
			static void SetStaticGenerator(cRandom* rng);
	};

	inline void cStaticRandom::Seed(const unsigned int seed)
	{
		cStaticRandom::m_Generator->Seed(&seed, 1);
	}

	inline void cStaticRandom::Seed(const unsigned int *seed, const unsigned int seed_len)
	{
		cStaticRandom::m_Generator->Seed(seed, seed_len);
	}

	inline int cStaticRandom::Next(void)
	{
		return cStaticRandom::m_Generator->Next();
	}

	inline double cStaticRandom::Next(const double up_to)
	{
		return cStaticRandom::m_Generator->Next(up_to);
	}

	inline double cStaticRandom::Next(const double from, const double to)
	{
		return cStaticRandom::m_Generator->Next(from, to);
	}

	inline int cStaticRandom::NextInt(const int up_to)
	{
		return cStaticRandom::m_Generator->NextInt(up_to);
	}
}

#endif /* CRANDOM_H_ */
