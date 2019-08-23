/************************************************************************/
/* 本文件主要是利用ffmpeg的解码功能解码视频文件  
*  author:lihaiping1603@aliyun.com
*  action:create  2019/01/18
*/
/************************************************************************/

#include "decoder.h"
#include "public/loger/logtrace.h"

using namespace PublicTool;

CDecoder::CDecoder()
    :m_pIfmtCtx(NULL)
    ,m_pVideoSt(NULL)
	,m_pAudioSt(NULL)
    ,m_pVDecCtx(NULL)
    ,m_pADecCtx(NULL)
    ,m_pDecFrame(NULL)
    ,m_bEof(false)
    ,m_nVideoStream(-1)
    ,m_nAudioStream(-1)
    ,m_fVideoFps(0)
{
}

CDecoder::~CDecoder()
{
     if(m_pDecFrame!=NULL){
        av_frame_unref(m_pDecFrame);
        av_frame_free(&m_pDecFrame);
        m_pDecFrame=NULL;
     }
     if(m_pVDecCtx){
        avcodec_close(m_pVDecCtx);
        avcodec_free_context(&m_pVDecCtx);
        m_pVDecCtx=NULL;
     }
     if(m_pADecCtx){
         avcodec_close(m_pADecCtx);
         avcodec_free_context(&m_pADecCtx);
         m_pADecCtx=NULL;
     }
     if(m_pIfmtCtx){
        avformat_close_input(&m_pIfmtCtx);
        m_pIfmtCtx=NULL;
     }
}

int CDecoder::OpenFile(const char *fileUrl)
{
    int ret=0;
    int err=0;
    AVFormatContext *ifmt_ctx = NULL;
    if(fileUrl==NULL){
       LogTraceW("the open file url is empty,you should specified file url.");
       return -1;
    }

    ifmt_ctx = avformat_alloc_context();
    if (!ifmt_ctx) {
        LogTraceE("Could not allocate context.\n");
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    err = avformat_open_input(&ifmt_ctx, fileUrl, NULL, NULL);
    if (err < 0) {
        LogTraceE("open file:%s failed,err:%d",fileUrl, err);
        ret = -1;
        goto fail;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {
           LogTraceE("Cannot find stream information\n");
           goto fail;
    }
    for (unsigned int i = 0; i < ifmt_ctx->nb_streams; i++) {
           AVStream *stream = ifmt_ctx->streams[i];
           //这里只对音频和视频的数据进行分析
           if(stream->codec->codec_type==AVMEDIA_TYPE_VIDEO||
                   stream->codec->codec_type==AVMEDIA_TYPE_AUDIO){
               AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
               if (!dec) {
                   LogTraceE("Failed to find decoder for stream #%u\n", i);
                   ret = AVERROR_DECODER_NOT_FOUND;
                   goto fail;
               }
               AVCodecContext *codec_ctx = avcodec_alloc_context3(dec);
               if (!codec_ctx) {
                   LogTraceE("Failed to allocate the decoder context for stream #%u\n", i);
                   ret = AVERROR(ENOMEM);
                   goto fail;
               }
               ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
               if (ret < 0) {
                   LogTraceE("Failed to copy decoder parameters to input decoder context "
                             "for stream #%u\n", i);
                   goto fail;
               }
               /* Reencode video & audio and remux subtitles etc. */
               if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
                       || codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
                   /* Open decoder */
                   ret = avcodec_open2(codec_ctx, dec, NULL);
                   if (ret < 0) {
                       LogTraceE("Failed to open decoder for stream #%u\n", i);
                       goto fail;
                   }
               }
               if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO){
                   codec_ctx->framerate = av_guess_frame_rate(ifmt_ctx, stream, NULL);
                   int num=codec_ctx->framerate.num;
                   int den=codec_ctx->framerate.den;
                   //计算帧率
                   m_fVideoFps = 1.0*num/den;
                   m_pVDecCtx = codec_ctx;
                   m_nVideoStream=i;
				   m_pVideoSt=stream;
               }else{
                   m_nAudioStream=i;
                   m_pADecCtx=codec_ctx;
				   m_pAudioSt=stream;
               }
           }
    }
    m_pIfmtCtx=ifmt_ctx;
    return 0;
    fail:
       if(ifmt_ctx){
         avformat_close_input(&ifmt_ctx);
         ifmt_ctx=NULL;
       }
       if(m_pVDecCtx){
         avcodec_close(m_pVDecCtx);
         avcodec_free_context(&m_pVDecCtx);
         m_pVDecCtx=NULL;
       }
       if(m_pADecCtx){
           avcodec_close(m_pADecCtx);
           avcodec_free_context(&m_pADecCtx);
           m_pADecCtx=NULL;
       }
       return ret;
}

