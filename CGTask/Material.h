#pragma once

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
������� ����� ��� ��������. �������� ��� �������� ��������.
*/
class Material {
public:
	Material();

	//������������� �������
	virtual void initialize() = 0;

	//�������� �� ���������� �������� uniform-����������, ������� �������� ������ ��� ���� 3�-�������
	virtual void applyCommonUniforms() = 0;

	//�������� �� ���������� �������� uniform-����������, ������� ����������� ����� 3�-��������
	virtual void applyModelSpecificUniforms() = 0;

	GLuint getProgramId() { return _programId; }

protected:
	GLuint makeShader( GLenum shaderType, std::string filename ); //������ ����� ������� �� ����� � ������� ������
	GLuint makeShaderProgram( std::string vertFilename, std::string fragFilename );

	GLuint _programId;
};