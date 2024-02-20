/**
 * \class arg::cSerializableVector
 * \brief
 *
 *  \author Pavel Kromer, (c) - 2010
 */

#ifndef CSERIALIZABLEVECTOR_H_
#define CSERIALIZABLEVECTOR_H_

#include <arg/core/cDebuggable.h>

#include <vector>
#include <set>
#include <fstream>

namespace arg
{
	template<class T>
	class cSerializableVector: public std::vector<T>, public cDebuggable
	{
		protected:
			bool Load(std::istream &);

		public:
			bool Load(const char*);
			bool Load(const std::string &);

			bool Contains(const T &);
			void Unique();
			void Fill(const T &);

			void operator+=(std::vector<T> &);
	};

	template<class T>
	void cSerializableVector<T>::operator+=(std::vector<T> & other)
	{
		if (this->size() != other.size())
		{
			err << "Wrong sizes for operator +=!.\n";
		}
		else
		{
			for (unsigned int i = 0; i < other.size(); i++)
				(*this)[i] += other[i];
		}
	}

	template<class T>
	bool cSerializableVector<T>::Load(std::istream & in)
	{
		dbg	<< "Loading map from stream.\n";
		in >> (*this);
		dbg	<< "Loaded " << this->size() << " terms.\n";
		return in.fail();
	}

	template<class T>
	bool cSerializableVector<T>::Load(const std::string & fname)
	{
		return Load(fname.c_str());
	}

	template<class T>
	bool cSerializableVector<T>::Load(const char * fname)
	{
		std::ifstream in;
		in.open(fname);

		bool result = false;

		if (in.is_open())
			result = Load(in);
		else
			err << "Cant open file \'" << fname << "\'.\n";

		in.close();
		return result;
	}

	template<class T>
	bool cSerializableVector<T>::Contains(const T & value)
	{
		for (typename cSerializableVector<T>::iterator i = this->begin(); i != this->end(); i++)
		{
			if ((*i) == value)
			{
				return true;
			}
		}

		return false;
	}

	template<class T>
	void cSerializableVector<T>::Fill(const T & value)
	{
		for (typename cSerializableVector<T>::iterator i = this->begin(); i != this->end(); i++)
		{
			(*i) = value;
		}
	}


	template<class T>
	void cSerializableVector<T>::Unique()
	{
		std::set<T> tmp_set;
		for (typename cSerializableVector<T>::iterator i = this->begin(); i != this->end(); i++)
		{
			tmp_set.insert((*i));
		}
		this->assign(tmp_set.begin(), tmp_set.end());
	}

	template<class T>
	std::ostream& operator<<(std::ostream& os, const cSerializableVector<T>& ts)
	{
		for (typename cSerializableVector<T>::const_iterator it = ts.begin(); it != ts.end(); it++)
		{
			os << *it << " ";
		}
		return os;
	}

	template<class T>
	std::istream& operator>>(std::istream& in, cSerializableVector<T>& ts)
	{
		while (true)
		{
			T t;
			in >> t;

			if (in.eof() || in.fail())
				break;
			ts.push_back(t);
		}
		return in;
	}

	typedef cSerializableVector<double> vecd;
	typedef cSerializableVector<int> veci;
	typedef cSerializableVector<long> vecl;
}

#endif /* CSERIALIZABLEVECTOR_H_ */
