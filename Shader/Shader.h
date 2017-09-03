#pragma once

#include <glad\glad.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class Shader
{
public:
	GLuint program;

	Shader(const std::string &vPath, const std::string &fPath);
	void use() const;
	void setUniformBool(const std::string &name, bool value) const;
	void setUniformInt(const std::string &name, int value) const;
	void setUniformFloat(const std::string &name, float value) const;

private:
	void checkErro(GLuint shader, const std::string &type) const;
};

Shader::Shader(const std::string &vPath, const std::string &fPath)
{
	//read the code from shader file
	std::ifstream vertFile, fragFile;
	std::string vertCode, fragCode;
	vertFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	fragFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	try
	{
		vertFile.open(vPath);
		fragFile.open(fPath);
		std::stringstream vStream, fStream;
		vStream << vertFile.rdbuf();
		fStream << fragFile.rdbuf();
		vertFile.close();
		fragFile.close();
		vertCode = vStream.str();
		fragCode = fStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "ERRO::SHADER::FILE_NOT_SUCCESSFULLY_READ";
		std::abort();
	}
	const char *vc = vertCode.c_str();
	const char *fc = fragCode.c_str();
	GLint success = 0;
	char infoLog[512] = {};
	//compile vertex shader
	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &vc, nullptr);
	glCompileShader(vshader);
	checkErro(vshader, "VERTEX");

	//compile fragment shader
	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &fc, nullptr);
	glCompileShader(fshader);
	checkErro(fshader, "FRAGMENT");

	//create shader program and link the shaders
	program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);
	checkErro(program, "PROGRAM");

	glDeleteShader(vshader);
	glDeleteShader(fshader);
}

void Shader::use() const
{
	glUseProgram(program);
}

void Shader::setUniformBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::setUniformInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setUniformFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::checkErro(GLuint shader, const std::string &type) const
{
	GLint success = 0;
	char infoLog[512] = {};

	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			std::cerr << "ERRO::SHADER::" << type << "::COMPILATION_FAILD\n" << infoLog;
			std::abort();
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 512, nullptr, infoLog);
			std::cerr << "ERRO::PROGRAM::LINKED_FAILD" << infoLog;
			std::abort();
		}
	}
}