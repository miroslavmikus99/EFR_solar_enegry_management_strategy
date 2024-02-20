/**************************************************************************}
{                                                                          }
{    ccMersenneTwister.h                                    			   }
{                                                                          }
{                                                                          }
{                 Copyright (c) 2011				   Vaclav Snasel	   }
{                                                                          }
{    VERSION: 2.0									   DATE 5/12/2010      }
{																		   }
{             following functionality:                                     }
{    source M. Matsumoto and T. Nishimura, "Mersenne Twister:			   } 
{           A 623-Dimensionally Equidistributed Uniform Pseudo-Random	   }
{			Number Generator", ACM Transactions on Modeling and Computer   }
{			Simulation, Vol. 8, No. 1, January 1998, pp 3-30.			   }
{																		   }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{                                                                          }
{                                                                          }
{**************************************************************************/



#ifndef __CMERSENNETWISTER_H__
#define __CMERSENNETWISTER_H__

#include <ctime>
#include <cmath>
#include <climits>

#include "../cRandom.h"

namespace arg
{

	class cMersenneTwister : public cRandom
	{
	// Data
	protected:

		static const unsigned int N = 624;       // length of state vector
		static const unsigned int SAVE = N + 1;  // length of array for save()
		static const unsigned int M = 397;		 // period parameter

		unsigned int state[N];   // internal state
		unsigned int *pNext;     // next value to get from state
		unsigned int left;       // number of values left before reload needed
	
	public:
	// cRandom methods
		virtual void Seed(const unsigned int* seed, const unsigned int seed_len = 1);
		virtual int Next(void);
		virtual double Next(const double);
		virtual double Next(const double, const double);
		virtual int NextInt(const int);
	
	// Methods
	public:
		cMersenneTwister(const unsigned int oneSeed);  // initialize with a simple unsigned int
		cMersenneTwister(unsigned int *const bigSeed, unsigned int const seedLength = N);  // or array
		cMersenneTwister();  // auto-initialize with /dev/urandom or time() and clock()


		// Do NOT use for CRYPTOGRAPHY without securely hashing several returned
		// values together, otherwise the generator state can be learned after
		// reading 624 consecutive values.

		// Access to 32-bit random numbers
		unsigned int randInt();							// integer in [0,2^32-1]
		unsigned int randInt(const unsigned int n);     // integer in [0,n] for n < 2^32
		double rand();									// real number in [0,1]
		double rand(const double n);					// real number in [0,n]
		double randExc();								// real number in [0,1)
		double randExc(const double n);					// real number in [0,n)
		double randDblExc();							// real number in (0,1)
		double randDblExc(const double n);				// real number in (0,n)

		// Access to 53-bit random numbers (capacity of IEEE double precision)
		double rand53();  // real number in [0,1)

		// Access to nonuniform random number distributions
		double randNorm(const double mean = 0.0, const double stddev = 1.0);

		// Re-seeding functions with same behavior as initializers
		void seed(const unsigned int oneSeed);
		void seed(unsigned int *const bigSeed, const unsigned int seedLength = N);


	protected:
		void initialize(const unsigned int oneSeed);
		void reload();
		
		inline unsigned int hiBit(const unsigned int u) const { return u & 0x80000000UL; }
		inline unsigned int loBit(const unsigned int u) const { return u & 0x00000001UL; }
		inline unsigned int loBits(const unsigned int u) const { return u & 0x7fffffffUL; }
	
		unsigned int mixBits(const unsigned int u, const unsigned int v) const
			{ return hiBit(u) | loBits(v); }
		unsigned int magic(const unsigned int u) const
			{ return loBit(u) ? 0x9908b0dfUL : 0x0UL; }
		unsigned int twist(const unsigned int m, const unsigned int s0, const unsigned int s1) const
			{ return m ^ (mixBits(s0,s1)>>1) ^ magic(s1); }
		static unsigned int hash(time_t t, clock_t c);
	};
	
	inline cMersenneTwister::cMersenneTwister()
	{
		seed(THREADSAFE_SEED);
	}
	
	inline cMersenneTwister::cMersenneTwister(const unsigned int seed_val)
	{
		seed(seed_val);
	}
	
	inline void cMersenneTwister::Seed(const unsigned int * seed_val, const unsigned int seed_len)
	{
		seed((unsigned int * const )seed_val, seed_len);
	}
	
	inline int cMersenneTwister::Next(void)
	{
		return randInt();
	}
	
	inline double cMersenneTwister::Next(const double up_to)
	{
		return rand(up_to);
	}
	
	inline double cMersenneTwister::Next(const double from, const double to)
	{
		return from + rand() * (to - from);
	}
	
	inline int cMersenneTwister::NextInt(const int up_to)
	{
		return randInt(up_to);
	}

	// Functions are defined in order of usage to assist inlining
	inline unsigned int cMersenneTwister::hash(time_t t, clock_t c)
	{
		// Get a unsigned int from t and c
		// Better than unsigned int(x) in case x is floating point in [0,1]
		// Based on code by Lawrence Kirby (fred@genesis.demon.co.uk)

		static unsigned int differ = 0;  // guarantee time-based seeds will change

		unsigned int h1 = 0;
		unsigned char *p = (unsigned char *) &t;
		for(unsigned int i = 0; i < sizeof(t); ++i)
		{
			h1 *= UCHAR_MAX + 2U;
			h1 += p[i];
		}
		unsigned int h2 = 0;
		p = (unsigned char *) &c;
		for(unsigned int j = 0; j < sizeof(c); ++j)
		{
			h2 *= UCHAR_MAX + 2U;
			h2 += p[j];
		}
		return (h1 + differ++) ^ h2;
	}

