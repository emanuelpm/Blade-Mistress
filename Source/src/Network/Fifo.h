// ============================================================================
// 
// Fifo.h
// 
// By: Erik Dieckert
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================


#ifndef FIFO_H
#define FIFO_H

	// System Headers.
#include <stddef.h>

// =============================================
// Fifo
//
// This class defines a template for creating a 
// fifo of a defined size.
// =============================================

template <class T>
class Fifo
{
	public:

			// Creators.
		Fifo(int size);
		~Fifo();

			// Mutators.
		bool	Push(T item);
		bool	Pop(T & item);
		bool	GetTail(T & item);

			// Accessors.
		int		Size() const;
		bool	IsEmpty() const;
		bool	IsFull() const;

	private:

		T *							m_pool;
		int							m_head;
		int							m_tail;
		int							m_begin;
		int							m_end;
		int							m_size;
		mutable CRITICAL_SECTION	m_lock;
};

// =============================================
// Inline Functions.
// =============================================

template<class T>
inline Fifo<T>::Fifo(int size) : m_size(size)
{
	m_pool	= new T[m_size];
	m_begin = 0;
	m_tail	= 0;
	m_head	= 0;
	m_end	= size;
	InitializeCriticalSection(&m_lock);
}

template<class T>
inline Fifo<T>::~Fifo()
{
	DeleteCriticalSection(&m_lock);
	delete [] m_pool;
	m_begin = m_end = m_head = m_tail = 0;
	m_pool = NULL;
}

template<class T>
inline bool Fifo<T>::Push(T item)
{
	bool retval = false;

	EnterCriticalSection(&m_lock);
	
	if(!IsFull())
	{
		m_pool[m_head] = item;
		
		++m_head;

		if(m_head >= m_end)
		{
			m_head = m_begin;
		}

		retval = true;
	}

	LeaveCriticalSection(&m_lock);

	return retval;
}

template<class T>
inline bool Fifo<T>::Pop(T &val)
{
	bool retval = false;

	EnterCriticalSection(&m_lock);

	if(!IsEmpty())
	{
		val = m_pool[m_tail];

		++m_tail;
		
		if(m_tail >= m_end)
		{
			m_tail = m_begin;
		}

		retval = true;
	}

	LeaveCriticalSection(&m_lock);

	return retval;
}

template<class T>
inline bool Fifo<T>::GetTail(T &val)
{
	bool retval = false;

	EnterCriticalSection(&m_lock);

	if(!IsEmpty())
	{
		val = m_pool[m_tail];
		retval = true;
	}

	LeaveCriticalSection(&m_lock);

	return retval;
}

template<class T>
inline int Fifo<T>::Size() const
{
	int size = 0;

	EnterCriticalSection(&m_lock);

	if(m_head < m_tail)
	{
		size = m_size - (m_tail - m_head);
	}

	else
	{
		size = m_head - m_tail;
	}

	LeaveCriticalSection(&m_lock);

	return size;
}

template<class T>
inline bool Fifo<T>::IsEmpty() const
{
	EnterCriticalSection(&m_lock);
	
	bool retval	= m_head == m_tail;

	LeaveCriticalSection(&m_lock);

	return retval;
}

template<class T>
inline bool Fifo<T>::IsFull() const
{
	bool retval = false;

	EnterCriticalSection(&m_lock);
	
	if(m_tail == m_begin)
	{
		if(m_head == m_end - 1)
		{
			retval = true;
		}
	}

	else if(m_head == m_tail - 1)
	{
		retval = true;
	}

	LeaveCriticalSection(&m_lock);

	return retval;
}

#endif
