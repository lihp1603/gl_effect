/************************************************************************/
/* 本文件主要是opengl进行视频转场效果过渡的渲染,依赖于glad,glfw3和glm  
*  author:lihaiping1603@aliyun.com
*  action:create  2019/01/22
*/
/************************************************************************/

#ifndef __TRANSITION_RENDER_H_
#define __TRANSITION_RENDER_H_

#include "render.h"

//创建一个用于渲染转场效果的类
class CRenderTransition:public CRender{
public:
	CRenderTransition();
	~CRenderTransition();
	//setup
	int32_t SetupGL(uint32_t window_width,uint32_t window_height,uint32_t video_width,uint32_t video_height,const char* shaderPath);

	//创建转场用的shader
	int32_t CreateTransShader(const char* transPath);
	//创建转场纹理
	void CreateTransTexture(uint32_t width,uint32_t height);
	//初始化uniforms变量的值
	void InitUniforms();
	//渲染
	void Render(MediaFrameInfo_S *fromFrame,const MediaFrameInfo_S *toFrame,float progress);

private:
	//纹理
	uint32_t m_uFromTexture;
	uint32_t m_uToTexture;

	uint32_t m_uProgressLoc;
	uint32_t m_uRatioLoc;
	uint32_t m_uFromRLoc;
	uint32_t m_uToRLoc;
};


#endif