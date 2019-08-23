/************************************************************************/
/* 本文件主要是利用ffmpeg的解码功能解码视频文件  
*  author:lihaiping1603@aliyun.com
*  action:create  2019/01/18
*/
/************************************************************************/
#ifndef __DECODER_H_
#define __DECODER_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/mem.h"
#include "libavutil/rational.h"
#include "libavutil/error.h"
#ifdef __cplusplus
}
#endif

class CDecoder
{
public:
    CDecoder();
    ~CDecoder();
    //打开媒体文件
    int OpenFile(const char* fileUrl);
    //获取一帧解码后的数据
	//streamIndex:0-video,1-audio;
    int GetFrame(AVFrame *oframe,int& streamIndex);
    //获取视频帧率
    float GetVideoFps();
	//获取时间调整
	long GetAVTimeBase();
private:
    //读取一个pkt数据
    int ReadPacket(AVPacket *pkt);
    //解码一个pkt数据
    int DecodeFrame(AVCodecContext *decCtx,AVPacket *pkt,AVFrame *frame);
private:
    bool m_bEof;
    AVFormatContext *m_pIfmtCtx;
    int m_nVideoStream;
    int m_nAudioStream;
    float m_fVideoFps;
    AVStream *m_pVideoSt;
	AVStream *m_pAudioSt;
    AVCodecContext *m_pVDecCtx;
    AVCodecContext *m_pADecCtx;
    AVFrame *m_pDecFrame;
};

#endif // EASYMEDIADECODE_H
