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

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "Material.h"

/**
������� ������. ����������� ��� ������� 3�-�������. ��������� ��������� � ���������������. ������������� ������ � ���������: �������� �������, �������� uniform-����������
*/
class RoadMaterial : public Material {
public:
	RoadMaterial();

	void initialize() override;
	void applyCommonUniforms() override;
	void applyModelSpecificUniforms() override;

	void setTime( float time ) { _time = time; }

	void setModelMatrix( glm::mat4 mat ) { _modelMatrix = mat; }
	void setViewMatrix( glm::mat4 mat ) { _viewMatrix = mat; }
	void setProjectionMatrix( glm::mat4 mat ) { _projMatrix = mat; }

	void setLightPos( glm::vec4 vec ) { _lightPos = vec; }
	void setAmbientColor( glm::vec3 vec ) { _ambientColor = vec; }
	void setDiffuseColor( glm::vec3 vec ) { _diffuseColor = vec; }
	void setSpecularColor( glm::vec3 vec ) { _specularColor = vec; }

	void setShininess( float s ) { _shininess = s; }

	void setDiffuseTexUnit( int unit ) { _diffuseTexUnit = unit; }

protected:
	//====== �������������� uniform-���������� ======
	GLuint _timeUniform;
	GLuint _modelMatrixUniform;
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;
	GLuint _normalToCameraMatrixUniform;

	GLuint _lightPosUniform;
	GLuint _ambientColorUniform;
	GLuint _diffuseColorUniform;
	GLuint _specularColorUniform;

	GLuint _shininessUniform;

	GLuint _diffuseTexUniform;

	//====== ����������, ������� �������� ��������, ������� ����� �������� � uniform-���������� �������� ======
	float _time;

	//�������
	glm::mat4 _modelMatrix;
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;

	//��������� ���������
	glm::vec4 _lightPos; //in world space
	glm::vec3 _ambientColor;
	glm::vec3 _diffuseColor;
	glm::vec3 _specularColor;

	//��������� ����������
	float _shininess;

	//���������� �����
	int _diffuseTexUnit;
};
