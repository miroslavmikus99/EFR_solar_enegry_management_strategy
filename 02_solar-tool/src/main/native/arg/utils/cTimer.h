/**
 * \class arg::cTimer
 * \brief An utility timer.
 *
 * Can measure time range in seconds and milliseconds. Works for both GCC (Linux) and MS Visual Studio.
 * Typical use:
 * \code
 * 		arg::cTimer timer;
 * 		timer.CpuStart();
 *		...
 *		cout << timer.CpuStop().CpuMillis();
 * \endcode
 *
 * \author Pavel Kromer, (c) 2007 - 2010
 *
 * \version 3.0
 * 
 * \b History:
 *		- initial version, 2007, pkromer
 * 		- 2011-01, 	pkromer,	for ms precision used sys/time.h. The use of new methods Cpu* highly suggested.
 * 		- 2011-02,	pkromer,	old methods removed, made portable
 * 		- 2011-12,	pkromer,	ms precision for MS Visual Studio
 *
 */
#ifndef __CTIMER__
#define __CTIMER__

#ifdef __GNUC__
	#include <sys/time.h>
#else
	#include <windows.h>
#endif

#include <ctime>

namespace arg
{
	class cTimer
	{
	#ifndef __GNUC__
			struct timeval
			{
				unsigned long tv_sec;
				unsigned long tv_usec;
			};

			void gettimeofday(timeval * my_time, void * ignore) const
			{
				SYSTEMTIME systemTime;
				GetSystemTime( &systemTime );

				FILETIME fileTime;
				SystemTimeToFileTime( &systemTime, &fileTime );

				ULARGE_INTEGER uli;
				uli.LowPart = fileTime.dwLowDateTime;
				uli.HighPart = fileTime.dwHighDateTime;

				ULONGLONG systemTimeIn_ms( uli.QuadPart/10000 );

				ignore = ignore;
				(*my_time).tv_sec = (*my_time).tv_usec = 0;
				(*my_time).tv_sec = (unsigned long) systemTimeIn_ms/1000;
				(*my_time).tv_usec = (unsigned long) systemTimeIn_ms%1000;
			}
	#endif

		protected:
			timeval m_TickerStart;
			unsigned long m_TickerIntervalMS;
			timeval m_TStart, m_TStop;

		public:
			inline void CpuStart(void); 		///< Start interval on host.
			inline cTimer & CpuStop(void); 		///< Stop interval on host.
			inline double CpuMillis(void); 		///< Computes time interval on host in millis.
			inline double CpuSeconds(void); 	///< Computes time interval on host in seconds.

			inline void StartTicking(const unsigned long sec);
			inline bool IsTicking(void) const;
	};

	inline double cTimer::CpuMillis(void)
	{
		return ((m_TStop.tv_sec * 1e6 + m_TStop.tv_usec) - (m_TStart.tv_sec * 1e6 + m_TStart.tv_usec)) / 1e3;
	}

	inline double cTimer::CpuSeconds(void)
	{
		return CpuMillis() / 1e3;
	}

	inline void cTimer::CpuStart(void)
	{
		gettimeofday(&m_TStart, NULL);
	}

	inline cTimer& cTimer::CpuStop(void)
	{
		gettimeofday(&m_TStop, NULL);
		return *this;
	}

	inline void cTimer::StartTicking(const unsigned long sec)
	{
		gettimeofday(&m_TickerStart, NULL);
		m_TickerIntervalMS = sec * 1000;
	}

	inline bool cTimer::IsTicking(void) const
	{
		timeval now;
		gettimeofday(&now, NULL);

		double millis = ((now.tv_sec * 1e6 + now.tv_usec) - (m_TickerStart.tv_sec * 1e6 + m_TickerStart.tv_usec)) / 1e3;

		return (m_TickerIntervalMS > 0) && (millis < m_TickerIntervalMS);
	}
}

#endif
