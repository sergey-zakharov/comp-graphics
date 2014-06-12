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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include "glimg/glimg.h"

#include "mapWorker.h"
#include "BuildingMaterial.h"
#include "RoadMaterial.h"
#include "GroundMaterial.h"

class Application
{
public:
	Application();
	~Application();

	//������������� ������������ ���������
	void initContext();

	//��������� ��������� ���������� OpenGL
	void initGL();

	//�������� ���������� �����
	void makeScene();

	//���� ����������
	void run();

	//���������� ���� ����
	void draw();

	//����������
	void update();

	//������� ��� ���������� ���������� ������
	void rotateLeft(bool b) { _rotateLeft = b; }
	void rotateRight(bool b) { _rotateRight = b; }

	void rotateUp(bool b) { _rotateUp = b; }
	void rotateDown(bool b) { _rotateDown = b; }

	void zoomIn(bool b) { _zoomIn = b; }
	void zoomOut(bool b) { _zoomOut = b; }

	void moveLeft(bool b) { _moveLeft = b; }
	void moveRight(bool b) { _moveRight = b; }
protected:

	BuildingMaterial _buildingMaterial;
	RoadMaterial _roadMaterial;
	GroundMaterial _groundMaterial;

	void makeSurface();
	void makeShaders();
	void makeBuildings();
	void makeRoads();
	void initData();

	GLFWwindow* _window;

	GLuint _shaderProgram;

	//�������������� uniform-����������
	GLuint _modelMatrixUniform;
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;
	GLuint _isWireframeUniform;
	GLuint _isBuildingUniform;
	GLuint _materialUniform;
	GLuint _lightDirUniform;
	GLuint _lightPosUniform;
	GLuint _ambientColorUniform;
	GLuint _diffuseColorUniform;
	GLuint _specularColorUniform;
	GLuint _shininessUniform;
	GLuint _attenuationUniform;
	GLuint _normalToCameraMatrixUniform;
	GLuint _diffuseTexUniform;
	GLuint _specularTexUniform;

	//�������������� ���������� ��������
	GLuint _buildingTexId;
	GLuint _roadTexId;

	GLuint _sampler;
	GLuint _repeatSampler;


	//����������, ������� �������� ��������, ������� ����� �������� � uniform-���������� ��������
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;
	glm::mat3 _normalToCameraMatrix;
	glm::vec4 _lightDir; //in world space
	glm::vec4 _lightPos; //in world space
	glm::vec3 _ambientColor;
	glm::vec3 _diffuseColor;
	glm::vec3 _specularColor;
	float _attenuation;

	//��������� ����������
	float _shininess1;
	glm::vec3 _material1;

	float _shininess2;
	glm::vec3 _material2;

	GLuint _surfaceVao;
	glm::mat4 _surfaceModelMatrix;
	int _surfaceNumTris;

	//GLuint _buildingVao;
	//glm::mat4 _buildingModelMatrix;
	//glm::vec3 _buildingMaterial;
	//int _buildingNumTris;

	//GLuint _roadVao;
	//glm::mat4 _roadModelMatrix;
	//glm::vec3 _roadMaterial;
	//int _roadNumTris;


	glm::vec3 _surfaceMaterial;

	double _oldTime;

	//��������� ����������� ������
	bool _rotateLeft;
	bool _rotateRight;
	double _phiAng;

	bool _rotateUp;
	bool _rotateDown;
	double _thetaAng;

	bool _zoomIn;
	bool _zoomOut;
	float _distance;
	float _offset;

	bool _moveLeft;
	bool _moveRight;

	MapWorker* worker;

	//������ ����� ������� �� ����� � ������� ������
	GLuint createShader(GLenum shaderType, std::string filename);

	void makeSceneImplementation();
	void drawImplementation();
	
	GLuint loadTexture( std::string filename );
	GLuint loadTextureWithMipmaps( std::string filename );
	GLuint makeCustomTexture();

	GLuint makeShader( GLenum shaderType, std::string filename ); //������ ����� ������� �� ����� � ������� ������
	GLuint makeShaderProgram( std::string vertFilename, std::string fragFilename ); //������� ��������� � ����������� ������� � ������� ��

	void drawBuidlingsOnScene( Camera& camera );
	void drawRoadsOnScene( Camera& camera );

	Mesh _buildings;
	Mesh _roads;
	Mesh _ground;

};