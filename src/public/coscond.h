#ifndef __COSCOND_H_
#define __COSCOND_H_

#ifndef __Win32__
    #if __PTHREADS_MUTEXES__
        #include <pthread.h>
    #else
//        #include "mycondition.h"
    #endif
#else
#include <winsock2.h>
#include <windows.h> 
#include <assert.h>
#endif

#include "stdint.h"
#include "cosmutex.h"


namespace PublicTool {

    class COSMutex;
    class COSCond
    {
        public:

            COSCond();
            ~COSCond();

            inline void     Signal();
            inline void     Wait(COSMutex* inMutex, int32_t inTimeoutInMilSecs = 0);
            inline void     Broadcast();

        private:

    #ifdef __Win32__
            HANDLE              fCondition;
            uint32_t              fWaitCount;
    #elif __PTHREADS_MUTEXES__
            pthread_cond_t      fCondition;
            void                TimedWait(COSMutex* inMutex, int32_t inTimeoutInMilSecs);
    #else
//            mycondition_t       fCondition;
    #endif
    };


    inline void COSCond::Wait(COSMutex* inMutex, int32_t inTimeoutInMilSecs)
    {
    #ifdef __Win32__
        DWORD theTimeout = INFINITE;
        if (inTimeoutInMilSecs > 0)
            theTimeout = inTimeoutInMilSecs;
        inMutex->Unlock();
        fWaitCount++;
        DWORD theErr = ::WaitForSingleObject(fCondition, theTimeout);
        fWaitCount--;
        assert((theErr == WAIT_OBJECT_0) || (theErr == WAIT_TIMEOUT));
        inMutex->Lock();
    #elif __PTHREADS_MUTEXES__
        this->TimedWait(inMutex, inTimeoutInMilSecs);
    #else
//        assert(fCondition != NULL);
//        mycondition_wait(fCondition, inMutex->fMutex, inTimeoutInMilSecs);
    #endif
    }

    inline void COSCond::Signal()
    {
    #ifdef __Win32__
        BOOL theErr = ::SetEvent(fCondition);
        assert(theErr == TRUE);
    #elif __PTHREADS_MUTEXES__
        pthread_cond_signal(&fCondition);
    #else
//        assert(fCondition != NULL);
//        mycondition_signal(fCondition);
    #endif
    }

    inline void COSCond::Broadcast()
    {
    #ifdef __Win32__
        //
        // There doesn't seem like any more elegant way to
        // implement Broadcast using events in Win32.
        // This will work, it may generate spurious wakeups,
        // but condition variables are allowed to generate
        // spurious wakeups
        uint32_t waitCount = fWaitCount;
        for (uint32_t x = 0; x < waitCount; x++)
        {
            BOOL theErr = ::SetEvent(fCondition);
            assert(theErr == TRUE);
        }
    #elif __PTHREADS_MUTEXES__
        pthread_cond_broadcast(&fCondition);
    #else
//        assert(fCondition != NULL);
//        mycondition_broadcast(fCondition);
    #endif
    }
}
#endif // COSCOND_H
