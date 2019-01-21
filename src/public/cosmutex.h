#ifndef __COSMUTEX_H_
#define __COSMUTEX_H_
//#include "coscond.h"
#include "costhread.h"
#include <stdint.h>

namespace PublicTool {

    class COSMutex
    {
        public:

            COSMutex();
            ~COSMutex();

            void Lock();
			void Unlock();

            // Returns true on successful grab of the lock, false on failure
            bool TryLock();

        private:

    #ifdef __Win32__
            CRITICAL_SECTION fMutex;

            DWORD       fHolder;
            uint32_t      fHolderCount;

    #elif !__PTHREADS_MUTEXES__
            mymutex_t fMutex;
    #else
            pthread_mutex_t fMutex;
            // These two platforms don't implement pthreads recursive mutexes, so
            // we have to do it manually
            pthread_t   fHolder;
            uint32_t      fHolderCount;
    #endif

    #if __PTHREADS_MUTEXES__ || __Win32__
            void        RecursiveLock();
            void        RecursiveUnlock();
            bool        RecursiveTryLock();
    #endif
            friend class COSCond;
    };

    //自动锁
    class   COSMutexLocker
    {
        public:

            COSMutexLocker(COSMutex *inMutexP) : fMutex(inMutexP) { if (fMutex != NULL) fMutex->Lock(); }
            ~COSMutexLocker() {  if (fMutex != NULL) fMutex->Unlock(); }

            void Lock()         { if (fMutex != NULL) fMutex->Lock(); }
            void Unlock()       { if (fMutex != NULL) fMutex->Unlock(); }

        private:

            COSMutex*    fMutex;
    };

    inline void COSMutex::Lock()
    {
    #if __PTHREADS_MUTEXES__ || __Win32__
        this->RecursiveLock();
    #else
        mymutex_lock(fMutex);
    #endif //!__PTHREADS__
    }

    inline void COSMutex::Unlock()
    {
    #if __PTHREADS_MUTEXES__ || __Win32__
        this->RecursiveUnlock();
    #else
        mymutex_unlock(fMutex);
    #endif //!__PTHREADS__
    }

    inline bool COSMutex::TryLock()
    {
    #if __PTHREADS_MUTEXES__ || __Win32__
        return this->RecursiveTryLock();
    #else
        return (bool)mymutex_try_lock(fMutex);
    #endif //!__PTHREADS__
    }


}

#endif // COSMUTEX_H
