/**************************************************************************}
 {                                                                          }
 {    cIterator.h                                            		       }
 {                                                                          }
 {                                                                          }
 {                 Copyright (c) 1998, 2005					Vaclav Snasel  }
 {                                                                          }
 {    VERSION: 2.0										DATE 20/2/1998     }
 {                                                                          }
 {             following functionality:                                     }
 {                                                                          }
 {                                                                          }
 {    UPDATE HISTORY:                                                       }
 {                                                                          }
 {                                                                          }
 {**************************************************************************/

#ifndef __CITERATOR_H__
#define __CITERATOR_H__

namespace arg
{

	template<class T>
	class cIterator
	{
		protected:
			bool m_IterationComplete;
		public:
			cIterator(void);
			virtual void Next(void) = 0;
			virtual void Reset(void) = 0;
			virtual T& Data(void) = 0;
			inline bool isEnd(void) const;
	};

	template<class T>
	cIterator<T>::cIterator(void) :
		m_IterationComplete(false)
	{
	}

	template<class T>
	inline bool cIterator<T>::isEnd(void) const
	{
		return m_IterationComplete;
	}
}
#endif   // __CITERATOR_H__
