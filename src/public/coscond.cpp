#include "coscond.h"

#if __PTHREADS_MUTEXES__
#include <sys/time.h>
#endif

#include "stdint.h"
#include "assert.h"

#define Assert assert

namespace PublicTool {

    COSCond::COSCond()
    {
    #ifdef __Win32__
        fCondition = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        Assert(fCondition != NULL);
    #elif __PTHREADS_MUTEXES__
        #if __MacOSX__
            int ret = pthread_cond_init(&fCondition, NULL);
            Assert(ret == 0);
        #else
            pthread_condattr_t cond_attr;
            pthread_condattr_init(&cond_attr);
            int ret = pthread_cond_init(&fCondition, &cond_attr);
            Assert(ret == 0);
        #endif
    #else
//        fCondition = mycondition_alloc();
//        Assert(fCondition != NULL);
    #endif
    }

    COSCond::~COSCond()
    {
    #ifdef __Win32__
        BOOL theErr = ::CloseHandle(fCondition);
        Assert(theErr == TRUE);
    #elif __PTHREADS_MUTEXES__
        pthread_cond_destroy(&fCondition);
    #else
//        Assert(fCondition != NULL);
//        mycondition_free(fCondition);
    #endif
    }


    #if __PTHREADS_MUTEXES__
    void COSCond::TimedWait(COSMutex* inMutex, int32_t inTimeoutInMilSecs)
    {
        struct timespec ts;
        struct timeval tv;
        struct timezone tz;
        int sec, usec;

        //These platforms do refcounting manually, and wait will release the mutex,
        // so we need to update the counts here

        inMutex->fHolderCount--;
        inMutex->fHolder = 0;


        if (inTimeoutInMilSecs == 0)
            (void)pthread_cond_wait(&fCondition, &inMutex->fMutex);
        else
        {
            gettimeofday(&tv, &tz);
            sec = inTimeoutInMilSecs / 1000;
            inTimeoutInMilSecs = inTimeoutInMilSecs - (sec * 1000);
            Assert(inTimeoutInMilSecs < 1000);
            usec = inTimeoutInMilSecs * 1000;
            Assert(tv.tv_usec < 1000000);
            ts.tv_sec = tv.tv_sec + sec;
            ts.tv_nsec = (tv.tv_usec + usec) * 1000;
            Assert(ts.tv_nsec < 2000000000);
            if(ts.tv_nsec > 999999999)
            {
                 ts.tv_sec++;
                 ts.tv_nsec -= 1000000000;
            }
            (void)pthread_cond_timedwait(&fCondition, &inMutex->fMutex, &ts);
        }


        inMutex->fHolderCount++;
        inMutex->fHolder = pthread_self();

    }
    #endif
}
