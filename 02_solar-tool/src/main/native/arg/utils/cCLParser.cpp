#include "cCLParser.h"

using namespace std;

namespace arg
{
	cCLParser::cCLParser(int argc, const char* argv[])
	{
		if (argc > 0 && argv != NULL)
		{
			mProgram = argv[0];
		}

		char * key = NULL;
		double val = 0;
		for (int i = 1; i < argc; i++)
		{
			const char * current = argv[i];
			val = atof(current);

			if (current[0] == '-' && val == 0 && key == NULL)
			{
				key = (char*) current + 1;
			}
			else if (current[0] == '-' && val < 0 && key != NULL)
			{
				mParams[key] = current;
				key = NULL;
			}
			else if (current[0] != '-' && key != NULL)
			{
				mParams[key] = current;
				key = NULL;
			}
			else if (current[0] == '-' && key != NULL)
			{
				mSwitches.insert(key);
				key = (char*) current + 1;
			}
			else if (current[0] != '-' && key == NULL)
			{
				mAnnonymous.push_back(current);
			}
		}
		if (key != NULL)
		{
			mSwitches.insert(key);
		}
	}

	void cCLParser::Insert(const char * key, const char * value)
	{
		if (value == NULL)
		{
			mSwitches.insert(key);
		}
		else
		{
			mParams[key] = value;
		}
	}

	void cCLParser::Insert(const char * key, const double value)
	{
		std::stringstream ss;
		ss << value;

		mParams[key] = ss.str();
	}

	bool cCLParser::Boolean(const char * key) const
	{
		string tmp = key;

		if (mSwitches.size() == 0)
		{
			return false;
		}
		sset::iterator it = mSwitches.find(tmp);
		return it != mSwitches.end();
	}

	const char * cCLParser::String(const char * key, const char * def) const
	{
		string tmp = key;

		if (mParams.size() == 0)
		{
			return def;
		}

		ssmap::const_iterator it = mParams.find(tmp);

		if (it == mParams.end())
		{
			return def;
		}
		else
		{
			return it->second.c_str();
		}
	}

	int cCLParser::Integer(const char * key, int def) const
	{
		const char * tmp = String(key);
		if (tmp == NULL)
		{
			//convention
			return def;
		}
		else
		{
			return atoi(tmp);
		}
	}

	double cCLParser::Double(const char * key, double def) const
	{
		const char * tmp = String(key);
		if (tmp == NULL)
		{
			//convention
			return def;
		}
		else
		{
			return atof(tmp);
		}
	}

	const char * cCLParser::Program(void) const
	{
		return mProgram.c_str();
	}

	int cCLParser::Size(void) const
	{
		return mParams.size() + mSwitches.size() + mAnnonymous.size();
	}

	const char * cCLParser::AnnonString(const unsigned int idx, const char * def) const
	{
		if (mAnnonymous.size() < idx + 1)
		{
			return def;
		}

		return mAnnonymous[idx].c_str();
	}

	vector<string> cCLParser::StringList(const char * name, const char delim) const
	{
		vector<string> res;
		const char * str = String(name);

		if (str != NULL)
		{
			stringstream ss(str);
			string item;
			while (std::getline(ss, item, delim))
			{
				if (item.size() > 0)
					res.push_back(item);
			}
		}
		return res;
	}

	vector<double> cCLParser::NumericalList(const char * name, const char delim) const
	{
		vector<double> res;
		const char * str = String(name);
		if (str != NULL)
		{
			stringstream ss(str);
			string item;
			while (std::getline(ss, item, delim))
			{
				if (item.size() > 0)
					res.push_back(atof(item.c_str()));
			}
		}
		return res;
	}

	vector<int> cCLParser::IntList(const char * name, const char delim, const char * def) const
	{
		vector<int> res;
		const char * str = String(name);

		if (str == NULL)
			str = def;

		if (str != NULL)
		{
			stringstream ss(str);
			string item;
			while (std::getline(ss, item, delim))
			{
				if (item.size() > 0)
					res.push_back(atof(item.c_str()));
			}
		}
		return res;
	}

	bool cCLParser::StringValue(const char * name, const char * value) const
	{
		const char * str = String(name);
		bool result = false;

		if (str!= NULL)
		{
			result = strcmp(str,value) == 0;
		}

		return result;
	}

	ostream& operator<<(ostream& os, const cCLParser& cl)
	{
		os << "#" << cl.mProgram << " command line parameters :\n";
		os << "#" << "\tSwitches:\n";
		for (sset::iterator it = cl.mSwitches.begin(); it != cl.mSwitches.end(); it++)
		{
			os << "#" << "\t\t-" << *it << "\n";
		}
		os << "#" << "\tArguments:\n";
		for (ssmap::const_iterator it = cl.mParams.begin(); it != cl.mParams.end(); it++)
		{
			os << "#" << "\t\t-" << it->first << " " << it->second << "\n";
		}
		os << "#" << "\tUnnamed params:\n";
		for (svector::const_iterator it = cl.mAnnonymous.begin(); it != cl.mAnnonymous.end(); it++)
		{
			os << "#" << "\t\t" << *it << "\n";
		}
		return os;
	}
}
