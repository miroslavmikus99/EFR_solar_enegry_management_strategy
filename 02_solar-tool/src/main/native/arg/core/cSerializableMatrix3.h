/**
 * \class arg::cSerializableMatrix3
 * \brief
 *
 *  \author Pavel Kromer, (c) - 2010
 */

#ifndef cSerializableMatrix3_H_
#define cSerializableMatrix3_H_

#include <arg/ir/cSerializableVector.h>
#include <arg/core/cDebuggable.h>

#include <iomanip>

namespace arg
{

	template<class T>
	class cSerializableMatrix3: public cDebuggable
	{
		public:
			unsigned int mX, mXPhys;
			unsigned int mY, mYPhys;
			T* mData;

		public:
			cSerializableMatrix3(unsigned int x = 0, unsigned int y = 0, bool zeros = false);
			T * Row(unsigned int i);
			cSerializableVector<T> Col(unsigned int i);

			bool Resize(const unsigned int x, const unsigned int y, const bool reshape = false);
			void Fill(const T val);
			void FillRow(const unsigned int idx, const T val);
			void FillColumn(const unsigned int idx, const T val);

			unsigned int Rows()	{ return mX; };
			unsigned int Cols()	{ return mY; };

			T& operator()(unsigned int x, unsigned int y);
			cSerializableMatrix3<T>& operator=(const cSerializableMatrix3<T>& other);

			bool LoadBinary(const char * fname);
			bool Load(const char * fname);

			~cSerializableMatrix3();
	};

	template<class T>
	cSerializableMatrix3<T>& cSerializableMatrix3<T>::operator=(const cSerializableMatrix3<T>& other) // copy assignment
	{
	    if (this != &other)
	    {
	        if ((mData != NULL) && (mXPhys != other.mXPhys ||  mYPhys != other.mYPhys))
	        {
	        	delete[] mData;
	        	mData = NULL;
	        }

	        mXPhys = other.mXPhys;
	        mYPhys = other.mYPhys;
	        mX = other.mX;
	        mY = other.mY;

	        if (mData == NULL)
	        	mData = new double[mXPhys*mYPhys];

	        memcpy(mData, other.mData, mXPhys*mYPhys*sizeof(double));
	    }
	    return *this;
	}

	template<class T>
	T* cSerializableMatrix3<T>::Row(unsigned int x)
	{
		return &(mData[x * mYPhys]);
	}

	template<class T>
	T& cSerializableMatrix3<T>::operator()(unsigned int x, unsigned int y)
	{
		if (x < mX && y < mY)
			return mData[x * mYPhys + y];
		else
			err << "The coordinates [" << x << "," << y << "] are invalid for the matrix with dimensions " << mX
					<< " x " << mY << ".\n";
		// just to have the compiler shut up
		return mData[0];
	}

	template<class T>
	bool cSerializableMatrix3<T>::Resize(unsigned int x, unsigned int y, bool reshape)
	{
		bool res = true;
		if (x <= mX && y <= mY)
		{
			mX = x;
			mY = y;
		}
		else if (reshape)
		{
			if (mData != NULL)
			{
				delete [] mData;
				mData = NULL;
			}

			if (x*y != 0)
				mData = new T[x*y];

			mX = mXPhys = x;
			mY = mYPhys = y;
		}
		else
		{
			err << "Something failed/\n";
			res = false;
		}

		return res;
	}

	template<class T>
	bool cSerializableMatrix3<T>::LoadBinary(const char * fname)
	{
		std::ifstream out;
		out.open(fname, std::ios::in | std::ios::binary);

		bool success = false;

		if (!out.is_open())
		{
			err << "Could not open file \'" << fname << "\'.\n";
		}
		else
		{
			out.read((char *) &mX, sizeof mX);
			out.read((char *) &mY, sizeof mY);

			if (mData != NULL)
				delete[] mData;

			(*this).mData = new T[mX * mY];

			dbg << "Preparing to read " << (mX * mY * (sizeof(T))) << " bytes.\n";

			out.read((char*) mData, mX * mY * (sizeof(T)));

			mXPhys = mX;
			mYPhys = mY;

			dbg << "Matrix of the dimension " << mX << " x " << mY << " loaded.\n";

			out.close();
			success = true;
		}
		return success;
	}

	template<class T>
	cSerializableVector<T> cSerializableMatrix3<T>::Col(unsigned int y)
	{
		cSerializableVector<T> col;
		col.reserve(mX);

		for (unsigned int i = 0; i < mX; i++)
		{
			col.push_back(mData[i * mYPhys + y]);
		}
		return col;
	}

	template<class T>
	bool cSerializableMatrix3<T>::Load(const char * fname)
	{
		std::ifstream out;
		out.open(fname, std::ios::in);

		bool success = false;

		if (!out.is_open())
		{
			err << "Could not open file \'" << fname << "\'.\n";
		}
		else
		{
			out >> (*this);

			dbg << "Matrix of the dimension " << mX << " x " << mY << " loaded.\n";

			out.close();
			success = true;
		}
		return success;
	}

	template<class T>
	cSerializableMatrix3<T>::cSerializableMatrix3(unsigned int x, unsigned int y, bool zeros)
	{
		mXPhys = x;
		mYPhys = y;

		mX = mY = 0;

		if (x*y == 0)
			mData = NULL;
		else
		{
			mData = new T[x * y];

			if (zeros)
			{
				mX = x;
				mY = y;

				for (unsigned int i = 0; i < mX; i++)
				{
					for (unsigned int j = 0; j < mY; j++)
					{
						mData[i * mY + j] = 0;
					}
				}
			}
		}
	}

	template<class T>
	void cSerializableMatrix3<T>::Fill(const T val)
	{
		for (unsigned int i = 0; i < mXPhys*mYPhys; i++)
			mData[i] = val;
	}

	template<class T>
	void cSerializableMatrix3<T>::FillRow(const unsigned int row_idx, const T val)
	{
		for (unsigned int i = 0; i < mY; i++)
			mData[row_idx * mYPhys + i] = val;
	}

	template<class T>
	void cSerializableMatrix3<T>::FillColumn(const unsigned int col_idx, const T val)
	{
		for (unsigned int i = 0; i < mX; i++)
			mData[i * mYPhys + col_idx] = val;
	}

	template<class T>
	cSerializableMatrix3<T>::~cSerializableMatrix3()
	{
		delete[] mData;
	}

	template<class T>
	std::ostream& operator<<(std::ostream& os, const cSerializableMatrix3<T>& ts)
	{
		os << ts.mX << " " << ts.mY << "\n";

		for (unsigned int x = 0; x < ts.mX; x++)
		{
			for (unsigned int y = 0; y < ts.mY; y++)
			{
				os << std::setw(10) << ts.mData[x * ts.mYPhys + y] << " ";
			}
			os << "\n";
		}
		return os;
	}

	template<class T>
	std::istream& operator>>(std::istream& in, cSerializableMatrix3<T>& ts)
	{
		unsigned int x, y;

		in >> x;
		in >> y;

		if (ts.mData != NULL)
			delete[] ts.mData;

		ts.mData = new T[x * y];

		for (unsigned int i = 0; i < x * y; i++)
		{
			in >> ts.mData[i];
		}
		ts.mX = ts.mXPhys = x;
		ts.mY = ts.mYPhys = y;
		return in;
	}

	typedef cSerializableMatrix3<double> matd3;
	typedef cSerializableMatrix3<double> dmat;
}

#endif /* CSERIALIZABLEVECTOR_H_ */
