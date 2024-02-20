#include "cStaticRngAdaptor.h"

cStaticRngAdaptor::cStaticRngAdaptor()
{
}

void cStaticRngAdaptor::Seed(const unsigned int* seed, const unsigned int seed_len = 1)
{
	arg::cStaticRandom::Seed(seed, seed_len);
}

int cStaticRngAdaptor::Next(void)
{
	return arg::cStaticRandom::Next();
}

double cStaticRngAdaptor::Next(const double v)
{
	return arg::cStaticRandom::Next(v);
}

double cStaticRngAdaptor::Next(const double a, const double b)
{
	return arg::cStaticRandom::Next(a,b);
}

int cStaticRngAdaptor::NextInt(const int v)
{
	return arg::cStaticRandom::NextInt(v);
}

cStaticRngAdaptor::~cStaticRngAdaptor()
{
}
