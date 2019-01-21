/************************************************************************/
/* 本文件主要是opengl进行glsl编程的一些辅助函数  
*  author:lihaiping1603@aliyun.com
*  action:create  2019/01/18
*/
/************************************************************************/

#ifndef __SHADER_H_
#define __SHADER_H_
#include <glm/glm.hpp>

#include <string>
#include "stdint.h"

class Shader{
public:
	~Shader();
	//读取shader path的内容，并以string返回内容
	std::string ReadShaderFile(const char* shaderPath);
	//编译一个shader,如果成功返回shader_id
	uint32_t BuildShader(const char* shaderSource,uint32_t shaderType);
	//连接一个shader
	bool BuildProgram( uint32_t vertexId, uint32_t fragmentId,uint32_t geometryId=0);
	// activate the shader
	// ------------------------------------------------------------------------
	void use(); 
	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool value) const;
	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int value) const;
	// ------------------------------------------------------------------------
	void setFloat(const std::string &name, float value) const;
	// ------------------------------------------------------------------------
	void setVec2(const std::string &name, const glm::vec2 &value) const;
	void setVec2(const std::string &name, float x, float y) const;
	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, const glm::vec3 &value) const;
	void setVec3(const std::string &name, float x, float y, float z) const;
	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, const glm::vec4 &value) const;
	void setVec4(const std::string &name, float x, float y, float z, float w) ;
	// ------------------------------------------------------------------------
	void setMat2(const std::string &name, const glm::mat2 &mat) const;
	// ------------------------------------------------------------------------
	void setMat3(const std::string &name, const glm::mat3 &mat) const;
	// ------------------------------------------------------------------------
	void setMat4(const std::string &name, const glm::mat4 &mat) const;

	uint32_t GetProgramId();
private:
	//检查
	int32_t CheckErrors(uint32_t shader, std::string type);
	uint32_t m_uShader;
};

#endif
