/************************************************************************/
/* 本文件主要是opengl进行视频渲染,依赖于glad,glfw3和glm  
*  author:lihaiping1603@aliyun.com
*  action:create  2019/01/18
*/
/************************************************************************/
#ifndef __RENDER_H_
#define __RENDER_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include "stdint.h"
#include "global.h"

class CRender{
public:
	CRender();
	~CRender();
	//setup
	int32_t SetupGL(uint32_t window_width,uint32_t window_height,uint32_t video_width,uint32_t video_height,const char* transPath);
	//启动opengl,包含创建窗口
	int32_t CreateEnvGL(int window_width,int window_height);
	//创建转场用的shader
	int32_t CreateTransShader(const char* transPath);
	//创建顶点用的VAO,VBO
	void CreateTransVAO();
	//创建转场纹理
	void CreateTransTexture(uint32_t width,uint32_t height);
	//初始化uniforms变量的值
	void InitUniforms();
	//渲染
	void Render(MediaFrameInfo_S *fromFrame,const MediaFrameInfo_S *toFrame,const float fProgress);
private:
	GLFWwindow* m_pWindow;
	Shader* m_pShaderCtx;
	uint32_t m_uVBO;
	uint32_t m_uVAO;
	uint32_t m_uFromTexture;
	uint32_t m_uToTexture;
	uint32_t m_uWindowWidth,m_uWindowHeight;
	uint32_t m_uVideoWidth,m_uVideoHeight;
	uint32_t m_uProgressLoc;
	uint32_t m_uRatioLoc;
	uint32_t m_uFromRLoc;
	uint32_t m_uToRLoc;
	int64_t m_lFirstPts;

	//transition
	uint32_t m_uTransOffset;
	uint32_t m_uTransDuration;
	

};






#endif