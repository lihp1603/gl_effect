/************************************************************************/
/*   
 * 此文件主要针对图片的处理进行的封装,例如调用std_image解码图片数据
 * author:lihaiping1603@aliyun.com
 * date: 2019-03-19
*/
/************************************************************************/

#ifndef __IMAGE_HELPER_H_
#define __IMAGE_HELPER_H_
#include "global.h"

class ImageHelper{
public:
	explicit ImageHelper(const char *fileUrl,PixFormat_E dirFmt);
	~ImageHelper();
	//读取一帧数据
	int GetFrame(MediaFrameInfo_S* pFrame);
private:
	uint8_t *m_pImageData;
	int32_t m_nWidth;
	int32_t m_nHeight;
	int32_t m_nChl;

};

#endif