int CDecoder::GetFrame(AVFrame *oFrame,int& streamIndex)
{
    int ret=0;
    AVPacket pkt1, *pkt = &pkt1;
    if(oFrame==NULL){
        LogTraceE("you should speciefed buffer,the out frame param is null.");
        return -1;
    }
    //先分配一个用于存储解码数据后的数据结构
//    if(m_pDecFrame==NULL){
//        AVFrame *frame = av_frame_alloc();
//        if(frame==NULL){
//            LogTraceE("av alloc frame buffer failed.");
//            return -1;
//        }
//        m_pDecFrame=frame;
//    }
    //先将上次的数据内存进行释放
//    av_frame_unref(m_pDecFrame);
    do{
        if(m_bEof){//如果结束了
            //直接取解码器中的缓存数据
            //这里暂时取视频数据
            ret=DecodeFrame(m_pVDecCtx,NULL,oFrame);
            if(ret<0){
                LogTraceI("no more decode frame");
                break;
            }
        }else{
            ret=ReadPacket(pkt);
            if(ret<0){//读取文件出错
                LogTraceE("read packet failed,a error happened.");
                break;
            }
            if(ret>0){//读取到了数据
                 ret=0;//用于在非视频数据的情况下，能继续读取
                if(pkt->stream_index==m_nVideoStream){//读到视频数据，这我们送入解码器
                    ret=DecodeFrame(m_pVDecCtx,pkt,oFrame);
					streamIndex=0;
                }else if(pkt->stream_index==m_nAudioStream){
					ret=DecodeFrame(m_pADecCtx,pkt,oFrame);
					streamIndex=1;
                    //continue;
                }else{//暂时对于非音视频数据的进行丢弃
                    //continue;
                }
            }
            //这里对内存数据进行释放
            av_packet_unref(pkt);
        }
    }while (ret==0);

    //如果正常解码到了一帧数据
    return ret;
}

int CDecoder::ReadPacket(AVPacket *pkt)
{
    int ret=0;
    if(m_pIfmtCtx==NULL){
        LogTraceE("first:you shoud open file.");
        return -1;
    }
    do{
        av_init_packet(pkt);
        pkt->data = NULL;
        pkt->size = 0;
        ret=av_read_frame(m_pIfmtCtx,pkt);
        if(ret<0){
            if ((ret == AVERROR_EOF || avio_feof(m_pIfmtCtx->pb)) && !m_bEof) {
                m_bEof = true;
                ret=0;
                break;
            }
            if (m_pIfmtCtx->pb && m_pIfmtCtx->pb->error){
                ret = -1;
                break;
            }
            if(ret==AVERROR(EAGAIN)){
                ret = 0;
            }
        }else{
            //说明成功读取到了数据包
            ret = 1;
        }
    }while(0);
    return ret;
}

int CDecoder::DecodeFrame(AVCodecContext *decCtx,AVPacket *pkt,AVFrame *frame)
{
    int ret=0;
    int got_frame=0;
    if(decCtx==NULL){
        LogTraceW("you should open decoder context.");
        return -1;
    }
    do{
		//从解码器获取解码后的数据
		ret = avcodec_receive_frame(decCtx, frame);
		if (ret < 0){
			if(ret == AVERROR(EAGAIN)){//需要再尝试一次
				ret=0;
			}
		}else{
			 got_frame=1;
			 //这里对pts做简单的调整
			 if(decCtx&&decCtx->codec_type==AVMEDIA_TYPE_VIDEO){                   
				 frame->pts = frame->best_effort_timestamp;
				 if(frame->pts!=AV_NOPTS_VALUE){
					 //这个地方这么做的原因是因为frame的pts是int64类型的，而如果直接通过
					 //frame->pts * av_q2d(m_pVideoSt->time_base)得到的可能是一个小数，这样的结果是不对的，
					 //所以需要*GetAVTimeBase(),来得到一个整数
					 int64_t pts = frame->pts * av_q2d(m_pVideoSt->time_base)*GetAVTimeBase();
					 frame->pts = pts;
				 }				 
			 }else if(decCtx&&decCtx->codec_type==AVMEDIA_TYPE_AUDIO){
				 AVRational tb;
				 tb.num=1;
				 tb.den=frame->sample_rate;
				 if (frame->pts != AV_NOPTS_VALUE){
					 /*av_rescale_q(a,b,c):return (a*b/c) 取整*/
					 frame->pts = av_rescale_q(frame->pts, decCtx->pkt_timebase, tb);
					 frame->pts = frame->pts*av_q2d(tb)*GetAVTimeBase();
				 }
			 }
		}
		//将数据送入解码器
		if (pkt) {
			ret = avcodec_send_packet(decCtx, pkt);
			// In particular, we don't expect AVERROR(EAGAIN), because we read all
			// decoded frames with avcodec_receive_frame() until done.
			if (ret < 0 && ret != AVERROR_EOF)
				break;
		}
    }while(0);
	if(got_frame)
		ret=got_frame;
    return ret;
}

float CDecoder::GetVideoFps()
{
    if(m_fVideoFps>0){
        return m_fVideoFps;
    }
    else if(m_pIfmtCtx&&m_nVideoStream>=0){
        int num=m_pIfmtCtx->streams[m_nVideoStream]->avg_frame_rate.num;
        int den=m_pIfmtCtx->streams[m_nVideoStream]->avg_frame_rate.den;
        m_fVideoFps= 1.0*num/den;
        return m_fVideoFps;
    }
    return 0;
}

long CDecoder::GetAVTimeBase()
{
	return AV_TIME_BASE;
}










