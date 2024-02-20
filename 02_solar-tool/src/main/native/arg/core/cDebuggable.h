/**
 * \class arg::cDebuggable
 * \brief A class that enables debug information in descendants.
 *
 * A class that extends cDebuggable can use the macro dbg in a similar way as they
 * use the object cout, cerr etc. The method IsDebugging() can be used to determine
 * whether the object is in debugging mode or not.
 *
 * \section Usage
 * The \a host code can use the debugging functionality  e.g. by: <br/><br/>
 * <code>
 * 		DebuggableClass instance;							<br/>
 * 		instance.Debug();									<br/>
 * 		... // now the debugging messages are shown			<br/>
 * 		instance.Debug(false);								<br/>
 * 		... // now the debugging messages are not shown		<br/>
 * </code>
 * <br/>
 * The \a client code can use the debugging functionality by:	<br/><br/>
 * <code>
 * 		dbg << "A debugging message";					<br/>
 * 		err << "An error message";						<br/>
 * 		_dbg << "Unformatted (short) debugging message";<br/>
 * 		_err << "Unformatted (short) error message";	<br/>
 * </code>
 *
 * \author Pavel Krömer (pkromer), (c) 2009 - 2011
 *
 * \b History:
 * 		- 2010-05-01,	pkromer,	added comment macro (google: c++ comment macro). It improves the
 * 		  performance of debugged codes incredibly.
 * 		  To enable debugging. Compile with -D_DEBUG. (In CDT, c++/settings/c++ preprocessor)
 * 		- 2011-01020,	pkromer,	cleaned for AmphorA-ng. Among others, removed comment macro to
 * 		  avoid unreachable code. Such is life.
 *
 */

#ifndef CDEBUGGABLE_H_
#define CDEBUGGABLE_H_

#include <typeinfo>
#include <iostream>
#include <ostream>
#include <ctime>
#include <cerrno>
#include <cstring>

#define PREFIX if (this->IsDebugging())

namespace arg
{
	/**
	 * Defines the macro \a dbg that can be used for formatted debugging with info
	 * such as time, file, line.
	 */
	#define dbg PREFIX this->debug() << __FILE__ << ":" << __FUNCTION__ << "("<< __LINE__ <<") "
	/** Defines the macro \a _dbg that can be used for unformatted debugging. */
	#define _dbg PREFIX this->debug()

	/**
	 * Defines the macros \a err and \a _err for error messages.
	 */
	#define err this->error() << __FILE__ << ":"<< __FUNCTION__ << "("<< __LINE__ <<") "
	#define _err PREFIX this->error()

	/** Define structured log */
	#define slg if (m_StructLog) std::cout << "#SLG\t,\t"

	class cDebuggable
	{
			std::ostream & stamp(std::ostream & stream) const;

			bool m_Debug;

		protected:

			bool m_StructLog;

			std::ostream * m_DebugStream;
			std::ostream * m_ErrorStream;

			inline std::ostream & debug(void);
			inline std::ostream & error(void);

			inline std::string strerrno();

			/** Can be also used for code that is expensive to evaluate. */
			inline bool IsDebugging(void) const; ///< True if the object is in debugging mode.

		public:
			cDebuggable(void);
			inline void Debug(const bool val = true); ///< Turns on/off debugging mode.
			inline void StructLog(const bool val = true); ///< Turns on/off structured logging.
			virtual inline ~cDebuggable(void);
	};

	inline void cDebuggable::Debug(const bool val)
	{
		m_Debug = val;
	}

	inline void cDebuggable::StructLog(const bool val)
	{
		m_StructLog = val;
	}

	inline bool cDebuggable::IsDebugging(void) const
	{
		return m_Debug;
	}

	inline std::ostream & cDebuggable::error(void)
	{
		return stamp(*m_ErrorStream);
	}

	inline std::ostream & cDebuggable::debug(void)
	{
		return stamp(*m_DebugStream);
	}

	inline std::string cDebuggable::strerrno()
	{
		return strerror(errno);
	}

	inline cDebuggable::~cDebuggable()
	{
	}
}

#endif /* CDEBUGGABLE_H_ */
