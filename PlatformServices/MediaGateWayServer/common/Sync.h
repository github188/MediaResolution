#pragma once

#include <limits.h>
#include "winbase.h"

namespace HxShare
{

	//原子计数器
	class AtomicCounter
	{
	public:
		typedef LONG counter_type;

		AtomicCounter(counter_type val = 0) : counter(val) {}
		~AtomicCounter() {}

		counter_type exchangeAdd(counter_type val) {
			return InterlockedExchangeAdd(&counter, val);
		}

		counter_type operator +=(counter_type val) {
			return exchangeAdd(val) + val;
		}

		counter_type operator -=(counter_type val) {
			return exchangeAdd(-val) - val;
		}

		counter_type operator ++() {
			return InterlockedIncrement(&counter);
		}

		counter_type operator --() {
			return InterlockedDecrement(&counter);
		}

		counter_type value() const { return counter; }

	private:
		volatile counter_type counter;
	};



	//Sync
	class Sync
	{
	public:
		virtual void lock(){};
		virtual void unlock(){};
	};

	//Mutex
	class Mutex : public Sync
	{
	public:
		Mutex()
		{
#ifdef DEBUG
			m_lThreadID = 0;
#endif
			InitializeCriticalSection(&m_cs);
		};
		~Mutex()
		{
			DeleteCriticalSection(&m_cs);
		};

	public:
		void lock()
		{
#ifdef DEBUG
			//不能重入Enter
			long lThreadID = GetCurrentThreadId();

			if(lThreadID == m_lThreadID)
			{
				ATLASSERT(FALSE);
			}
#endif			
			__try
			{
				EnterCriticalSection(&m_cs);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{}
#ifdef DEBUG
			InterlockedExchange(&m_lThreadID, lThreadID) ;
#endif	

		}

		void unlock()
		{
#ifdef DEBUG
			//加锁和解锁必须是同一线程
			long lThreadID = GetCurrentThreadId();

			if(lThreadID != m_lThreadID)
			{
				ATLASSERT(FALSE);
			}
			
			InterlockedExchange(&m_lThreadID, 0) ;
#endif
			__try
			{
				LeaveCriticalSection(&m_cs);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{}
			
		}

		void clear()
		{
			DeleteCriticalSection(&m_cs);
			InitializeCriticalSection(&m_cs);
#ifdef DEBUG
			InterlockedExchange(&m_lThreadID, 0) ;
#endif
		}

	protected:
#ifdef DEBUG
		DWORD m_lThreadID;
#endif
		CRITICAL_SECTION m_cs;
	};

	//Event
//	class Event : public Sync
//	{
//	public:
//		Event()
//		{
//			m_event = CreateEvent(NULL,FALSE,FALSE,NULL);
//		}
//		~Event()
//		{
//			__try
//			{
//				CloseHandle(m_event);
//			}
//			__except (EXCEPTION_EXECUTE_HANDLER)
//			{
////				HxTrace("Close event exception");
//			}
//		}
//		void lock()
//		{
//			WaitForSingleObject(m_event,INFINITE);
//		}
//		void lock(int time_out)
//		{
//			wait(time_out);
//		}
//		bool wait()
//		{
//			if ( WaitForSingleObject(m_event,INFINITE) == WAIT_TIMEOUT )
//				return false;
//
//			return true;
//		}
//		bool wait(int time_out)
//		{
//			if (WaitForSingleObject(m_event,time_out) == WAIT_TIMEOUT)
//				return false;
//
//			return true;
//		}
//		void unlock()
//		{
//			SetEvent(m_event);
//		}
//
//		void set()
//		{
//			SetEvent(m_event);
//		}
//		void reset()
//		{
//			ResetEvent(m_event);
//		}
//
//	private:
//		HANDLE m_event;
//	};


	/*class Semaphore : public Sync
	{
	public:
		Semaphore()
		{
			m_hSem = CreateSemaphore(NULL,0,INT_MAX,NULL);
		}
		~Semaphore()
		{
			if (m_hSem)
			{
				CloseHandle(m_hSem);
			}
		}
		void lock()
		{
			WaitForSingleObject(m_hSem,INFINITE);
		}
		void unlock()
		{
			release(1);
		}
		void lock(int time_out)
		{
			wait(time_out);
		}
		bool wait(int time_out)
		{
			if (WaitForSingleObject(m_hSem,time_out)==WAIT_TIMEOUT)
				return false;

			return true;
		}
		void release(int count)
		{
			ReleaseSemaphore(m_hSem,count,NULL);
		}

	private:
		HANDLE m_hSem;
	};*/

	//AutoLock
	class AutoLock
	{
	public:
		AutoLock()
		{
			m_lock = NULL;
			m_lock_yn = false;
		}
		AutoLock(Sync* lock,bool lock_yn = true)
		{
			m_lock = lock;
			m_lock_yn = lock_yn;
			if (lock_yn)
			{
				m_lock->lock();
			}
		}
		~AutoLock()
		{
			unlock();
		}
		void lock()
		{
			m_lock->lock();
			m_lock_yn = true;
		}
		void unlock()
		{
			if (m_lock_yn)
			{
				m_lock->unlock();
				m_lock_yn = false;
			}
		}
	private:
		Sync*	m_lock;
		bool	m_lock_yn;
	};
}