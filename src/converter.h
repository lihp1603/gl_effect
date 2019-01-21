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

typedef struct tagAVConvertInfo_S{
    int32_t nWidth;
    int32_t nHeight;
    int32_t nFormat;
    uint8_t *pData[4];
    int32_t  nLineSize[4];
}AVConvertInfo_S;

class CConverter
{
public:
    CConverter();
    ~CConverter();
    //对video数据格式进行转化
    int ImgConvert(AVFrame *iFrame,AVConvertInfo_S *oFrame,int wantFlags=SWS_BICUBIC);
    //对音频数据进行转化
    int AudioConvert(AVFrame *iFrame,AVFrame *oFrame,int wantFmt);
private:
    struct SwsContext *m_pImgConvertCtx;
    struct SwrContext *m_pAudSwrCtx;
    AVFrame *m_pFrame;
};

#endif // EASYMEDIACONVERT_H
