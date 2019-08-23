/************************************************************************/
/* 本文件主要是利用ffmpeg的解码功能解码视频文件  
*  author:lihaiping1603@aliyun.com
*  action:create  2019/01/18
*/
/************************************************************************/

#ifndef __CONVERTER_H_
#define __CONVERTER_H_

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


typedef struct __tagAudioParam_S {
	int32_t nFreq;//频率
	int32_t nChannels;//通道数
	int64_t nChannelLayout;//布局声道数
	enum AVSampleFormat nASFmt;//格式
	int32_t nFrameSize;//帧数据大小
	int32_t nBytesPerSec;//
	int32_t nNbSamples;//
} AudioParam_S;

typedef struct tagAVConvertInfo_S{
	uint8_t *pData[4];
	int32_t  nLineSize[4];
	//video
    int32_t nWidth;
    int32_t nHeight;
    int32_t nFormat;
	//audio
	AudioParam_S struAudioParam;
}AVConvertInfo_S;

class CConverter
{
public:
    CConverter();
    ~CConverter();
    //对video数据格式进行转化
    int ImgConvert(AVFrame *iFrame,AVConvertInfo_S *oFrame,int wantFlags=SWS_BICUBIC);
    //对音频数据进行转化
    int AudioConvert(AVFrame *iFrame,AVConvertInfo_S *oFrame,int wantFmt);
private:
    struct SwsContext *m_pImgConvertCtx;
    struct SwrContext *m_pAudSwrCtx;
    AVFrame *m_pFrame;
};

#endif // EASYMEDIACONVERT_H
