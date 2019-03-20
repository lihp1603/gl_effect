/************************************************************************/
/*   
 * 此文件主要针对图片的处理进行的封装,例如调用std_image解码图片数据
 * author:lihaiping1603@aliyun.com
 * date: 2019-03-19
*/
/************************************************************************/
#include "imageHelper.h"
#include "stb_image.h"
#include "logtrace.h"
#include <exception>

ImageHelper::ImageHelper( const char *fileUrl,PixFormat_E dirFmt )
{
	try
	{
		if (PF_RGB32==dirFmt||PF_RGBA==dirFmt)
		{
			m_pImageData = stbi_load(fileUrl,&m_nWidth,&m_nHeight, &m_nChl, STBI_rgb_alpha);
		}else{
			m_pImageData = stbi_load(fileUrl,&m_nWidth,&m_nHeight, &m_nChl, STBI_rgb);
		}
	}
	catch (...)
	{
		//LogTraceE("stbi_load failed");
		throw "stbi_load failed";
	}
}

ImageHelper::~ImageHelper()
{
	if (m_pImageData)
	{
		stbi_image_free(m_pImageData);
		m_pImageData=NULL;
	}
}

int ImageHelper::GetFrame( MediaFrameInfo_S* pFrame )
{
	//对数据进行拷贝
	pFrame->nWidth=m_nWidth;
	pFrame->nHeight=m_nHeight;
	int32_t frameSize=m_nWidth*m_nHeight*m_nChl;
	if (m_nChl==4)
	{
		pFrame->ePixFmt=PF_RGB32;//PF_RGB32;
	}else{
		pFrame->ePixFmt=PF_BGR24;//PF_RGB32;
	}
	
	pFrame->lPts=0;
	pFrame->lSeq=0;
	pFrame->nFrameSize=frameSize;
	pFrame->pFrame=m_pImageData;
	pFrame->fFps=1;
	return 0;
}
