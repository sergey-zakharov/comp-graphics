#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>


#include "Application.h"
#include "Camera.h"
#include "Mesh.h"

//#include "geometry.h"
//#include "parse.h"

const unsigned HeightSurfaceResolution = 25;
const unsigned LengthSurfaceResolution = 25;
const unsigned WidthSurfaceResolution = 25;
const float A = -5;
const float B = -5;
const float C = 0;

int version = 2; // 1 - map_ny_cooper_triangle with houses only, 2 - house_in_ny, 3 - two houses and road, 4 single road, 5 downtown, 6 downtown small


const char* file = "../house_in_ny.osm"; //house_in_ny , map_ny_cooper_triangle





//====================================== Вспомогательные функции



//======================================

//Функция обратного вызова для обработки нажатий на клавиатуре
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);


	if( action == GLFW_PRESS) {
		switch( key ) {
			case GLFW_KEY_ESCAPE: 
				glfwSetWindowShouldClose(window, GL_TRUE); 
				break;
			case GLFW_KEY_A:
				app->rotateLeft(true);
				break;
			case GLFW_KEY_D:
				app->rotateRight(true);
				break;
			case GLFW_KEY_W:
				app->rotateUp(true);	
				break;
			case GLFW_KEY_S:
				app->rotateDown(true);
				break;
			case GLFW_KEY_E:
				app->zoomIn(true);
				break;
			case GLFW_KEY_Q:
				app->zoomOut(true);
				break;
			case GLFW_KEY_Z:
				app->moveLeft(true);
				break;
			case GLFW_KEY_C:
				app->moveRight(true);
				break;
		}
	} else if( action == GLFW_RELEASE ) {
		switch( key ) {
			case GLFW_KEY_A:
				app->rotateLeft(false);
				break;
			case GLFW_KEY_D:
				app->rotateRight(false);
				break;
			case GLFW_KEY_W:
				app->rotateUp(false);
				break;
			case GLFW_KEY_S:
				app->rotateDown(false);
				break;
			case GLFW_KEY_E:
				app->zoomIn(false);
				break;
			case GLFW_KEY_Q:
				app->zoomOut(false);
				break;
			case GLFW_KEY_Z:
				app->moveLeft(false);
				break;
			case GLFW_KEY_C:
				app->moveRight(false);
				break;
		}
	}
}

//======================================

Application::Application() :
_oldTime(0.0f),
_rotateLeft(false),
_rotateRight(false),
_phiAng(0.0f),
_rotateUp(false),
_rotateDown(false),
_thetaAng(0.0f),
_zoomIn(false),
_zoomOut(false),
_distance(20.f),
_offset(0.f)
{

	if( version == 1 ) {
		file = "../map_ny_cooper_triangle.osm";
	} else if( version == 2 ) {
		file = "../house_in_ny.osm";
	} else if( version == 3 ) {
		file = "../road_in_ny.osm";
	} else if( version == 4 ) {
		file = "../straight_road_only.osm";
	} else if( version == 5 ) {
		file = "../ny_center.osm";
	} else if( version == 6 ) {
		file = "../ny_center_small.osm";
	}


	XMLDocument doc;
	doc.LoadFile(file);
	worker = new MapWorker(doc);
	std::cout << "App loaded" << std::endl;
}

Application::~Application()
{
	glfwTerminate();
}

