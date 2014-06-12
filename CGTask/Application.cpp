#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>


#include "Application.h"
#include "mapWorker.h"
#include "geometry.h"
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

void addPoint(std::vector<float>& vec, float x, float y, float z)
{
	vec.push_back(x);
	vec.push_back(y);
	vec.push_back(z);
}

void addColor(std::vector<float>& vec, float r, float g, float b, float a)
{
	vec.push_back(r);
	vec.push_back(g);
	vec.push_back(b);
	vec.push_back(a);
}

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


void Application::makeSurface()
{
	_surfaceNumTris = 0; 
	float r = 0.1, g = 0.45, b = 0.0;

	std::vector<float> vertices;
	std::vector<float> colors;
	
	pave_floor(0, 0, 100, 100, vertices, colors);
	//pave_wall(2, 2, 60, 50, 10, vertices, colors);

	vertices.insert(vertices.end(), colors.begin(), colors.end());

	unsigned int vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	_surfaceVao = 0;
	glGenVertexArrays(1, &_surfaceVao);
	glBindVertexArray(_surfaceVao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(_surfaceNumTris * 3 * 3 * 4));

	glBindVertexArray(0);

	_surfaceModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Application::makeBuildings()
{
	_buildingNumTris = 0; 
	//float r = 0.5, g = 0.6, b = 0.55;

	std::vector<float> vertices;
	std::vector<float> colors;

	makeBuildings(vertices, colors);

	vertices.insert(vertices.end(), colors.begin(), colors.end());

	unsigned int vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	_buildingVao = 0;
	glGenVertexArrays(1, &_buildingVao);
	glBindVertexArray(_buildingVao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(_buildingNumTris * 3 * 3 * 4));

	glBindVertexArray(0);

	_buildingModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	
}

void Application::makeRoads()
{
	_roadNumTris = 0;
	//float r = 0.5, g = 0.6, b = 0.55;

	std::vector<float> vertices;
	std::vector<float> colors;

	makeRoads(vertices, colors);

	vertices.insert(vertices.end(), colors.begin(), colors.end());

	unsigned int vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	_roadVao = 0;
	glGenVertexArrays(1, &_roadVao);
	glBindVertexArray(_roadVao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(_roadNumTris * 3 * 3 * 4));

	glBindVertexArray(0);

	_roadModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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

void Application::pave_floor(float first_x, float first_y, float second_x, float second_y, std::vector<float>& vertices, std::vector<float>& colors)
{
	float r = 0.7, g = .7, b = 0.7;

	int localXResolution = int(abs(first_x - second_x)); // округленное количество квадратов замощения
	int localYResolution = int(abs(first_y - second_y));

	float localA = first_x;
	float localB = first_y;
	float localC = C;

	int iDirection = int(abs(second_x - first_x) / (second_x - first_x));
	int jDirection = int(abs(second_y - first_y) / (second_y - first_y));

	std::cout << "iDirection = " << iDirection << std::endl;
	std::cout << "jDirection = " << jDirection << std::endl;
	std::cout << "localXResolution = " << localXResolution << std::endl;
	std::cout << "localYResolution = " << localYResolution << std::endl;

	for( int i = 0; i < localXResolution; i++ ) {
		for( int j = 0; j < localYResolution; j++ ) {

			//Первый треугольник, образующий квадрат
			addPoint(vertices, A + (float)j + jDirection, B + i, C);
			addPoint(vertices, A + (float)j, B + i, C);
			addPoint(vertices, A + (float)j, B + i + iDirection, C);

			addColor(colors, r, g, b, 1.0);
			addColor(colors, r, g, b, 1.0);
			addColor(colors, r, g, b, 1.0);

			_surfaceNumTris++;

			//Второй треугольник, образующий квадрат
			addPoint(vertices, A + 1. + (float)j, B + i, C);
			addPoint(vertices, A + (float)j + jDirection, B + i + iDirection, C);
			addPoint(vertices, A + (float)j, B + i + iDirection, C);

			addColor(colors, r, g, b, 1.0);
			addColor(colors, r, g, b, 1.0);
			addColor(colors, r, g, b, 1.0);

			_surfaceNumTris++;
		}
	}
}

void Application::pave_wall(double first_x, double first_y, double second_x, double second_y, double height, std::vector<float>& vertices, std::vector<float>& colors, bool col)
{
	float r, g, b;
	if( col ) {
		r = 1.;
		g = .70;
		b = 0.0;
	} else {
		r = 0.9; g = .60; b = 0.0;
	}


	//Первый треугольник, образующий квадрат
	addPoint(vertices, first_x, first_y, C);
	addPoint(vertices, second_x, second_y, C);
	addPoint(vertices, first_x, first_y, height);

	addColor(colors, r, g, b, 1.0);
	addColor(colors, r, g, b, 1.0);
	addColor(colors, r, g, b, 1.0);

	_buildingNumTris++;

	//Второй треугольник, образующий квадрат
	addPoint(vertices, first_x, first_y, height);
	addPoint(vertices, second_x, second_y, C);
	addPoint(vertices, second_x, second_y, height);

	addColor(colors, r, g, b, 1.0);
	addColor(colors, r, g, b, 1.0);
	addColor(colors, r, g, b, 1.0);

	_buildingNumTris++;
}

void Application::pave_road(double first_x, double first_y, double second_x, double second_y, std::vector<float>& vertices, std::vector<float>& colors)
{
	float r, g, b;
	r = 0.3; g = .3; b = 0.3;
	
	float roadHalfWidth = .5;
	// требуется получить точку, лежащуу на фикс расстоянии от первой на перпердикуляре прямой a_1, a_2
	std::pair< std::pair<std::pair<double, double>, std::pair<double, double>>, std::pair<std::pair<double, double>, std::pair<double, double>>> 
		corners = getRoadCorners(first_x, first_y, second_x, second_y, roadHalfWidth);
	std::pair<std::pair<double, double>, std::pair<double, double>> firstCorners = corners.first;
	std::pair<std::pair<double, double>, std::pair<double, double>> secondCorners = corners.second;
	

	//Первый треугольник, образующий квадрат
	/*addPoint(vertices, firstCorners.first.first, firstCorners.first.second, C + .05);
	addPoint(vertices, secondCorners.first.first, secondCorners.first.second, C + .05);
	addPoint(vertices, firstCorners.second.first, firstCorners.second.second, C + .05);*/

	//addPoint(vertices, firstCorners.first.first, firstCorners.first.second, C + .05);
	addPoint(vertices, firstCorners.first.first, firstCorners.first.second, C + .05);
	addPoint(vertices, secondCorners.first.first, secondCorners.first.second, C + .05);
	addPoint(vertices, firstCorners.second.first, firstCorners.second.second, C + .05);
	
	addColor(colors, r, g, b, 1.0);
	addColor(colors, r, g, b, 1.0);
	addColor(colors, r, g, b, 1.0);

	_roadNumTris++;

	//Второй треугольник, образующий квадрат
	addPoint(vertices, firstCorners.second.first, firstCorners.second.second, C + .05);
	addPoint(vertices, secondCorners.first.first, secondCorners.first.second, C + .05);
	addPoint(vertices, secondCorners.second.first, secondCorners.second.second, C + .05);

	
	addColor(colors, r, g, b, 1.0);
	addColor(colors, r, g, b, 1.0);
	addColor(colors, r, g, b, 1.0);

	_roadNumTris++;
}

void Application::pave_roof( vector<vector<pair<float, float>>> triangles, double height, std::vector<float>& vertices, std::vector<float>& colors )
{
	float r, g, b;
	r = 0.; g = 0.; b = 0.;

	vector < vector<pair<float, float>>>::iterator it = triangles.begin();
	for( ; it != triangles.end(); it++ ) {
		addPoint( vertices, (*it)[0].first, (*it)[0].second, height );
		addPoint( vertices, (*it)[1].first, (*it)[1].second, height );
		addPoint( vertices, (*it)[2].first, (*it)[2].second, height );

		addColor( colors, r, g, b, 1.0 );
		addColor( colors, r, g, b, 1.0 );
		addColor( colors, r, g, b, 1.0 );

		_buildingNumTris++;
	}

}



// используя pave_wall отрисовать дома
void Application::makeBuildings(std::vector<float>& vertices, std::vector<float>& colors)
{
	std::vector<Building*/*way*/> buildings = worker->buildings;
	for each (Building* building in buildings)
	{
		double height = building->getHeight();
		std::vector<std::pair<double, double>> corners = building->getCorners();
		
		std::pair<double, double> curCorner = corners[0];
		std::pair<double, double> nextCorner;
		vector<pair<float, float>> roof_corners;

		bool col;
		for( int i = 1; i < corners.size(); i++ ) {
			nextCorner = corners[i];
			// отрисовываем стену
			if( i % 2 == 1 ) {
				col = true;
			} 
			else 
				col = false;
			pave_wall(curCorner.first/10., curCorner.second/10., nextCorner.first/10., nextCorner.second/10., height/10, vertices, colors, col);
			curCorner = nextCorner;
			//if( i != corners.size() - 1 ) {
				roof_corners.push_back( pair<float, float>( curCorner.first / 10., curCorner.second / 10. ) );
			//}
		}
		std::reverse( roof_corners.begin(), roof_corners.end() );
		Polygon poly( roof_corners );
		poly.triangulation();
		vector<vector<pair<float, float>>> roof_tris = poly.saveIntoVector();
		pave_roof( roof_tris, height/10, vertices, colors );
	}
	//vertices.insert( vertices.end(), colors.begin(), colors.end() );
	vertices.insert( vertices.end(), colors.begin(), colors.end() );
	
	unsigned int vbo = 0;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( float ), vertices.data(), GL_STATIC_DRAW );

	_buildingVao = 0;
	glGenVertexArrays( 1, &_buildingVao );
	glBindVertexArray( _buildingVao );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(_buildingNumTris * 3 * 3 * 4) );
}




