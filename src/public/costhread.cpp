#include "costhread.h"
#include "jthread/jthread.h"

using namespace jthread ;

namespace PublicTool {

    COSThread::COSThread()
        :JThread()
    {
        this->Start();
    }

    COSThread::~COSThread()
    {

    }

    void *COSThread::Thread()
    {
        //必须调用这个函数，告诉线程已经启动了
        this->ThreadStarted();
        this->Entry();
        return NULL;
    }
}
