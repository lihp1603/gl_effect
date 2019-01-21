/************************************************************************/
/* 本文件主要是opengl进行glsl编程的一些辅助函数  
*  author:lihaiping1603@aliyun.com
*  action:create  2019/01/18
*/
/************************************************************************/

#include "shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


Shader::~Shader()
{
	glDeleteProgram(m_uShader);
}

std::string Shader::ReadShaderFile( const char* shaderPath )
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string shaderCode;
	std::ifstream shaderFile;
	if (shaderPath==NULL)
	{
		return shaderCode;
	}
	// ensure ifstream objects can throw exceptions:
	shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	try{
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;
		// read file's buffer contents into streams
		shaderStream << shaderFile.rdbuf();
		// close file handlers
		shaderFile.close();
		// convert stream into string
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e){
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	return shaderCode;
}


uint32_t Shader::BuildShader( const char* shaderSource,uint32_t shaderType )
{
	uint32_t shaderId=0;
	if (shaderSource==NULL)
	{
		return 0;
	}
	// 2. compile shaders
	// vertex shader
	shaderId = glCreateShader(shaderType);
	if (!shaderId||!glIsShader(shaderId))
	{
		return 0;
	}
	glShaderSource(shaderId, 1, &shaderSource, NULL);
	glCompileShader(shaderId);

	if (CheckErrors(shaderId, "COMPILE")<0)
	{
		glDeleteShader(shaderId);
		return 0;
	}
	return shaderId;
}


bool Shader::BuildProgram( uint32_t vertexId, uint32_t fragmentId,uint32_t geometryId/*=0*/ )
{
	// shader Program
	uint32_t programID = glCreateProgram();
	glAttachShader(programID, vertexId);
	glAttachShader(programID, fragmentId);
	if(geometryId != 0)
		glAttachShader(programID, geometryId);
	glLinkProgram(programID);
	if (CheckErrors(programID, "PROGRAM")<0)
	{
		return false;
	}
	
	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertexId);
	glDeleteShader(fragmentId);
	if(geometryId != 0)
		glDeleteShader(geometryId);
	//记录下来
	m_uShader=programID;
	return true;
}

// activate the shader
// ------------------------------------------------------------------------
void Shader::use() 
{ 
	glUseProgram(m_uShader); 
}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) const
{         
	glUniform1i(glGetUniformLocation(m_uShader, name.c_str()), (int)value); 
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const
{ 
	glUniform1i(glGetUniformLocation(m_uShader, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const
{ 
	glUniform1f(glGetUniformLocation(m_uShader, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{ 
	glUniform2fv(glGetUniformLocation(m_uShader, name.c_str()), 1, &value[0]); 
}
void Shader::setVec2(const std::string &name, float x, float y) const
{ 
	glUniform2f(glGetUniformLocation(m_uShader, name.c_str()), x, y); 
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{ 
	glUniform3fv(glGetUniformLocation(m_uShader, name.c_str()), 1, &value[0]); 
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{ 
	glUniform3f(glGetUniformLocation(m_uShader, name.c_str()), x, y, z); 
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{ 
	glUniform4fv(glGetUniformLocation(m_uShader, name.c_str()), 1, &value[0]); 
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) 
{ 
	glUniform4f(glGetUniformLocation(m_uShader, name.c_str()), x, y, z, w); 
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(m_uShader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_uShader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_uShader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


// utility function for checking shader compilation/linking errors.
// ------------------------------------------------------------------------
int32_t Shader::CheckErrors(uint32_t shader, std::string type)
{
	GLint success=0;
	GLchar infoLog[1024]={0};
	if(type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			return -1;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			return -1;
		}
	}
	return 0;
}

uint32_t Shader::GetProgramId()
{
	return m_uShader;
}