// используя pave_road отрисовать дороги
void Application::makeRoads(std::vector<float>& vertices, std::vector<float>& colors)
{
	std::vector<Road*> roads = worker->roads;
	for each (Road* road in roads)
	{
		double level = road->getLevel();
		std::vector<std::pair<double, double>> vertexes = road->getVertexes();

		std::pair<double, double> curVertex = vertexes[0];
		std::pair<double, double> nextVertex;
		bool col;
		for( int i = 1; i < vertexes.size(); i++ ) {
			nextVertex = vertexes[i];
			pave_road(curVertex.first / 10., curVertex.second / 10., nextVertex.first / 10., nextVertex.second / 10., vertices, colors);
			curVertex = nextVertex;
		}

	}
	vertices.insert( vertices.end(), colors.begin(), colors.end() );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(_roadNumTris * 3 * 3 * 4) );
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

std::pair< std::pair<std::pair<double, double>, std::pair<double, double>>, std::pair<std::pair<double, double>, std::pair<double, double>>> Application::getRoadCorners(double first_x, double first_y, double second_x, double second_y, double roadHalfWidth)
{
	std::pair< std::pair<std::pair<double, double>, std::pair<double, double>>, std::pair<std::pair<double, double>, std::pair<double, double>>> corners;
	std::pair<std::pair<double, double>, std::pair<double, double>> coords =
		std::pair<std::pair<double, double>, std::pair<double, double>>(
		std::pair<double, double>(first_x, first_y),
		std::pair<double, double>(second_x, second_y)
		);
	double angle = getAngleToXO(coords);

	if( angle != 0.) {
		std::pair<std::pair<double, double>, std::pair<double, double>> newCoords = turnOnAngle(coords, -angle);
		corners = getNormRoadCorners(newCoords, roadHalfWidth);
		corners.first = turnOnAngle(corners.first, angle);
		corners.second = turnOnAngle(corners.second, angle);
	} else {
		corners = getNormRoadCorners(coords, roadHalfWidth);
	}

	return corners;
}

