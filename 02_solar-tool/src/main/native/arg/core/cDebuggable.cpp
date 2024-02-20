#include "cDebuggable.h"

namespace arg
{
	cDebuggable::cDebuggable(void) :
		m_Debug(false), m_StructLog(false), m_DebugStream(&std::cout), m_ErrorStream(&std::cerr)
	{
	}

	std::ostream & cDebuggable::stamp(std::ostream & stream) const
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time(&rawtime);

		timeinfo = localtime(&rawtime);
		
		strftime(buffer, 80, "[%Y/%m/%d, %H:%M:%S] ", timeinfo);
		return stream;
	}
}
