/************************************************************************/
/* 本文件主要是opengl进行视频渲染,依赖于glad,glfw3和glm  
*  author:lihaiping1603@aliyun.com
*  action:create  2019/01/21
*/
/************************************************************************/
#ifndef __RENDER_H_
#define __RENDER_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include "stdint.h"
#include "global.h"

#define PIXEL_FORMAT (GL_BGR)

class CRender{
public:
	CRender();
	~CRender();
	//setup
	virtual int32_t SetupGL(uint32_t window_width,uint32_t window_height,uint32_t video_width,uint32_t video_height,const char* shaderPath);
	//启动opengl,包含创建窗口
	int32_t CreateEnvGL(int window_width,int window_height);
	//创建用的shader
	int32_t CreateShader( const char* v_shader_source,const char* f_shader_source );
	//创建顶点用的VAO,VBO
	void CreateVAO(const float vertexPostion[]=NULL,int32_t arrySize=0);
	//创建纹理
	uint32_t CreateTexture(uint32_t width,uint32_t height,PixFormat_E ePixFmt=PF_RGB24);
	//初始化uniforms变量的值
	virtual void InitUniforms();
	//渲染
	void Render(MediaFrameInfo_S *mainFrame);
	//根据effect文件名来加载shader内容
	int32_t LoadShader(const char* v_shader_source,const char* f_shader_source,const char* effectPath=NULL);
	//获取shader上下文
	Shader* GetShader();

protected:
	GLFWwindow* m_pWindow;
	Shader* m_pShaderCtx;
	uint32_t m_uVBO;
	uint32_t m_uVAO;
	uint32_t m_uMainTexture;
	uint32_t m_uWindowWidth,m_uWindowHeight;
	uint32_t m_uVideoWidth,m_uVideoHeight;
	uint32_t m_uProgressLoc;
	uint32_t m_uRatioLoc;
	uint32_t m_uFromRLoc;

};







#endif