/*
 * 此文件主要为针对音视频数据解码和解码后数据的格式转换实现业务功能的封装
 * author:lihaiping1603@aliyun.com
 * date: 2018-11-23 create
*/
#include "contex.h"
#include "global.h"

/*调试打印函数*/
void ffLoger(char *szFmt, ...)
{
    va_list argList;
    va_start(argList, szFmt);
    char szBuf[1024];
    memset(szBuf, 0, 1024);
    vsnprintf(szBuf, 1024, szFmt, argList);
    va_end(argList);
    //调用答应函数
    PublicTool::LogTraceI("%s",szBuf);
}


//运行时锁,回调函数锁定代码
static int ff_lockmgr_callback(void **mutex, enum AVLockOp op)
{
    switch(op)
    {
    case AV_LOCK_CREATE:///< Create a mutex
        {
#ifdef __WIN32__
            CRITICAL_SECTION *cs = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
            if(!cs){
                return 1;//失败
            }
            memset(cs,0,sizeof(CRITICAL_SECTION));
            InitializeCriticalSection(cs);
            *(CRITICAL_SECTION**)mutex = cs;
#else//linux
           pthread_mutex_t *fmutex=(pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
           if(!fmutex){
               return 1;//失败
           }
           memset(fmutex,0,sizeof(pthread_mutex_t));
           (void)pthread_mutex_init(fmutex, NULL);
           *(pthread_mutex_t**)mutex = fmutex;
#endif

#ifdef _BUG_PRINTF
            ffLoger("ff_lockmgr_callback CREATE");
#endif
        }
        break;
    case AV_LOCK_OBTAIN:///< Lock the mutex
        {
#ifdef __WIN32__
            if(mutex && *(CRITICAL_SECTION**)mutex)
             {
                 ::EnterCriticalSection(*(CRITICAL_SECTION**)mutex);
             }
#else
             if(mutex && *(pthread_mutex_t**)mutex)
             {
                (void)pthread_mutex_lock(*(pthread_mutex_t**)mutex);
             }
#endif

#ifdef _BUG_PRINTF
            ffLoger("ff_lockmgr_callback ENTER");
#endif
        }
        break;
    case AV_LOCK_RELEASE:///< Unlock the mutex
        {
#ifdef __WIN32__
            if(mutex && *(CRITICAL_SECTION**)mutex)
            {
                ::LeaveCriticalSection(*(CRITICAL_SECTION**)mutex);
            }
#else
            if(mutex && *(pthread_mutex_t**)mutex)
            {
               (void)pthread_mutex_unlock(*(pthread_mutex_t**)mutex);
            }
#endif
#ifdef _BUG_PRINTF
            ffLoger("ff_lockmgr_callback RELEASE");
#endif
        }
        break;
    case AV_LOCK_DESTROY:///< Free mutex resources
        {
#ifdef __WIN32__
            if(mutex && *(CRITICAL_SECTION**)mutex)
            {
                ::DeleteCriticalSection(*(CRITICAL_SECTION**)mutex);
                free(*(CRITICAL_SECTION**)mutex);
                *(CRITICAL_SECTION**)mutex = NULL;
            }
#else
            if(mutex && *(pthread_mutex_t**)mutex)
            {
                pthread_mutex_destroy(*(pthread_mutex_t**)mutex);
                free(*(pthread_mutex_t**)mutex);
                *(pthread_mutex_t**)mutex = NULL;
            }
#endif
#ifdef _BUG_PRINTF
            ffLoger("ff_lockmgr_callback DESTROY");
#endif
        }
        break;
    default:
        //break;
#ifdef _BUG_PRINTF
        ffLoger("ff_lockmgr_callback default");
#endif
        return 1;//失败
    }
    return 0;
}

//注册FFMPEG日志用的回调函数
static void ff_log_callback(void*avcl, int level, const char*fmt, va_list vl)
{
    char log[1024]={0};
    memset(log,0,sizeof(log));
    vsnprintf(log,sizeof(log),fmt,vl);
#ifdef _BUG_PRINTF
    ffLoger("%s",log);
#endif
}

//初始化函数
int InitContexEnv(){
    //1.先初始化日志打印系统
    InitLogTrace("easyMediaDecode","./log",LogTrace_INFO);
    //2.初始化ffmpeg全局环境
    //2.1先设在日志回调
    av_log_set_callback(ff_log_callback);
    //2.2 注册lock
    if (av_lockmgr_register(ff_lockmgr_callback)) {
           LogTraceE("Could not initialize lock manager!\n");
           return -1;
    }
    //2.3 初始化
    avcodec_register_all();
    av_register_all();
    avformat_network_init();
    return 0;
}

CContex::CContex()
    :m_pDecoder(NULL)
    ,m_pConvert(NULL)
    ,m_pDecFrame(NULL)
    ,m_pCvtFrame(NULL)
    ,m_lFrameSeq(0)
{

}

CContex::~CContex()
{
    if(m_pDecoder){
        delete m_pDecoder;
        m_pDecoder=NULL;
    }
    if(m_pConvert){
        delete m_pConvert;
        m_pConvert=NULL;
    }
    if(m_pDecFrame){
        av_frame_unref(m_pDecFrame);
        av_frame_free(&m_pDecFrame);
        m_pDecFrame=NULL;
    }
    if(m_pCvtFrame){
       av_freep(&m_pCvtFrame->pData[0]);
       free(m_pCvtFrame);
       m_pCvtFrame=NULL;
    }
}

int CContex::OpenFile(const char *fileUrl)
{
    if(!fileUrl){
        LogTraceE("the file url is empty");
        return -1;
    }
    if(m_pDecoder){
        delete m_pDecoder;
        m_pDecoder=NULL;
    }
    if(m_pConvert){
        delete m_pConvert;
        m_pConvert=NULL;
    }
    do{
        m_pDecoder=new CDecoder();
        if(m_pDecoder==NULL){
            LogTraceE("new media decode failed");
            break;
        }
        m_pConvert=new CConverter();
        if(m_pConvert==NULL){
            LogTraceE("new media convert failed");
            break;
        }
        COSMutexLocker Locker(&m_cMutex);
        return m_pDecoder->OpenFile(fileUrl);
    }while(0);

failed:
    if(m_pDecoder){
        delete m_pDecoder;
        m_pDecoder=NULL;
    }
    if(m_pConvert){
        delete m_pConvert;
        m_pConvert=NULL;
    }
    return -1;
}

int CContex::GetFrame(MediaFrameInfo_S *pFrame)
{
    int ret=0;
    //先分配一个用于存储解码数据后的数据结构
    if(m_pDecFrame==NULL){
        AVFrame *decFrame = av_frame_alloc();
        if(decFrame==NULL){
            LogTraceE("av alloc dec frame buffer failed.");
            return -1;
        }
        m_pDecFrame=decFrame;
    }
    do{
        COSMutexLocker Locker(&m_cMutex);
        //先将上次的数据内存进行释放
        av_frame_unref(m_pDecFrame);
        if(m_pDecoder&&m_pDecoder->GetFrame(m_pDecFrame)<=0){
            ret=-1;
            break;
        }

        //这里先将要转化的数据参数初始化到oframe中
        int frameSize=0;
        if(m_pCvtFrame==NULL){
            AVConvertInfo_S *cvtFrame = (AVConvertInfo_S*)malloc(sizeof(AVConvertInfo_S));
            if(cvtFrame==NULL){
                LogTraceE("av alloc convert frame buffer failed.");
                return -1;
            }
            m_pCvtFrame=cvtFrame;
            m_pCvtFrame->nFormat=AV_PIX_FMT_BGR24;//AV_PIX_FMT_RGB32;
            m_pCvtFrame->nWidth=m_pDecFrame->width;
            m_pCvtFrame->nHeight=m_pDecFrame->height;
            /* buffer is going to be written to rawvideo file, no alignment */
            if ((frameSize = av_image_alloc(m_pCvtFrame->pData, m_pCvtFrame->nLineSize,
                                      m_pCvtFrame->nWidth, m_pCvtFrame->nHeight, (enum AVPixelFormat)m_pCvtFrame->nFormat, 1)) < 0) {
                LogTraceE("Could not allocate destination image\n");
                free(m_pCvtFrame);
                m_pCvtFrame=NULL;
                break;
            }
        }

        m_pCvtFrame->nFormat=AV_PIX_FMT_BGR24;//AV_PIX_FMT_RGB32;
        m_pCvtFrame->nWidth=m_pDecFrame->width;
        m_pCvtFrame->nHeight=m_pDecFrame->height;
        frameSize=av_image_get_buffer_size((enum AVPixelFormat)m_pCvtFrame->nFormat,m_pCvtFrame->nWidth,m_pCvtFrame->nHeight,1);

        if(m_pConvert&&m_pConvert->ImgConvert(m_pDecFrame,m_pCvtFrame)<0){
            ret=-1;
            break;
        }
        //对数据进行拷贝
        pFrame->nWidth=m_pCvtFrame->nWidth;
        pFrame->nHeight=m_pCvtFrame->nHeight;
        pFrame->ePixFmt=PF_BGR24;//PF_RGB32;
        pFrame->lPts=m_pDecFrame->pts;
        pFrame->lSeq=m_lFrameSeq++;
        pFrame->nFrameSize=frameSize;
        pFrame->pFrame=m_pCvtFrame->pData[0];
        pFrame->fFps=m_pDecoder->GetVideoFps();
    }while(0);

    return ret;
}
