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

int CConverter::AudioConvert(AVFrame *iFrame, AVConvertInfo_S *oFrame, int wantFmt)
{
	int data_size, resampled_data_size;
	int64_t dec_channel_layout;
	int wanted_nb_samples=oFrame->struAudioParam.nNbSamples;
	if (wanted_nb_samples<=0)
	{
		wanted_nb_samples=iFrame->nb_samples;
	}
	if (iFrame==NULL||iFrame->data==NULL
		||oFrame==NULL||oFrame->pData[0]==NULL)
	{
		 LogTraceE("the the audio convert frame param is invalid");
		 return -1;
	}
	
	//获取帧数据大小
	data_size = av_samples_get_buffer_size(NULL, iFrame->channels,
		iFrame->nb_samples,
		(enum AVSampleFormat)iFrame->format, 1);

	dec_channel_layout =
		(iFrame->channel_layout && iFrame->channels == av_get_channel_layout_nb_channels(iFrame->channel_layout)) ?
		iFrame->channel_layout : av_get_default_channel_layout(iFrame->channels);
	if(iFrame->format        != oFrame->struAudioParam.nASFmt            ||
		dec_channel_layout       != oFrame->struAudioParam.nChannelLayout ||
		iFrame->sample_rate   != oFrame->struAudioParam.nFreq ||  
		(iFrame->nb_samples!=wanted_nb_samples&&!m_pAudSwrCtx)){
		swr_free(&m_pAudSwrCtx);
		m_pAudSwrCtx = swr_alloc_set_opts(NULL,
			oFrame->struAudioParam.nChannelLayout, oFrame->struAudioParam.nASFmt, oFrame->struAudioParam.nFreq,
			iFrame->channel_layout,          (enum AVSampleFormat)iFrame->format, iFrame->sample_rate,
			0, NULL);
		if (!m_pAudSwrCtx || swr_init(m_pAudSwrCtx) < 0) {
			LogTraceE("Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!",
				iFrame->sample_rate, av_get_sample_fmt_name((enum AVSampleFormat)iFrame->format), iFrame->channels,
				oFrame->struAudioParam.nFreq, av_get_sample_fmt_name(oFrame->struAudioParam.nASFmt), oFrame->struAudioParam.nChannels);
			swr_free(&m_pAudSwrCtx);
			return -1;
		}
	}
	const uint8_t **in = (const uint8_t **)iFrame->extended_data;
	uint8_t **out = &oFrame->pData[0];
	int32_t out_count = (int64_t)wanted_nb_samples * oFrame->struAudioParam.nFreq / iFrame->sample_rate + 256;
	if(m_pAudSwrCtx!=NULL&&in!=NULL&&out!=NULL){
		int len2 = swr_convert(m_pAudSwrCtx, out, out_count, in,iFrame->nb_samples);
		if (len2 < 0) {
			LogTraceE("swr_convert() failed");
			return -1;
		}
		if (len2 == out_count) {
			LogTraceI("audio buffer is probably too small\n");
			if (swr_init(m_pAudSwrCtx) < 0)
				swr_free(&m_pAudSwrCtx);
				m_pAudSwrCtx=NULL;
		}
		resampled_data_size = len2 * oFrame->struAudioParam.nChannels * av_get_bytes_per_sample(oFrame->struAudioParam.nASFmt);
		oFrame->struAudioParam.nFrameSize=resampled_data_size;
	}else{
		oFrame->pData[0] = iFrame->data[0];
		resampled_data_size = data_size;
		oFrame->struAudioParam.nFrameSize=resampled_data_size;
	}
	
    return resampled_data_size;
}


