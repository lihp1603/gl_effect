#ifndef __COSTHREAD_H_
#define __COSTHREAD_H_
#include "jthread/jthread.h"

using namespace jthread ;
namespace PublicTool {

    //定义一个系统线程类
    class COSThread:public JThread
    {
    public:
        COSThread();
        virtual ~COSThread();
        //线程切入，入口函数
        virtual void Entry()=0;

    #ifdef __Win32__
        static DWORD        GetCurrentThreadID() { return ::GetCurrentThreadId(); }
    #elif __PTHREADS__
        static  pthread_t   GetCurrentThreadID() { return ::pthread_self(); }
    #else
        static  cthread_t   GetCurrentThreadID() { return cthread_self(); }
    #endif
    private:
        //线程任务函数，私有了
        virtual void *Thread();
    };
}

#endif // COSTHREAD_H
