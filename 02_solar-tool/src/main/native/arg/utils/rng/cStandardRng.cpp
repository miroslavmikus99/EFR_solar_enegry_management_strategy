#include "cStandardRng.h"

using namespace arg;

cStandardRng::cStandardRng()
{
	m_State = THREADSAFE_SEED;
	srand((unsigned int) m_State);
}

void cStandardRng::Seed(const unsigned int* seed, const unsigned int seed_len)
{
	UNREFERENCED_PARAMETER(seed_len);
	m_State = seed[0];
	srand((unsigned int) m_State);
}

int cStandardRng::Next(void)
{
	return RAND;
}

double cStandardRng::Next(const double up_to)
{
	return ((double) RAND / RAND_MAX) * up_to;
}

double cStandardRng::Next(const double from, const double to)
{
	return from + ((double) RAND / RAND_MAX) * (to - from);
}

int cStandardRng::NextInt(const int up_to)
{
	return (int) Next((double) up_to);
}

cStandardRng::~cStandardRng()
{
}

/*
cRanlux48::cRanlux48()
{
	m_Generator.seed(time(NULL));
}

void cRanlux48::Seed(const unsigned int* seed, const unsigned int seed_len)
{
	m_Generator.seed(seed[0]);
}

int cRanlux48::Next(void)
{
	return m_Generator();
}

double cRanlux48::Next(const double up_to)
{
	return ((double) m_Generator() / m_Generator.max()) * up_to;
}

double cRanlux48::Next(const double from, const double to)
{
	return from + ((double) m_Generator() / m_Generator.max()) * (to - from);
}

int cRanlux48::NextInt(const int up_to)
{
	return (int) Next((double) up_to);
}

cRanlux48::~cRanlux48()
{
}
*/
