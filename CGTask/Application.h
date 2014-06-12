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

	//Инициализация графического контекста
	void initContext();

	//Настройка некоторых параметров OpenGL
	void initGL();

	//Создание трехмерной сцены
	void makeScene();

	//Цикл рендеринга
	void run();

	//Отрисовать один кадр
	void draw();

	//Обновление
	void update();

	//Функции для управления положением камеры
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

	//идентификаторы uniform-переменных
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

	//идентификаторы текстурных объектов
	GLuint _buildingTexId;
	GLuint _roadTexId;

	GLuint _sampler;
	GLuint _repeatSampler;


	//переменные, которые содержат значения, которые будут записаны в uniform-переменные шейдеров
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;
	glm::mat3 _normalToCameraMatrix;
	glm::vec4 _lightDir; //in world space
	glm::vec4 _lightPos; //in world space
	glm::vec3 _ambientColor;
	glm::vec3 _diffuseColor;
	glm::vec3 _specularColor;
	float _attenuation;

	//параметры материалов
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

	//Состояние виртуальной камеры
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

	//Читает текст шейдера из файла и создает объект
	GLuint createShader(GLenum shaderType, std::string filename);

	void makeSceneImplementation();
	void drawImplementation();
	
	GLuint loadTexture( std::string filename );
	GLuint loadTextureWithMipmaps( std::string filename );
	GLuint makeCustomTexture();

	GLuint makeShader( GLenum shaderType, std::string filename ); //Читает текст шейдера из файла и создает объект
	GLuint makeShaderProgram( std::string vertFilename, std::string fragFilename ); //создает вершинный и фрагментный шейдеры и линкует их

	void drawBuidlingsOnScene( Camera& camera );
	void drawRoadsOnScene( Camera& camera );

	Mesh _buildings;
	Mesh _roads;
	Mesh _ground;

};