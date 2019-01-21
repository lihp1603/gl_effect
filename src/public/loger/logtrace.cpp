#include "logtrace.h"
//#include "../includes/glog/logging.h"
#include <stdarg.h>
#include <stdio.h>
#include <iostream>

namespace PublicTool {
    void InitLogTrace(const char *argv0, const char *logDir, int logServerity)
    {
#ifndef __WIN32__
        google::InitGoogleLogging(argv0);
        int setLogServerity=google::INFO;
        if(logServerity>=google::INFO&&logServerity<=google::FATAL){
           setLogServerity=logServerity;
        }
        google::SetLogDestination(setLogServerity,logDir);
#endif
    }

    void LogTrace(int logServerity,const char *format,...){
        char buffer[1024];

        va_list varArgs;
        va_start( varArgs, format );
        vsnprintf( buffer, sizeof(buffer), format, varArgs);
        va_end( varArgs );
#ifndef __WIN32__
        switch (logServerity) {
        case (int)LogTrace_INFO:
            LOG(INFO)<<buffer;
            break;
        case (int)LogTrace_WARNING:
            LOG(WARNING)<<buffer;
            break;
        case (int)LogTrace_ERROR:
            LOG(ERROR)<<buffer;
            break;
        default:
            LOG(FATAL)<<buffer;
            break;
        }
#else
		std::cout<<buffer<<std::endl;
#endif
    }
}



