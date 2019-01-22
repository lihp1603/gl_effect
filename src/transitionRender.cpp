/************************************************************************/
/* ���ļ���Ҫ��opengl������Ƶת��Ч�����ɵ���Ⱦ,������glad,glfw3��glm  
*  author:lihaiping1603@aliyun.com
*  action:create  2019/01/22
*/
/************************************************************************/

#include "transitionRender.h"


//�����ķ�����һ��������������ͼ��ʱ�򣬻Ὣ������ͼ�������񶥵�����һ������cpu���뵽gpu
//��Ҷ�֪��,���ִ�����Ч�ʱȽϵ͵ķ���
//����,��������һ�ִӶ�������ת���ķ�����ʵ��������ͼ���꣬�Ӷ�����Ч��
// vec2 uv = position * 0.5 + 0.5;����ط��ǰѶ��������[-1,1]ת��Ϊ[0,1]
//���ｫ[-1,1]ת��Ϊ[0,1]��Ҫ��Ϊ�˺ͺ����������ͼ����,����һ��������ͼ����Ĵ���
//����

//_uv = vec2(uv.x, 1.0 - uv.y);
//�൱��_uv.x=uv.x;_uv.y=1-uv.y
//�����_uv���뵽����ĺ���getFromColor��getToColor�������ٽ���һ�μ���
//�仯Ϊtexture2D(form,_uv.x,1.0-(_uv.y))���뻯��texture2D(form,_uv.x,_uv.y);
//�������Ϊ��ffmpeg�������˾���Ҫ��ô���㣬��ʵ�������ֱ���������������ʾ�Ļ����ᷢ��������ͼ����y�ᷭת�����
//�����ڲ��������ǣ�����ֻ��Ҫȡһ��1.0-y�Ϳ��Խ����ת������
//�ο�https://blog.csdn.net/narutojzm1/article/details/51940817

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


//GLSL��mix����genType mix (genType x, genType y, genType a)�����Բ�ֵ��ʵ�ַ���,
//���������Ի�ϵ�x��y���磺x*(1-a)+y*a
// default to a basic fade effect
static const GLchar *f_default_transition_source =
	"vec4 transition (vec2 uv) {\n"
	"  return mix(\n"
	"    getFromColor(uv),\n"
	"    getToColor(uv),\n"
	"    progress\n"
	"  );\n"
	"}\n";

//����һ��������Ⱦת��Ч������
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
	
	//����
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
	//��Ⱦ
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
	//��������
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
