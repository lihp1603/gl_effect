/************************************************************************/
/* 本文件主要是opengl进行视频转场效果过渡的渲染,依赖于glad,glfw3和glm  
*  author:lihaiping1603@aliyun.com
*  action:create  2019/01/22
*/
/************************************************************************/

#include "transitionRender.h"


//厉害的方法，一般我们做纹理贴图的时候，会将纹理贴图的坐标像顶点坐标一样，从cpu传入到gpu
//大家都知道,这种传输是效率比较低的方法
//这里,作者用了一种从顶点坐标转化的方法来实现纹理贴图坐标，从而提升效率
// vec2 uv = position * 0.5 + 0.5;这个地方是把顶点坐标从[-1,1]转化为[0,1]
//这里将[-1,1]转化为[0,1]主要是为了和后面的纹理贴图坐标,减少一次纹理贴图坐标的传输
//厉害

//_uv = vec2(uv.x, 1.0 - uv.y);
//相当于_uv.x=uv.x;_uv.y=1-uv.y
//将这个_uv代入到后面的函数getFromColor和getToColor函数中再进行一次计算
//变化为texture2D(form,_uv.x,1.0-(_uv.y))代入化简texture2D(form,_uv.x,_uv.y);
//这个是因为在ffmpeg里面做滤镜需要这么计算，但实际如果是直接拿上面这个来显示的话，会发现纹理贴图出现y轴翻转的情况
//所以在播放器的是，我们只需要取一次1.0-y就可以解决翻转的问题
//参考https://blog.csdn.net/narutojzm1/article/details/51940817

static const GLchar *v_shader_template_transition =
	"#version 330 core\n"
	"attribute vec2 position;\n"
	"varying vec2 _uv;\n"
	"void main(void) {\n"
	"  gl_Position = vec4(position, 0, 1);\n"
	"  vec2 uv = position * 0.5 + 0.5;\n"
	"  _uv = vec2(uv.x, 1.0 - uv.y);\n"
	"}\n";

static const GLchar *f_shader_template_transition =
	"#version 330 core\n"
	"varying vec2 _uv;\n"
	"uniform sampler2D from;\n"
	"uniform sampler2D to;\n"
	"uniform float progress;\n"
	"uniform float ratio;\n"
	"uniform float _fromR;\n"
	"uniform float _toR;\n"
	"\n"
	"vec4 getFromColor(vec2 uv) {\n"
	"  return texture2D(from, vec2(uv.x, uv.y));\n"
	"}\n"
	"\n"
	"vec4 getToColor(vec2 uv) {\n"
	"  return texture2D(to, vec2(uv.x, uv.y));\n"
	"}\n"
	"\n"
	"\n%s\n"
	"void main() {\n"
	"  gl_FragColor = transition(_uv);\n"
	"}\n";


//GLSL中mix函数genType mix (genType x, genType y, genType a)是线性插值的实现方法,
//它返回线性混合的x和y，如：x*(1-a)+y*a
// default to a basic fade effect
static const GLchar *f_default_transition_source =
	"vec4 transition (vec2 uv) {\n"
	"  return mix(\n"
	"    getFromColor(uv),\n"
	"    getToColor(uv),\n"
	"    progress\n"
	"  );\n"
	"}\n";

//创建一个用于渲染转场效果的类
int32_t CRenderTransition::CreateTransShader( const char* transPath )
{
	std::string strFragmentSource(f_shader_template_transition);
	std::string strTransSource;
	if (transPath)
	{
		strTransSource=GetShader()->ReadShaderFile(transPath);
	}
	if (strTransSource.empty())
	{
		strTransSource.append(f_default_transition_source);
	}
	//strFragmentSource.append(strTransSource);
	strFragmentSource.replace(strFragmentSource.find("%s"),2,strTransSource.c_str(),strTransSource.length());
	
	//创建
	return CreateShader(v_shader_template_transition,strFragmentSource.c_str());;
}

void CRenderTransition::CreateTransTexture( uint32_t width,uint32_t height )
{
	{ // from
		glGenTextures(1, &m_uFromTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_uFromTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, PIXEL_FORMAT, GL_UNSIGNED_BYTE, NULL);

		glUniform1i(glGetUniformLocation(GetShader()->GetProgramId(), "from"), 0);
	}

	{ // to
		glGenTextures(1, &m_uToTexture);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, m_uToTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, PIXEL_FORMAT, GL_UNSIGNED_BYTE, NULL);

		glUniform1i(glGetUniformLocation(GetShader()->GetProgramId(), "to"), 1);
	}
	m_uVideoWidth=width;
	m_uVideoHeight=height;
}

void CRenderTransition::InitUniforms()
{
	uint32_t programId=GetShader()->GetProgramId();
	m_uProgressLoc=glGetUniformLocation(programId, "progress");
	glUniform1f(m_uProgressLoc, 0.0f);

	// TODO: this should be output ratio
	m_uRatioLoc=glGetUniformLocation(programId, "ratio");
	glUniform1f(m_uRatioLoc, (float)m_uVideoWidth / m_uVideoHeight);

	m_uFromRLoc=glGetUniformLocation(programId, "_fromR");
	glUniform1f(m_uFromRLoc, (float)m_uVideoWidth / m_uVideoHeight);

	// TODO: initialize this in config_props for "to" input
	m_uToRLoc=glGetUniformLocation(programId, "_toR");
	glUniform1f(m_uToRLoc, (float)m_uVideoWidth / m_uVideoHeight);
}

void CRenderTransition::Render( MediaFrameInfo_S *fromFrame,const MediaFrameInfo_S *toFrame ,float fProgress)
{
	glfwMakeContextCurrent(m_pWindow);
	uint32_t programId=m_pShaderCtx->GetProgramId();
	glUseProgram(programId);
	
	glUniform1f(m_uProgressLoc, fProgress);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uFromTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fromFrame->nWidth, fromFrame->nHeight, 0, PIXEL_FORMAT, GL_UNSIGNED_BYTE, fromFrame->pFrame);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, m_uToTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, toFrame->nWidth, toFrame->nHeight, 0, PIXEL_FORMAT, GL_UNSIGNED_BYTE, toFrame->pFrame);
	//渲染
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glfwSwapBuffers(m_pWindow);
}


int32_t CRenderTransition::SetupGL( uint32_t window_width,uint32_t window_height,uint32_t video_width,uint32_t video_height,const char* shaderPath )
{
	if (CreateEnvGL(window_width,window_height)<0)
	{
		return -1;
	}

	if (LoadShader(v_shader_template_transition,f_shader_template_transition,shaderPath)<0)
	{
		return -1;
	}

	CreateVAO();
	InitUniforms();
	//创建纹理
	CreateTransTexture(video_width,video_width);
	return 0;
}

CRenderTransition::CRenderTransition()
	:CRender()
	,m_uFromTexture(0)
	,m_uToTexture(0)
{

}

CRenderTransition::~CRenderTransition()
{
	
}
