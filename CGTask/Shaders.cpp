#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"

GLuint Application::makeShader(GLenum shaderType, std::string filename)
{
	//Читаем текст шейдера из файла
	std::ifstream vertFile(filename.c_str());
	if (vertFile.fail())
	{
		std::cerr << "Failed to load shader file " << filename << std::endl;
		exit(1);
	}
	std::string vertFileContent((std::istreambuf_iterator<char>(vertFile)), (std::istreambuf_iterator<char>()));
	vertFile.close();

	const char* vertexShaderText = vertFileContent.c_str();

	//Создаем шейдерный объект в OpenGL
	GLuint vs = glCreateShader(shaderType);
	glShaderSource(vs, 1, &vertexShaderText, NULL);
	glCompileShader(vs);

	//Проверяем ошибки компиляции
	int status = -1;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cerr << "Failed to compile the shader:\n";		
		
		GLint errorLength;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &errorLength);

		GLchar* log = new GLchar[errorLength];
		glGetShaderInfoLog(vs, errorLength, 0, log);

		std::cerr << log << std::endl;

		delete[] log;
		exit(1);
	}

	return vs;
}

GLuint Application::makeShaderProgram(std::string vertFilename, std::string fragFilename)
{
	GLuint vs = makeShader(GL_VERTEX_SHADER, vertFilename);
	GLuint fs = makeShader(GL_FRAGMENT_SHADER, fragFilename);

	GLuint programId = glCreateProgram();
	glAttachShader(programId, fs);
	glAttachShader(programId, vs);
	glLinkProgram(programId);

	//Проверяем ошибки линковки
	int status = -1;
	glGetProgramiv(programId, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cerr << "Failed to link the program:\n";		
		
		GLint errorLength;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &errorLength);

		GLchar* log = new GLchar[errorLength];
		glGetProgramInfoLog(programId, errorLength, 0, log);

		std::cerr << log << std::endl;

		delete[] log;
		exit(1);
	}

	return programId;
}