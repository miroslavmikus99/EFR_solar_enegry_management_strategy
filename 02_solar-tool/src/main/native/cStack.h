/***************************************************************************}
{																			}
{    cStack.h                                            		      		}
{																			}
{																			}
{                 Copyright (c) 1999, 2015					Vaclav Snasel	}
{																			}
{    VERSION: 2.0											DATE 20/2/1999	}
{																			}
{             following functionality:										}
{																			}
{																			}
{    UPDATE HISTORY:														}
{																			}
{																			}
{***************************************************************************/



#ifndef __CSTACK_H__
#define __CSTACK_H__

#include <stdio.h>

template <class T>
class cStack
{
public:
	cStack(const unsigned int Size = 1024*1024);
	cStack(T* Stack, unsigned int Size = 1024*1024);
	~cStack(void);
	inline void Push(const T Item);
	inline T Pop(void);
	inline T Top(void);
	inline T * TopPtr(void);
	inline bool isEmpty(void) const;
	inline void Clear(void);
	inline unsigned int Count(void) const;
	inline unsigned int Size(void) const;
private:
	T* m_Items;
	int m_Sp;
	unsigned int m_Size;
	bool m_Static;
};



template <class T>
cStack<T>::cStack(const unsigned int Size) : m_Sp(-1), m_Static(false)
{
	m_Size = Size;
	m_Items = new T[m_Size];
}


template <class T>
cStack<T>::cStack(T* Stack, unsigned int Size) : m_Sp(-1), m_Static(true)
{
	m_Size = Size;
	m_Items = Stack;
}

template <class T>
cStack<T>::~cStack(void)
{
	if (!m_Static)
	{
		delete[] m_Items;
	}
}

template <class T>
inline void cStack<T>::Push(const T Item)
{
	m_Items[++m_Sp] = Item;
}

template <class T>
inline T cStack<T>::Pop(void)
{
	return m_Items[m_Sp--];
}

template <class T>
inline T cStack<T>::Top(void)
{
	return m_Items[m_Sp];
}

template <class T>
inline bool cStack<T>::isEmpty(void) const
{
   return m_Sp == -1;
}


template <class T>
inline void cStack<T>::Clear(void)
{
	m_Sp = -1;
}

template <class T>
inline T * cStack<T>::TopPtr(void)
{
	return m_Items;
}

template <class T>
inline unsigned int cStack<T>::Count(void) const
{
	return (unsigned int)(m_Sp + 1);
}


template <class T>
inline unsigned int cStack<T>::Size(void) const
{
	return m_Size;
}


#endif            //    __CSTACK_H__
