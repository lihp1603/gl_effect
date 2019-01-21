#include "cosmutex.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "loger/logtrace.h"

namespace PublicTool {


    // Private globals
    #if __PTHREADS_MUTEXES__
        static pthread_mutexattr_t  *sMutexAttr=NULL;
        static void MutexAttrInit();

        #if __solaris__
                static pthread_once_t sMutexAttrInit = {PTHREAD_ONCE_INIT};
        #else
                static pthread_once_t sMutexAttrInit = PTHREAD_ONCE_INIT;
        #endif

    #endif

    COSMutex::COSMutex()
    {
    #ifdef __Win32__
        ::InitializeCriticalSection(&fMutex);
        fHolder = 0;
        fHolderCount = 0;
    #elif __PTHREADS_MUTEXES__
        (void)pthread_once(&sMutexAttrInit, MutexAttrInit);
        (void)pthread_mutex_init(&fMutex, sMutexAttr);

        fHolder = 0;
        fHolderCount = 0;
    #else
        fMutex = mymutex_alloc();
    #endif
    }

    #if __PTHREADS_MUTEXES__
    void MutexAttrInit()
    {
        sMutexAttr = (pthread_mutexattr_t*)malloc(sizeof(pthread_mutexattr_t));
        ::memset(sMutexAttr, 0, sizeof(pthread_mutexattr_t));
        pthread_mutexattr_init(sMutexAttr);
    }
    #endif

    COSMutex::~COSMutex()
    {
    #ifdef __Win32__
        ::DeleteCriticalSection(&fMutex);
    #elif __PTHREADS_MUTEXES__
        pthread_mutex_destroy(&fMutex);
    #else
        mymutex_free(fMutex);
    #endif
    }


    #if __PTHREADS_MUTEXES__ || __Win32__
    void        COSMutex::RecursiveLock()
    {
        // We already have this mutex. Just refcount and return
        if (COSThread::GetCurrentThreadID() == fHolder)
        {
            fHolderCount++;
            return;
        }
    #ifdef __Win32__
        ::EnterCriticalSection(&fMutex);
    #else
        (void)pthread_mutex_lock(&fMutex);
    #endif
        if(fHolder == 0){
            fHolder = COSThread::GetCurrentThreadID();
            fHolderCount++;
            assert(fHolderCount == 1);
        }else{
             LogTraceW("maybe Repeat locked");
        }
        //这种写法会出现崩溃的情况
//        assert(fHolder == 0);
//        fHolder = COSThread::GetCurrentThreadID();
//        fHolderCount++;
//        assert(fHolderCount == 1);
    }

    void        COSMutex::RecursiveUnlock()
    {
        if (COSThread::GetCurrentThreadID() != fHolder){
            LogTraceW("maybe happed a bug");
            return;
        }

        assert(fHolderCount > 0);
        fHolderCount--;
//        LogTraceW("fHolder -- =%d",fHolder);
        if (fHolderCount == 0)
        {
            fHolder = 0;
    #ifdef __Win32__
            ::LeaveCriticalSection(&fMutex);
    #else
            pthread_mutex_unlock(&fMutex);
    #endif
        }
    }

    bool      COSMutex::RecursiveTryLock()
    {
        // We already have this mutex. Just refcount and return
        if (COSThread::GetCurrentThreadID() == fHolder)
        {
            fHolderCount++;
            return true;
        }

    #ifdef __Win32__
        bool theErr = (bool)::TryEnterCriticalSection(&fMutex); // Return values of this function match our API
        if (!theErr)
            return theErr;
    #else
        int theErr = pthread_mutex_trylock(&fMutex);
        if (theErr != 0)
        {
            assert(theErr == EBUSY);
            return false;
        }
    #endif
        assert(fHolder == 0);
        fHolder = COSThread::GetCurrentThreadID();
        fHolderCount++;
        assert(fHolderCount == 1);
        return true;
    }
    #endif //__PTHREADS_MUTEXES__ || __Win32__
}
