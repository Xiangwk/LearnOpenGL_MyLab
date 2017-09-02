#pragma once

#include <glad\glad.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class Shader
{
public:
	Shader(const std::string &vPath, const std::string &fPath);
	void use() const;

private:
	GLuint program;
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
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vshader, 512, nullptr, infoLog);
		std::cerr << "ERRO::SHADER::VERTEX::COMPILATION_FAILD" << infoLog;
		std::abort();
	}

	//compile fragment shader
	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &fc, nullptr);
	glCompileShader(fshader);
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fshader, 512, nullptr, infoLog);
		std::cerr << "ERRO::SHADER::FRAGMENT::COMPILATION_FAILD" << infoLog;
		std::abort();
	}

	//create shader program and link the shaders
	program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, nullptr, infoLog);
		std::cerr << "ERRO::PROGRAM::LINKED_FAILD" << infoLog;
		std::abort();
	}

	glDeleteShader(vshader);
	glDeleteShader(fshader);
}

void Shader::use() const
{
	glUseProgram(program);
}