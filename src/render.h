/************************************************************************/
/* ���ļ���Ҫ��opengl������Ƶ��Ⱦ,������glad,glfw3��glm  
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
	//����opengl,������������
	int32_t CreateEnvGL(int window_width,int window_height);
	//�����õ�shader
	int32_t CreateShader( const char* v_shader_source,const char* f_shader_source );
	//���������õ�VAO,VBO
	void CreateVAO(const float vertexPostion[]=NULL,int32_t arrySize=0);
	//��������
	uint32_t CreateTexture(uint32_t width,uint32_t height,PixFormat_E ePixFmt=PF_RGB24);
	//��ʼ��uniforms������ֵ
	virtual void InitUniforms();
	//��Ⱦ
	void Render(MediaFrameInfo_S *mainFrame);
	//����effect�ļ���������shader����
	int32_t LoadShader(const char* v_shader_source,const char* f_shader_source,const char* effectPath=NULL);
	//��ȡshader������
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