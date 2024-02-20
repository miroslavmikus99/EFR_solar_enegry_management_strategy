#include "cAmphorA.h"

using namespace arg;

cAmphorA::cAmphorA()
{
}

std::string cAmphorA::Identify(void)
	{
		std::stringstream ss;

		ss << m_Major << "." << m_Minor << "." << m_Fix << ", built on " << __DATE__ << " " << __TIME__;
		#ifdef SCM_VERSION
			ss << ", SCM version [" << QUOTE(SCM_VERSION) << "]";
		#endif
		return ss.str();
	}


bool cAmphorA::Require(const unsigned int major, const unsigned int minor, const unsigned int fix = 0) const
{
	if (m_Major > major)
	{
		return true;
	}
	else if (m_Major < major)
	{
		return false;
	}

	//m_Major == major
	if (m_Minor > minor)
	{
		return true;
	}
	else if (m_Minor < minor)
	{
		return false;
	}

	//m_Major == major && m_Minor == minor
	if (m_Fix >= fix)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool cAmphorA::Require(const unsigned int version_hash) const
{
	return version_hash <= m_Major * 1e4 + m_Minor * 1e2 + m_Fix;
}
