/************************************************************************/
/* 本文件主要是opengl进行视频渲染,依赖于glad,glfw3和glm  
*  author:lihaiping1603@aliyun.com
*  action:create  2019/01/18
*/
/************************************************************************/

#include "render.h"
#include <exception>
#include "shader.h"
#include <assert.h>
#include <iostream>


//这里在一个gl中标准化的设备坐标中定义需要绘制的画布的顶点坐标值
//2d
static const float vertex_position[12] = {
	-1.0f, -1.0f,
	1.0f, -1.0f, 
	-1.0f, 1.0f, 
	-1.0f, 1.0f, 
	1.0f, -1.0f, 
	1.0f, 1.0f
};
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

static const GLchar *v_shader_template_default =
	"#version 330 core\n"
	"attribute vec2 position;\n"
	"varying vec2 _uv;\n"
	"void main(void) {\n"
	"  gl_Position = vec4(position, 0, 1);\n"
	"  vec2 uv = position * 0.5 + 0.5;\n"
	"  _uv = vec2(uv.x, 1.0 - uv.y);\n"
	"}\n";

static const GLchar *f_shader_template_default =
	"#version 330 core\n"
	"varying vec2 _uv;\n"
	"uniform sampler2D from;\n"
	"uniform float progress;\n"
	"uniform float ratio;\n"
	"uniform float _fromR;\n"
	"\n"
	"vec4 getFromColor(vec2 uv) {\n"
	"  return texture2D(from, vec2(uv.x, uv.y));\n"
	"}\n"
	"\n"
	"\n%s\n"
	"void main() {\n"
	"  gl_FragColor = effect(_uv);\n"
	"}\n";


static const GLchar *f_default_effect_source =
	"vec4 effect (vec2 uv) {\n"
	"  return getFromColor(uv);\n"
	"}\n";

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


CRender::CRender()
	:m_pWindow(NULL)
	,m_pShaderCtx(NULL)
	,m_uVBO(0)
	,m_uVAO(0)
	,m_uMainTexture(0)
	,m_uWindowWidth(0)
	,m_uWindowHeight(0)
	,m_uVideoWidth(0)
	,m_uVideoHeight(0)
	,m_uProgressLoc(0)
	,m_uRatioLoc(0)
	,m_uFromRLoc(0)
{

}

CRender::~CRender()
{
	if (m_pWindow) {
		glDeleteTextures(1, &m_uMainTexture);
		glDeleteVertexArrays(1,&m_uVAO);
		glDeleteBuffers(1, &m_uVBO);
		if (m_pShaderCtx)
		{
			delete m_pShaderCtx;
			m_pShaderCtx=NULL;
		}
		glfwDestroyWindow(m_pWindow);
	}
}


int32_t CRender::CreateEnvGL( int window_width,int window_height )
{
	try
	{
		// glfw: initialize and configure
		// ------------------------------
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

		// glfw window creation
		// --------------------
		GLFWwindow* window = glfwCreateWindow(window_width, window_height, "GL_Effect", NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		//glfwSetCursorPosCallback(window, mouse_callback);
		//glfwSetScrollCallback(window, scroll_callback);

		// tell GLFW to capture our mouse
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -1;
		}

		// configure global opengl state
		// -----------------------------
		//glEnable(GL_DEPTH_TEST);
		m_pWindow=window;
		m_uWindowWidth=window_width;
		m_uWindowHeight=window_height;
	}
	catch (std::exception* e)
	{
		throw e;
	}
	return 0;
}

int32_t CRender::CreateShader( const char* v_shader_source,const char* f_shader_source )
{
	if (v_shader_source==NULL||f_shader_source==NULL)
	{
		std::cout<<"vertex source or fragment source is null"<<std::endl;
		return -1;
	}
	GLuint v_shader, f_shader;
	Shader *pShaderCtx=new Shader();
	assert(pShaderCtx);
	std::cout<<"vertex shader source:\n"<<v_shader_source<<std::endl;
	if (!(v_shader=pShaderCtx->BuildShader(v_shader_source,GL_VERTEX_SHADER)))
	{
		return -1;
	}
	std::cout<<"fragment shader source:\n"<<f_shader_source<<std::endl;
	if (!(f_shader=pShaderCtx->BuildShader(f_shader_source,GL_FRAGMENT_SHADER)))
	{
		return -1;
	}

	if (!pShaderCtx->BuildProgram(v_shader,f_shader))
	{
		return -1;
	}
	pShaderCtx->use();

	m_pShaderCtx=pShaderCtx;
	return 0;
}

