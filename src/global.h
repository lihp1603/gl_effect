#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "stdint.h"

// 媒体数据类型
typedef enum {
    MEDIA_TYPE_UNKNOWN = -1,
    MEDIA_TYPE_VIDEO,
    MEDIA_TYPE_AUDIO,
}MediaType_E;


//图像数据格式
typedef enum {
    //视频图像格式
    PF_UNKNOWN= -1,
    PF_YV12= 100, /**< Planar mode: Y + V + U  (3 planes) */
    PF_IYUV, /**< Planar mode: Y + U + V  (3 planes) */
    //暂不支持
    PF_YUY2, /**< Packed mode: Y0+U0+Y1+V0 (1 plane) */
    PF_UYVY, /**< Packed mode: U0+Y0+V0+Y1 (1 plane) */
    PF_YVYU, /**< Packed mode: Y0+V0+Y1+U0 (1 plane) */
    PF_RGB24,
    PF_BGR24,
    PF_RGB32,  /* ARGB,BGRA*/
	PF_RGBA    /*packed RGBA 8:8:8:8, 32bpp, RGBARGBA...*/
}PixFormat_E;

//音频采样格式
typedef enum {
    SAMPLE_FMT_NONE = -1,
    SAMPLE_FMT_U8,          ///< unsigned 8 bits
    SAMPLE_FMT_S16,         ///< signed 16 bits
    SAMPLE_FMT_S32,         ///< signed 32 bits
    SAMPLE_FMT_FLT,         ///< float
    SAMPLE_FMT_DBL,         ///< double

    SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
    SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
    SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
    SAMPLE_FMT_FLTP,        ///< float, planar
    SAMPLE_FMT_DBLP,        ///< double, planar

    SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
}AudioSampleFmt_E;

//帧数据基本格式信息
typedef struct tagMediaFrameInfo_S{
    uint8_t *pFrame;//帧数据
    int32_t nFrameSize;//帧数据大小
	MediaType_E eMediaType;//媒体类型
    PixFormat_E ePixFmt;//帧数据格式
    int32_t nWidth;//图像宽
    int32_t nHeight;//图像高度
    int64_t lPts;//图像的显示时间戳
	double  dwFrameTime;//帧数据时间
    float fFps;//视频帧率
    int64_t lSeq;//序列号
    int32_t nSampleRate;//采样率
    uint64_t lChanels;//通道数
	AudioSampleFmt_E eSampleFmt;//采样格式
    void *pRes;
    int32_t nResSize;
}MediaFrameInfo_S;



#endif // EASYMEDIADECODE_GLOBAL_H
