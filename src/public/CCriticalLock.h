/************************************************************************/
/* 
funtion:用于对临界区进行加锁,对linux和win环境下使用
author: -lhp
date:	-20151224
*/
/************************************************************************/
#ifndef  __CCRITICALLOCK_H_
#define  __CCRITICALLOCK_H_
//////////////////////////////////////////////////////////////////////////
namespace PublicTool{
	// scope locker
	template<typename LockerType>
	class CScopeLocker
	{
	public:
		explicit CScopeLocker(LockerType &locker)
			: m_locker(locker)
		{
			m_locker.Lock();
		}
		~CScopeLocker()
		{
			m_locker.UnLock();
		}	
	private:
		CScopeLocker(const CScopeLocker& rhs);
		CScopeLocker& operator=(const CScopeLocker& rhs);
	private:	
		LockerType &m_locker;
	};	

#ifdef __Win32__    //定义win32环境下的临界区
	//包含必须的头文件
#include <windows.h>
	class CCriticalSection
	{
	public:
		CCriticalSection()
		{
			InitializeCriticalSection(&m_pCrit);
		};
		~CCriticalSection()
		{

			DeleteCriticalSection(&m_pCrit);
		};
		void Lock()
		{
			EnterCriticalSection(&m_pCrit);
		};

		void UnLock()
		{

			LeaveCriticalSection(&m_pCrit);
		};

		void Enter() {  Lock();	}
		void Leave() {  UnLock();}


	private:
		CRITICAL_SECTION m_pCrit;
	};
	//CWinScopeCS
	typedef CScopeLocker<CCriticalSection> CritScope;

	//为了兼容之前的定义

#else//linux环境下

#include <pthread.h>
	class CCriticalSection
	{
	public:
		CCriticalSection()
		{
			pthread_mutex_init(&m_pMutex,NULL);
		}
		~CCriticalSection()
		{
			pthread_mutex_destroy(&m_pMutex);
		}

		void Lock()
		{
			pthread_mutex_lock(&m_pMutex);
		}
		void UnLock()
		{
			pthread_mutex_unlock(&m_pMutex);
		}
		void Enter() {  Lock();	}
		void Leave() {  UnLock();}
	private:
		pthread_mutex_t m_pMutex;
	};
	//CLinuxScopeCS
	typedef CScopeLocker<CCriticalSection> CritScope;

#endif

}

#endif