std::pair< std::pair<std::pair<double, double>, std::pair<double, double>>, std::pair<std::pair<double, double>, std::pair<double, double>>> Application::getNormRoadCorners(std::pair<std::pair<double, double>, std::pair<double, double>> vertexes, double roadHalfWidth)
{
	double first_x = vertexes.first.first;
	double first_y = vertexes.first.second;
	double second_x = vertexes.second.first;
	double second_y = vertexes.second.second;

	double first_right_x = first_x;
	double first_right_y = first_y - roadHalfWidth;
	
	double first_left_x = first_x;
	double first_left_y = first_y + roadHalfWidth;

	double second_right_x = second_x;
	double second_right_y = second_y - roadHalfWidth;
	
	double second_left_x = second_x;
	double second_left_y = second_y + roadHalfWidth;
	
	return std::pair<std::pair<std::pair<double, double>,std::pair<double, double >> , 
					 std::pair<std::pair<double, double>, std::pair<double, double>>
					>(std::pair<std::pair<double, double>, std::pair<double, double >>
						(std::pair<double, double >(first_right_x, first_right_y), 
						 std::pair<double, double >(first_left_x, first_left_y)), 
					  std::pair<std::pair<double, double>, std::pair<double, double >>
					    (std::pair<double, double >(second_right_x, second_right_y), 
					     std::pair<double, double >(second_left_x, second_left_y))
					 );
}
std::pair<std::pair<double, double>, std::pair<double, double>> Application::turnOnAngle(std::pair<std::pair<double, double>, std::pair<double, double>> corners, double angle)
{
	double first_x = corners.first.first;
	double first_y = corners.first.second;
	double second_x = corners.second.first;
	double second_y = corners.second.second;

	// левосторонняя истема координат и положитеьное вращение против часовой
	double res_first_x = first_x * cos(angle) - first_y * sin(angle);
	double res_first_y = first_x * sin(angle) + first_y * cos(angle);

	double res_second_x = second_x * cos(angle) - second_y * sin(angle);
	double res_second_y = second_x * sin(angle) + second_y * cos(angle);

	return std::pair<std::pair<double, double>, std::pair<double, double>>(
		std::pair<double, double>(res_first_x, res_first_y),
		std::pair<double, double>(res_second_x, res_second_y)
		);
}
double Application::getAngleToXO(std::pair<std::pair<double, double>, std::pair<double, double>> corners)
{
	double first_x = corners.first.first;
	double first_y = corners.first.second;
	double second_x = corners.second.first;
	double second_y = corners.second.second;

	// get angle to XO
	if( first_y == second_y ) {
		return 0.;
	}
	else if( first_x == second_x ) {
		return 90.;
	} else {
	// interesting part
		double a = (second_y - first_y) / (second_x - first_x);
		return atan(-a);
	}
}