	inline void cMersenneTwister::initialize(const unsigned int seed)
	{
		// Initialize generator state with seed
		// See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
		// In previous versions, most significant bits (MSBs) of the seed affect
		// only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto.
		unsigned int *s = state;
		unsigned int *r = state;
		*s++ = seed & 0xffffffffUL;
		for(unsigned int i = 1; i < N; ++i)
		{
			*s++ = (1812433253UL * (*r ^ (*r >> 30)) + i) & 0xffffffffUL;
			r++;
		}
	}

	inline void cMersenneTwister::reload()
	{
		// Generate N new values in state
		// Made clearer and faster by Matthew Bellew (matthew.bellew@home.com)
		int MmN = int(M) - int(N);  // in case enums are unsigned
		unsigned int *p = state;
		for(int i = N - M; i--; ++p)
		{
			*p = twist(p[M], p[0], p[1]);
		}
		for(int i = M; --i; ++p)
		{
			*p = twist(p[MmN], p[0], p[1]);
		}

		*p = twist(p[MmN], p[0], state[0]);

		left = N, pNext = state;
	}

	inline void cMersenneTwister::seed(const unsigned int oneSeed)
	{
		// Seed the generator with a simple unsigned int
		initialize(oneSeed);
		reload();
	}
	
	inline void cMersenneTwister::seed(unsigned int *const bigSeed, const unsigned int seedLength)
	{
		// Seed the generator with an array of unsigned int's
		// There are 2^19937-1 possible initial states.  This function allows
		// all of those to be accessed by providing at least 19937 bits (with a
		// default seed length of N = 624 unsigned int's).  Any bits above the lower 32
		// in each element are discarded.
		// Just call seed() if you want to get array from /dev/urandom
		initialize(19650218UL);
		unsigned int i = 1;
		unsigned int j = 0;
		for(int k = (N > seedLength ? N : seedLength); k; --k)
		{
			state[i] = state[i] ^ ((state[i-1] ^ (state[i-1] >> 30)) * 1664525UL);
			state[i] += (bigSeed[j] & 0xffffffffUL) + j;
			state[i] &= 0xffffffffUL;
			++i;  ++j;
			if(i >= N)
			{
				state[0] = state[N-1];
				i = 1;
			}
			if(j >= seedLength)
			{
				j = 0;
			}
		}
		for(int k = N - 1; k; --k)
		{
			state[i] = state[i] ^ ((state[i-1] ^ (state[i-1] >> 30)) * 1566083941UL);
			state[i] -= i;
			state[i] &= 0xffffffffUL;
			++i;
			if(i >= N)
			{
				state[0] = state[N-1];
				i = 1;
			}
		}
		state[0] = 0x80000000UL;  // MSB is 1, assuring non-zero initial array
		reload();
	}

	inline cMersenneTwister::cMersenneTwister(unsigned int *const bigSeed, const unsigned int seedLength)
	{
		seed(bigSeed,seedLength);
	}

	inline unsigned int cMersenneTwister::randInt()
	{
		// Pull a 32-bit integer from the generator state
		// Every other access function simply transforms the numbers extracted here

		if (left == 0)
		{
			reload();
		}
		--left;

		unsigned int s1;
		s1 = *pNext++;
		s1 ^= (s1 >> 11);
		s1 ^= (s1 <<  7) & 0x9d2c5680UL;
		s1 ^= (s1 << 15) & 0xefc60000UL;
		return (s1 ^ (s1 >> 18));
	}
	
	inline unsigned int cMersenneTwister::randInt(const unsigned int n)
	{
		// Find which bits are used in n
		// Optimized by Magnus Jonsson (magnus@smartelectronix.com)
		unsigned int used = n;
		used |= used >> 1;
		used |= used >> 2;
		used |= used >> 4;
		used |= used >> 8;
		used |= used >> 16;

		// Draw numbers until one is found in [0,n]
		unsigned int i;
		do
			i = randInt() & used;  // toss unused bits to shorten search
		while(i > n);
		return i;
	}

	inline double cMersenneTwister::rand()
	{
		return double(randInt()) * (1.0 / 4294967295.0);
	}

	inline double cMersenneTwister::rand(const double n)
	{
		return rand() * n;
	}

	inline double cMersenneTwister::randExc()
	{
		return double(randInt()) * (1.0 / 4294967296.0);
	}

	inline double cMersenneTwister::randExc(const double n)
	{
		return randExc() * n;
	}

	inline double cMersenneTwister::randDblExc()
	{
		return (double(randInt()) + 0.5) * (1.0 / 4294967296.0);
	}

	inline double cMersenneTwister::randDblExc(const double n)
	{
		return randDblExc() * n;
	}
	
	inline double cMersenneTwister::rand53()
	{
		unsigned int a = randInt() >> 5, b = randInt() >> 6;
		return (a * 67108864.0 + b) * (1.0 / 9007199254740992.0);  // by Isaku Wada
	}
	
	inline double cMersenneTwister::randNorm(const double mean, const double stddev)
	{
		// Return a real number from a normal (Gaussian) distribution with given
		// mean and standard deviation by polar form of Box-Muller transformation
		double x, y, r;
		do
		{
			x = 2.0 * rand() - 1.0;
			y = 2.0 * rand() - 1.0;
			r = x * x + y * y;
		}
		while (r >= 1.0 || r == 0.0);
		double s = sqrt(-2.0 * log(r) / r);
		return mean + x * s * stddev;
	}
}

#endif  // __CMERSENNETWISTER_H__
