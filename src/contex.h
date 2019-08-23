/*
 * 此文件主要为针对音视频数据解码和解码后数据的格式转换实现业务功能的封装
 * author:lihaiping1603@aliyun.com
 * date: 2018-11-23 create
*/

#ifndef __CONTEX_H_
#define __CONTEX_H_

#include "global.h"
#include "decoder.h"
#include "converter.h"
#include "public/loger/logtrace.h"
#include "public/cosmutex.h"
#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/mem.h"
#include "libavutil/rational.h"
#include "libavutil/error.h"
#include "libavutil/imgutils.h"
#include "libavutil/log.h"

#ifdef __cplusplus
}
#endif

using namespace PublicTool;

//初始化函数
int InitContexEnv();

class CContex
{
public:
    CContex();
    ~CContex();
    //打开文件数据流
    int OpenFile(const char *fileUrl);
    //读取一帧数据
    int GetFrame(MediaFrameInfo_S* pFrame);
private:
    CDecoder     *m_pDecoder;
    CConverter    *m_pConvert;
    COSMutex            m_cMutex;
    AVFrame *m_pDecFrame;
    AVConvertInfo_S *m_pCvtFrame;
	AVConvertInfo_S *m_pCatFrame;
    uint64_t m_lFrameSeq;
};

#endif // EASYMEDIACONTEX_H
