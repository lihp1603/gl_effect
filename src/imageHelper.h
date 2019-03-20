/************************************************************************/
/*   
 * ���ļ���Ҫ���ͼƬ�Ĵ�����еķ�װ,�������std_image����ͼƬ����
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
	//��ȡһ֡����
	int GetFrame(MediaFrameInfo_S* pFrame);
private:
	uint8_t *m_pImageData;
	int32_t m_nWidth;
	int32_t m_nHeight;
	int32_t m_nChl;

};

#endif



