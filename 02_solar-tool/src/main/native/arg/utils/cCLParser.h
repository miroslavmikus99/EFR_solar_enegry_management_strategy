/**
 * \class arg::cCLParser
 * \brief Program command line parser.
 *
 * Parses the command line arguments in the form < -key value >, or < -switch >, or < unnamed >.
 * Unnamed parameter cannot follow after switch, such a pair will be interpreted as < -key value >.
 *
 * \b Examples:
 * 		- progname -switch1 -key1 value1 unnamed1
 *
 * \author Pavel Kromer, (c) 2009 - 2011
 *
 * \version 4.0
 *
 * \b History:
 *		- 2009,	pkromer,	initial version,
 *		- 2011, pkromer,	fixes
 *		- 2011,	pkromer,	changes, unnamed parameters
 *
 */
#ifndef CCLIPARSER_H_
#define CCLIPARSER_H_

#include <string>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <ostream>
#include <cstdlib>
#include <cstring>

namespace arg
{
	typedef std::map<std::string, std::string> ssmap;
	typedef std::set<std::string> sset;
	typedef std::vector<std::string> svector;

	class cCLParser
	{
			friend std::ostream& operator<<(std::ostream& os, const cCLParser& cl);

		protected:
			std::string mProgram; 	///< Program name
			ssmap mParams; 			///< Parameter map (key->value)
			sset mSwitches; 		///< Program line switches (without value, interpreted as true)
			svector mAnnonymous; 	///< Plain (unnamed, annonymous) cmdline arguments (neither <-key value> nor <-switch>)

		public:
			cCLParser(int argc, const char* argv[]);

			bool Boolean(const char *) const; 									///< Checks if certain switch was present
			const char * String(const char *, const char * def = NULL) const; 	///< Checks for value of certain argument
			bool StringValue(const char *, const char *) const; 				///< Checks for string argument with certain value
			int Integer(const char *, int def = -1) const; 						///< Checks for integer value of certain argument. Returns -1 if not present or not a number. Accepts default value as second argument.
			double Double(const char *, double def = -1) const; 				///< Checks for integer value of certain argument. Returns -1 if not present or not a number. Accepts default value as second argument.
			int Size(void) const;												///< Number of switches and parameters in command line.

			void Insert(const char * key, const char * value = NULL);			///< Insert key value pair. Can be used to override cmdline arguments. If value is NULL sets switch on.
			void Insert(const char * key, const double value);					///< Convenience method to insert number.

			const char * AnnonString(const unsigned int, const char * def = NULL) const;
			std::vector<std::string> StringList(const char * name, const char delim) const;
			std::vector<int> IntList(const char * name, const char delim, const char * def = NULL) const;
			std::vector<double> NumericalList(const char * name, const char delim) const;

			const char * Program(void) const;
	};
}

#endif /* CCLIPARSER_H_ */
