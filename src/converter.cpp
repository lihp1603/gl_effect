#include "converter.h"

#include "./public/loger/logtrace.h"
using namespace PublicTool;

CConverter::CConverter()
    :m_pImgConvertCtx(NULL)
    ,m_pAudSwrCtx(NULL)
{

}

CConverter::~CConverter()
{
    if(m_pImgConvertCtx!=NULL){
        sws_freeContext(m_pImgConvertCtx);
        m_pImgConvertCtx=NULL;
    }
    if(m_pAudSwrCtx){
        swr_free(&m_pAudSwrCtx);
        m_pAudSwrCtx=NULL;
    }
}

int CConverter::ImgConvert(AVFrame *iFrame, AVConvertInfo_S *oFrame,int wantFlags)
{
    int ret=0;
    m_pImgConvertCtx = sws_getCachedContext(m_pImgConvertCtx,
                                            iFrame->width, iFrame->height, (enum AVPixelFormat)iFrame->format,
                                            oFrame->nWidth, oFrame->nHeight,(enum AVPixelFormat)oFrame->nFormat,
                                            wantFlags, NULL, NULL, NULL);
    if(m_pImgConvertCtx!=NULL){
        sws_scale(m_pImgConvertCtx,
                 (const uint8_t * const *)iFrame->data, iFrame->linesize,
                 0, iFrame->height, oFrame->pData, oFrame->nLineSize);
        sws_freeContext(m_pImgConvertCtx);
        m_pImgConvertCtx=NULL;
    }else{
        LogTraceE("Cannot initialize the image conversion context");
        ret=-1;
    }
    return ret;
}

int CConverter::AudioConvert(AVFrame *iFrame, AVFrame *oFrame, int wantFmt)
{
    return -1;
}