void CRender::CreateVAO(const float vertexPostion[],int32_t arrySize)
{
	glGenVertexArrays(1, &m_uVAO);
	glGenBuffers(1, &m_uVBO);

	glBindVertexArray(m_uVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uVBO);
	if (arrySize==0)
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_position), vertex_position, GL_STATIC_DRAW);
	}
	else
		glBufferData(GL_ARRAY_BUFFER, arrySize, vertexPostion, GL_STATIC_DRAW);
	
	//设置顶点属性指针
	GLint loc = glGetAttribLocation(m_pShaderCtx->GetProgramId(), "position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

uint32_t CRender::CreateTexture(uint32_t width,uint32_t height,PixFormat_E ePixFmt/*=PF_RGB24*/)
{
	{ // main
		glGenTextures(1, &m_uMainTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_uMainTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		if (PF_RGB32==ePixFmt||PF_RGBA==ePixFmt)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}else{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, PIXEL_FORMAT, GL_UNSIGNED_BYTE, NULL);
		}

		glUniform1i(glGetUniformLocation(m_pShaderCtx->GetProgramId(), "from"), 0);
	}
	m_uVideoWidth=width;
	m_uVideoHeight=height;
	return m_uMainTexture;
}

void CRender::InitUniforms()
{
	uint32_t programId=m_pShaderCtx->GetProgramId();
	m_uProgressLoc=glGetUniformLocation(programId, "progress");
	glUniform1f(m_uProgressLoc, 0.0f);

	// TODO: this should be output ratio
	m_uRatioLoc=glGetUniformLocation(programId, "ratio");
	glUniform1f(m_uRatioLoc, (float)m_uVideoWidth / m_uVideoHeight);

	m_uFromRLoc=glGetUniformLocation(programId, "_fromR");
	glUniform1f(m_uFromRLoc, (float)m_uVideoWidth / m_uVideoHeight);
}

void CRender::Render(MediaFrameInfo_S *mainFrame,bool update /*=true*/)
{
	glfwMakeContextCurrent(m_pWindow);
	uint32_t programId=m_pShaderCtx->GetProgramId();
	glUseProgram(programId);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uMainTexture);
	if (update)
	{
		if (mainFrame&&(PF_RGB32==mainFrame->ePixFmt||PF_RGBA==mainFrame->ePixFmt))
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mainFrame->nWidth, mainFrame->nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mainFrame->pFrame);
		}else{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mainFrame->nWidth, mainFrame->nHeight, 0, PIXEL_FORMAT, GL_UNSIGNED_BYTE, mainFrame->pFrame);
		}
	}

	//渲染
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glfwSwapBuffers(m_pWindow);
}


int32_t CRender::SetupGL( uint32_t window_width,uint32_t window_height,uint32_t video_width,uint32_t video_height,const char* effectPath )
{
	if (CreateEnvGL(window_width,window_height)<0)
	{
		return -1;
	}
	ConfigGlobalState();
	if (LoadShader(v_shader_template_default,f_shader_template_default,effectPath)<0)
	{
		return -1;
	}

	CreateVAO();
	InitUniforms();
	//CreateTexture(video_width,video_width);
	return 0;
}

int32_t CRender::LoadShader(const char* v_shader_source,const char* f_shader_source,const char* effectPath )
{
	std::string strFragmentSource(f_shader_source);
	std::string strEffectSource;
	if (effectPath)
	{
		strEffectSource=m_pShaderCtx->ReadShaderFile(effectPath);
	}
	if (strEffectSource.empty())
	{
		strEffectSource.append(f_default_effect_source);
	}
	//strFragmentSource.append(strTransSource);
	strFragmentSource.replace(strFragmentSource.find("%s"),2,strEffectSource.c_str(),strEffectSource.length());

	//创建
	return CreateShader(v_shader_source,strFragmentSource.c_str());
}

Shader* CRender::GetShader()
{
	return m_pShaderCtx;
}

float CRender::GetTime()
{
	float currentTime = glfwGetTime();
	return currentTime;
}

void CRender::ConfigGlobalState()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}




