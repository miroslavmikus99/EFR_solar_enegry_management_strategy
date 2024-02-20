#include <cstring>

#include <arg/utils/cRandom.h>

#include <arg/utils/rng/cMersenneTwister.h>
#include <arg/utils/rng/cStandardRng.h>
#include <arg/utils/rng/cRanlux.h>
#include <arg/utils/rng/cStaticRngAdaptor.h>

namespace arg
{
	std::unique_ptr<cRandom> cStaticRandom::m_Generator(std::unique_ptr<cRandom>(new cMersenneTwister(THREADSAFE_SEED)));

	cRandom* cRandom::GetInstance(const t_RngType rng_type)
	{
		cRandom * instance = NULL;
		if (rng_type == cRandom::RNG_RANLUXD1)
		{
			instance = new cRanlux(cRanlux::RANNLUX_D1);
		}
		else if (rng_type == cRandom::RNG_RANLUXD2)
		{
			instance = new cRanlux();
		}
		else if (rng_type == cRandom::RNG_STANDARD)
		{
			instance = new cStandardRng();
		}
		else if (rng_type == cRandom::RNG_STATIC_ADAPTOR)
		{
			instance = new cStaticRngAdaptor();
		}
		else
		{
			instance = new cMersenneTwister();
		}
		return instance;
	}

	void cStaticRandom::SetStaticGenerator(cRandom::t_RngType rng_type)
	{
		if (rng_type == cRandom::RNG_RANLUXD1)
		{
			SetStaticGenerator(new cRanlux(cRanlux::RANNLUX_D1));
		}
		else if (rng_type == cRandom::RNG_RANLUXD2)
		{
			SetStaticGenerator(new cRanlux());
		}
		else if (rng_type == cRandom::RNG_STANDARD)
		{
			SetStaticGenerator(new cStandardRng());
		}
		else
		{
			SetStaticGenerator(new cMersenneTwister());
		}
	}

	bool cStaticRandom::SetStaticGenerator(const char* rng_type)
	{
		bool success = false;

		if (strcmp(rng_type, "mersenne") == 0)
		{
			SetStaticGenerator(cRandom::RNG_MERSENNE_TWISTER);
			success = true;
		}
		else if (strcmp(rng_type, "ranluxd1") == 0)
		{
			SetStaticGenerator(cRandom::RNG_RANLUXD1);
			success = true;
		}
		else if (strcmp(rng_type, "ranluxd2") == 0)
		{
			SetStaticGenerator(cRandom::RNG_RANLUXD2);
			success = true;
		}
		else if (strcmp(rng_type, "std") == 0)
		{
			SetStaticGenerator(cRandom::RNG_STANDARD);
			success = true;
		}

		return success;
	}

	void cStaticRandom::SetStaticGenerator(cRandom* rng)
	{
		m_Generator.reset(rng);
	}
}