void Application::initContext()
{
	if( !glfwInit() ) {
		std::cerr << "ERROR: could not start GLFW3\n";
		exit(1);
	}

	_window = glfwCreateWindow(1024, 768, "3D NY Map using OpenStreetMaps", NULL, NULL);
	if( !_window ) {
		std::cerr << "ERROR: could not open window with GLFW3\n";
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(_window);

	glfwSetWindowUserPointer(_window, this); //регистрируем указатель на данный объект, чтобы потом использовать его в функциях обратного вызова
}

void Application::initGL()
{
	glewExperimental = GL_TRUE;
	glewInit();

	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported: " << version << std::endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void Application::makeScene()
{
	makeSceneImplementation();
}

void Application::run()
{
	glfwSetKeyCallback(_window, keyCallback);

	while( !glfwWindowShouldClose(_window) ) {
		glfwPollEvents();

		update();

		draw();
	}
}

void Application::draw()
{
	int width, height;
	glfwGetFramebufferSize(_window, &width, &height);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_projMatrix = glm::perspective(45.0f, (float)width / height, 0.1f, 1000.f);

	drawImplementation();

	glfwSwapBuffers(_window); 
}

void Application::update()
{
	float dt = glfwGetTime() - _oldTime;
	_oldTime = glfwGetTime();

	float speed = 5.0;
	float zoomSpeed = 40.0;
	float moveSpeed = 25.0;

	if( _rotateLeft ) {
		_phiAng -= speed * dt;
	}
	if( _rotateRight ) {
		_phiAng += speed * dt;
	}
	if( _rotateUp ) {
		_thetaAng += speed * dt;
	}
	if( _rotateDown ) {
		_thetaAng -= speed * dt;
	}
	if( _zoomIn ) {
		_distance -= zoomSpeed * dt;
	}
	if( _zoomOut ) {
		_distance += zoomSpeed * dt;
	}
	if( _moveLeft ) {
		_offset += moveSpeed * dt;
	}
	if( _moveRight ) {
		_offset -= moveSpeed * dt;
	}

	_thetaAng = glm::clamp<float>(_thetaAng, -(float)M_PI * 0.45f, (float)M_PI * 0.45f);
	glm::vec3 pos = glm::vec3(glm::cos(_phiAng) * glm::cos(_thetaAng), glm::sin(_phiAng) * glm::cos(_thetaAng), glm::sin(_thetaAng)) * _distance;

	_viewMatrix = glm::lookAt(pos, glm::vec3(1.f), glm::vec3(0.0f, 0.0f, 1.0f));
}

GLuint Application::createShader(GLenum shaderType, std::string filename)
{
	//Читаем текст шейдера из файла
	std::ifstream vertFile(filename.c_str());
	if( vertFile.fail() ) {
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
	if( status != GL_TRUE ) {
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

void Application::makeSceneImplementation()
{
	_commonMaterial.initialize();
	_skyBoxMaterial.initialize();

	_buildingTexId = loadTexture( "../images/brick.jpg" );
	_roadTexId = loadTexture( "../images/road.jpg" );

	makeSurface();
	makeBuildings();
	makeRoads();
	makeShaders();
	initData();
}

void Application::makeShaders()
{
	std::string vertFilename = "../shaders/CGTask/shader.vert";
	std::string fragFilename = "../shaders/CGTask/shader.frag";

	GLuint vs = createShader(GL_VERTEX_SHADER, vertFilename);
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fragFilename);

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, fs);
	glAttachShader(_shaderProgram, vs);
	glLinkProgram(_shaderProgram);
	
}
void Application::drawImplementation()
{
	glUseProgram(_shaderProgram);

	//Копирование на видеокарту значений uniform-пемеренных, общих для всех объектов
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));

	glUniform4fv( _lightDirUniform, 1, glm::value_ptr( _lightDir ) );
	glUniform4fv( _lightPosUniform, 1, glm::value_ptr( _lightPos ) );
	glUniform3fv( _ambientColorUniform, 1, glm::value_ptr( _ambientColor ) );
	glUniform3fv( _diffuseColorUniform, 1, glm::value_ptr( _diffuseColor ) );
	glUniform3fv( _specularColorUniform, 1, glm::value_ptr( _specularColor ) );
	glUniform1f( _attenuationUniform, _attenuation );


	glUniform1i( _diffuseTexUniform, 0 ); //текстурный юнит 0
	glUniform1i( _specularTexUniform, 1 ); //текстурный юнит 1

	

	//====== Поверхность ======
	//Копирование на видеокарту значений uniform-пемеренных для сферы
	_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _surfaceModelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

	glBindVertexArray(_surfaceVao); //Подключаем VertexArray для поверхности
	glUniform3fv(_materialUniform, 1, glm::value_ptr(_surfaceMaterial));

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_surfaceModelMatrix));

	glBindVertexArray(_surfaceVao); //Подключаем VertexArray для сферы
	//glUniform1i(_isWireframeUniform, false);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, _surfaceNumTris * 3); //Рисуем поверхность


	// активизация текстур
	glActiveTexture( GL_TEXTURE0 + 0 );  //текстурный юнит 0
	glBindTexture( GL_TEXTURE_2D, _buildingTexId );
	glBindSampler( 0, _sampler );

	//====== Здания ======
	//Копирование на видеокарту значений uniform-пемеренных для зданий
	_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _buildingModelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

	glUniform3fv(_materialUniform, 1, glm::value_ptr(_buildingMaterial));
	glUniform1f( _shininessUniform, _shininess1 );

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_buildingModelMatrix));

	glBindVertexArray(_buildingVao); //Подключаем VertexArray для здания
	
	//glUniform1i(_isBuildingUniform, true);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, _buildingNumTris * 3); //Рисуем здание



	// активизация текстур
	glActiveTexture( GL_TEXTURE0 + 0 );  //текстурный юнит 0
	glBindTexture( GL_TEXTURE_2D, _roadTexId );
	glBindSampler( 0, _sampler );

	//====== Дороги ======
	//Копирование на видеокарту значений uniform-пемеренных для дорог
	_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _roadModelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

	glUniform3fv(_materialUniform, 1, glm::value_ptr(_roadMaterial));

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_roadModelMatrix));

	glBindVertexArray(_roadVao); //Подключаем VertexArray для дорог


	

	//glUniform1i(_isBuildingUniform, true);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, _roadNumTris * 3); //Рисуем здание
	/*glUniform1i(_isBuildingUniform, true);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, _surfaceNumTris * 3);*/ //Рисуем поверхность
	
	//glUniform1i(_isBuildingUniform, false);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDrawArrays(GL_TRIANGLES, 0, _surfaceNumTris * 3); //Рисуем поверхность

	//glUniform1i(_isWireframeUniform, true);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glLineWidth(2.0);
	//glDrawArrays(GL_TRIANGLES, 0, _surfaceNumTris * 3); //Рисуем сеточкой
}
void Application::initData()
{

	//Инициализация матриц
	_viewMatrix = glm::lookAt( glm::vec3( 0.0f, -5.0f, 0.0f ), glm::vec3( 0.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
	_projMatrix = glm::perspective( glm::radians( 45.0f ), 4.0f / 3.0f, 0.1f, 10000.f );


	_surfaceMaterial = glm::vec3( 0.0, 1.0, 0.0 );
	_buildingMaterial = glm::vec3( 0.0, 0.0, 1.0 );
	_roadMaterial = glm::vec3( 0.0, 0.0, 1. );

	//Инициализация значений переменных освщения
	_lightDir = glm::vec4( 0.0f, 1.0f, 0.8f, 0.0f );
	_lightPos = glm::vec4( 10.0f, 10.0f, 200.f, .01f );
	_ambientColor = glm::vec3( 0.2, 0.2, 0.2 );
	_diffuseColor = glm::vec3( 0.8, 0.8, 0.8 );
	_specularColor = glm::vec3( 0.5, 0.5, 0.5 );
	_attenuation = 1.0f;

	//Инициализация материалов
	_shininess1 = 100.0f;
	_material1 = glm::vec3( 1.0, 0.0, 0.0 );

	_shininess2 = 100.0f;
	_material2 = glm::vec3( 0.0, 1.0, 0.0 );

	

	//====
	GLfloat maxAniso = 0.0f;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso );

	std::cout << "Max anistropy " << maxAniso << std::endl;
	//====

	//Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
	glGenSamplers( 1, &_sampler );
	glSamplerParameteri( _sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glSamplerParameteri( _sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glSamplerParameteri( _sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glSamplerParameteri( _sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	glSamplerParameteri( _repeatSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glSamplerParameteri( _repeatSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	glSamplerParameterf( _repeatSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f );

	glSamplerParameteri( _repeatSampler, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glSamplerParameteri( _repeatSampler, GL_TEXTURE_WRAP_T, GL_REPEAT );


}

void Application::drawBuidlingsOnScene( Camera& camera )
{
	//====== Buildings ======	
	glUseProgram( _buildingMaterial.getProgramId() ); //Подключаем общий шейдер для всех объектов

	_buildingMaterial.setTime( (float)glfwGetTime() );
	_buildingMaterial.setViewMatrix( camera.getViewMatrix() );
	_buildingMaterial.setProjectionMatrix( camera.getProjMatrix() );

	_buildingMaterial.setLightPos( _lightPos );
	_buildingMaterial.setAmbientColor( _ambientColor );
	_buildingMaterial.setDiffuseColor( _diffuseColor );
	_buildingMaterial.setSpecularColor( _specularColor );

	_buildingMaterial.applyCommonUniforms();

	//if( demoNum == 4 || demoNum == 5 ) {
		glEnable( GL_CULL_FACE );
		glFrontFace( GL_CW );
		glCullFace( GL_BACK );
	//}

	/*if( demoNum == 5 ) {
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}

	if( demoNum == 6 ) {
		glEnable( GL_STENCIL_TEST );

		glStencilFunc( GL_ALWAYS, 1, 1 );
		glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	}*/

	//====== Buildings ======
	glActiveTexture( GL_TEXTURE0 + 0 );  //текстурный юнит 0
	glBindTexture( GL_TEXTURE_2D, _buildingTexId );
	glBindSampler( 0, _sampler );

	_buildingMaterial.setDiffuseTexUnit( 0 ); //текстурный юнит 0
	_buildingMaterial.setModelMatrix( glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) ) );
	_buildingMaterial.setShininess( 100.0f );
	_buildingMaterial.applyModelSpecificUniforms();

	glBindVertexArray( _buildings.getVao() ); //Подключаем VertexArray для сферы
	glDrawArrays( GL_TRIANGLES, 0, _buildings.getNumVertices() ); //Рисуем сферу

#if 0
	glCullFace( GL_FRONT );
	glBindVertexArray( _sphere.getVao() ); //Подключаем VertexArray для сферы
	glDrawArrays( GL_TRIANGLES, 0, _sphere.getNumVertices() ); //Рисуем сферу
	glCullFace( GL_BACK );
	glBindVertexArray( _sphere.getVao() ); //Подключаем VertexArray для сферы
	glDrawArrays( GL_TRIANGLES, 0, _sphere.getNumVertices() ); //Рисуем сферу	
#endif

	/*if( demoNum == 6 ) {
		glStencilFunc( GL_NOTEQUAL, 1, 1 );
		glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	}*/

	//====== Плоскость YZ ======
	//glActiveTexture( GL_TEXTURE0 + 0 );  //текстурный юнит 0
	//glBindTexture( GL_TEXTURE_2D, _brickTexId );
	//glBindSampler( 0, _sampler );

	//_commonMaterial.setDiffuseTexUnit( 0 ); //текстурный юнит 0
	//_commonMaterial.setModelMatrix( glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0f, -1.0f, 0.0f ) ) );
	//_commonMaterial.setShininess( 100.0f );
	//_commonMaterial.applyModelSpecificUniforms();

	//glBindVertexArray( _plane.getVao() ); //Подключаем VertexArray для плоскости
	//glDrawArrays( GL_TRIANGLES, 0, _plane.getNumVertices() ); //Рисуем плоскость

	//if( demoNum == 3 ) {
	//	glActiveTexture( GL_TEXTURE0 + 0 );  //текстурный юнит 0
	//	glBindTexture( GL_TEXTURE_2D, _worldTexId );
	//	glBindSampler( 0, _sampler );

	//	_commonMaterial.setDiffuseTexUnit( 0 ); //текстурный юнит 0
	//	_commonMaterial.setModelMatrix( glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0001f, -1.0f, 0.0f ) ) );
	//	_commonMaterial.setShininess( 100.0f );
	//	_commonMaterial.applyModelSpecificUniforms();

	//	//glDisable(GL_DEPTH_TEST);
	//	//glPolygonOffset(-1.0f, -1.0f);

	//	glBindVertexArray( _plane.getVao() ); //Подключаем VertexArray для плоскости
	//	glDrawArrays( GL_TRIANGLES, 0, _plane.getNumVertices() ); //Рисуем плоскость

	//	//glPolygonOffset(0.0f, 0.0f);
	//	//glEnable(GL_DEPTH_TEST);
	//}

	//glDisable( GL_STENCIL_TEST );
	//glDisable( GL_BLEND );
	glDisable( GL_CULL_FACE );
}
void Application::drawRoadsOnScene( Camera& camera )
{
	//====== Buildings ======	
	glUseProgram( _roadMaterial.getProgramId() ); //Подключаем общий шейдер для всех объектов

	_buildingMaterial.setTime( (float)glfwGetTime() );
	_buildingMaterial.setViewMatrix( camera.getViewMatrix() );
	_buildingMaterial.setProjectionMatrix( camera.getProjMatrix() );

	_buildingMaterial.setLightPos( _lightPos );
	_buildingMaterial.setAmbientColor( _ambientColor );
	_buildingMaterial.setDiffuseColor( _diffuseColor );
	_buildingMaterial.setSpecularColor( _specularColor );

	_buildingMaterial.applyCommonUniforms();

	//if( demoNum == 4 || demoNum == 5 ) {
	glEnable( GL_CULL_FACE );
	glFrontFace( GL_CW );
	glCullFace( GL_BACK );
	//}

	/*if( demoNum == 5 ) {
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}

	if( demoNum == 6 ) {
	glEnable( GL_STENCIL_TEST );

	glStencilFunc( GL_ALWAYS, 1, 1 );
	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	}*/

	//====== Buildings ======
	glActiveTexture( GL_TEXTURE0 + 0 );  //текстурный юнит 0
	glBindTexture( GL_TEXTURE_2D, _buildingTexId );
	glBindSampler( 0, _sampler );

	_buildingMaterial.setDiffuseTexUnit( 0 ); //текстурный юнит 0
	_buildingMaterial.setModelMatrix( glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) ) );
	_buildingMaterial.setShininess( 100.0f );
	_buildingMaterial.applyModelSpecificUniforms();

	glBindVertexArray( _buildings.getVao() ); //Подключаем VertexArray для сферы
	glDrawArrays( GL_TRIANGLES, 0, _buildings.getNumVertices() ); //Рисуем сферу

#if 0
	glCullFace( GL_FRONT );
	glBindVertexArray( _sphere.getVao() ); //Подключаем VertexArray для сферы
	glDrawArrays( GL_TRIANGLES, 0, _sphere.getNumVertices() ); //Рисуем сферу
	glCullFace( GL_BACK );
	glBindVertexArray( _sphere.getVao() ); //Подключаем VertexArray для сферы
	glDrawArrays( GL_TRIANGLES, 0, _sphere.getNumVertices() ); //Рисуем сферу	
#endif

	/*if( demoNum == 6 ) {
	glStencilFunc( GL_NOTEQUAL, 1, 1 );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	}*/

	//====== Плоскость YZ ======
	//glActiveTexture( GL_TEXTURE0 + 0 );  //текстурный юнит 0
	//glBindTexture( GL_TEXTURE_2D, _brickTexId );
	//glBindSampler( 0, _sampler );

	//_commonMaterial.setDiffuseTexUnit( 0 ); //текстурный юнит 0
	//_commonMaterial.setModelMatrix( glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0f, -1.0f, 0.0f ) ) );
	//_commonMaterial.setShininess( 100.0f );
	//_commonMaterial.applyModelSpecificUniforms();

	//glBindVertexArray( _plane.getVao() ); //Подключаем VertexArray для плоскости
	//glDrawArrays( GL_TRIANGLES, 0, _plane.getNumVertices() ); //Рисуем плоскость

	//if( demoNum == 3 ) {
	//	glActiveTexture( GL_TEXTURE0 + 0 );  //текстурный юнит 0
	//	glBindTexture( GL_TEXTURE_2D, _worldTexId );
	//	glBindSampler( 0, _sampler );

	//	_commonMaterial.setDiffuseTexUnit( 0 ); //текстурный юнит 0
	//	_commonMaterial.setModelMatrix( glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0001f, -1.0f, 0.0f ) ) );
	//	_commonMaterial.setShininess( 100.0f );
	//	_commonMaterial.applyModelSpecificUniforms();

	//	//glDisable(GL_DEPTH_TEST);
	//	//glPolygonOffset(-1.0f, -1.0f);

	//	glBindVertexArray( _plane.getVao() ); //Подключаем VertexArray для плоскости
	//	glDrawArrays( GL_TRIANGLES, 0, _plane.getNumVertices() ); //Рисуем плоскость

	//	//glPolygonOffset(0.0f, 0.0f);
	//	//glEnable(GL_DEPTH_TEST);
	//}

	//glDisable( GL_STENCIL_TEST );
	//glDisable( GL_BLEND );
	glDisable( GL_CULL_FACE );
}
