/************************************************************************/
/* ���ļ���Ҫ��opengl������Ƶת��Ч�����ɵ���Ⱦ,������glad,glfw3��glm  
*  author:lihaiping1603@aliyun.com
*  action:create  2019/01/22
*/
/************************************************************************/

#ifndef __TRANSITION_RENDER_H_
#define __TRANSITION_RENDER_H_

#include "render.h"

//����һ��������Ⱦת��Ч������
class CRenderTransition:public CRender{
public:
	CRenderTransition();
	~CRenderTransition();
	//setup
	int32_t SetupGL(uint32_t window_width,uint32_t window_height,uint32_t video_width,uint32_t video_height,const char* shaderPath);

	//����ת���õ�shader
	int32_t CreateTransShader(const char* transPath);
	//����ת������
	void CreateTransTexture(uint32_t width,uint32_t height);
	//��ʼ��uniforms������ֵ
	void InitUniforms();
	//��Ⱦ
	void Render(MediaFrameInfo_S *fromFrame,const MediaFrameInfo_S *toFrame,float progress);

private:
	//����
	uint32_t m_uFromTexture;
	uint32_t m_uToTexture;

	uint32_t m_uProgressLoc;
	uint32_t m_uRatioLoc;
	uint32_t m_uFromRLoc;
	uint32_t m_uToRLoc;
};


#endif