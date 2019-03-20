/**************************************************************************
 * 本文件主要是对glog的日志库进行了简单的封装和调用，方便后面的移植与兼容
 * author:lihaiping1603@aliyun.com
 * date:2018-11-21 create
**************************************************************************/
#ifndef __LOGTRACE_H_
#define __LOGTRACE_H_
#include <stdio.h>

namespace PublicTool {
    enum LogTraceServerity{
        LogTrace_INFO,
        LogTrace_WARNING,
        LogTrace_ERROR,
        LogTrace_FATAL
    };

    //初始化日志目录
    void InitLogTrace(const char*argv0,const char *logDir,int logServerity);
    //日志打印函数
    void LogTrace(int logServerity,const char *format,...);


    #ifdef __WIN32__

    //调试级别的日志信息打印
#define LogTraceD(format, ...) LogTrace(LogTrace_WARNING,format,##__VA_ARGS__)
    //一般信息日志打印
#define LogTraceI(format, ...) LogTrace(LogTrace_INFO,format,##__VA_ARGS__)
    //警告级别的日志信息打印
#define LogTraceW(format, ...) LogTrace(LogTrace_WARNING,format,##__VA_ARGS__)
    //错误级别的日志信息打印
#define LogTraceE(format, ...) LogTrace(LogTrace_ERROR,format,##__VA_ARGS__)

    #else //linux

    //调试级别的日志信息打印
    #define LogTraceD(format,...)  LogTrace(LogTrace_FATAL,format,##__VA_ARGS__)
    //一般信息日志打印
    #define LogTraceI(format,...)  LogTrace(LogTrace_INFO,format,##__VA_ARGS__)
    //警告级别的日志信息打印
    #define LogTraceW(format,...)  LogTrace(LogTrace_WARNING,format,##__VA_ARGS__)
    //错误级别的日志信息打印
    #define LogTraceE(format,...)  LogTrace(LogTrace_ERROR,format,##__VA_ARGS__)

    #endif
}
#endif // LOGTRACE_H
