#define _USE_MATH_DEFINES
#include <math.h>
#include <glimg/glimg.h>
#include <vector>

#include "Mesh.h"
#include "mapWorker.h"
#include "geometry.h"

using namespace std;
//=========================================================

const unsigned HeightSurfaceResolution = 25;
const unsigned LengthSurfaceResolution = 25;
const unsigned WidthSurfaceResolution = 25;
const float A = -5;
const float B = -5;
const float C = 0;

int paveBuildings( MapWorker* worker, std::vector<float>& vertices, std::vector<float>& colors );

float frand( float low, float high )
{
	return low + (high - low) * (rand() % 1000) * 0.001f;
}

void addVec2( std::vector<float>& vec, float s, float t )
{
	vec.push_back( s );
	vec.push_back( t );
}

void addVec3( std::vector<float>& vec, float x, float y, float z )
{
	vec.push_back( x );
	vec.push_back( y );
	vec.push_back( z );
}

void addVec4( std::vector<float>& vec, float r, float g, float b, float a )
{
	vec.push_back( r );
	vec.push_back( g );
	vec.push_back( b );
	vec.push_back( a );
}

void addPoint( std::vector<float>& vec, float x, float y, float z )
{
	vec.push_back( x );
	vec.push_back( y );
	vec.push_back( z );
}

void addColor( std::vector<float>& vec, float r, float g, float b, float a )
{
	vec.push_back( r );
	vec.push_back( g );
	vec.push_back( b );
	vec.push_back( a );
}

//вычисление цвета по линейной палитре
void getColorFromLinearPalette( float value, float& r, float& g, float& b )
{
	if( value < 0.25f ) {
		r = 0.0f;
		g = value * 4.0f;
		b = 1.0f;
	} else if( value < 0.5f ) {
		r = 0.0f;
		g = 1.0f;
		b = (0.5f - value) * 4.0f;
	} else if( value < 0.75f ) {
		r = (value - 0.5f) * 4.0f;
		g = 1.0f;
		b = 0.0f;
	} else {
		r = 1.0f;
		g = (1.0f - value) * 4.0f;
		b = 0.0f;
	}
}

//=========================================================

Mesh::Mesh() :
_vao( 0 ),
_numVertices( 0 )
{}

Mesh::Mesh( GLuint vao, int numVertices ) :
_vao( vao ),
_numVertices( numVertices )
{}


Mesh Mesh::makeGround()
{
	int surfaceNumTris = 0;
	float r = 0.1, g = 0.45, b = 0.0;

	std::vector<float> vertices;
	std::vector<float> colors;

	surfaceNumTris += pave_floor( 0, 0, 100, 100, vertices, colors );
	//pave_wall(2, 2, 60, 50, 10, vertices, colors);

	vertices.insert( vertices.end(), colors.begin(), colors.end() );

	unsigned int vbo = 0;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( float ), vertices.data(), GL_STATIC_DRAW );

	GLuint surfaceVao = 0;
	glGenVertexArrays( 1, &surfaceVao );
	glBindVertexArray( surfaceVao );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(surfaceNumTris * 3 * 3 * 4) );

	glBindVertexArray( 0 );

	//_surfaceModelMatrix = glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );

	return Mesh( surfaceVao, surfaceNumTris * 3 );
}

Mesh Mesh::makeBuildings(MapWorker* worker)
{
	int buildingNumTris = 0;
	//float r = 0.5, g = 0.6, b = 0.55;

	std::vector<float> vertices;
	std::vector<float> colors;

	buildingNumTris += paveBuildings(worker, vertices, colors );

	vertices.insert( vertices.end(), colors.begin(), colors.end() );

	unsigned int vbo = 0;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( float ), vertices.data(), GL_STATIC_DRAW );

	GLuint buildingVao = 0;
	glGenVertexArrays( 1, &buildingVao );
	glBindVertexArray( buildingVao );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(buildingNumTris * 3 * 3 * 4) );

	glBindVertexArray( 0 );

	//_buildingModelMatrix = glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
	return Mesh( buildingVao, buildingNumTris * 3 );
}

Mesh Mesh::makeRoads(MapWorker* worker)
{
	int roadNumTris = 0;
	//float r = 0.5, g = 0.6, b = 0.55;

	std::vector<float> vertices;
	std::vector<float> colors;

	roadNumTris += paveRoads(worker, vertices, colors );

	vertices.insert( vertices.end(), colors.begin(), colors.end() );

	unsigned int vbo = 0;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( float ), vertices.data(), GL_STATIC_DRAW );

	GLuint roadVao = 0;
	glGenVertexArrays( 1, &roadVao );
	glBindVertexArray( roadVao );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(roadNumTris * 3 * 3 * 4) );

	glBindVertexArray( 0 );

	//_roadModelMatrix = glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
	return Mesh( roadVao, roadNumTris * 3 );
}
// использу€ pave_road отрисовать дороги
int paveRoads(MapWorker* worker, std::vector<float>& vertices, std::vector<float>& colors )
{
	int roadNumTris = 0;
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
			roadNumTris += pave_road( curVertex.first / 10., curVertex.second / 10., nextVertex.first / 10., nextVertex.second / 10., vertices, colors );
			curVertex = nextVertex;
		}
	}
	vertices.insert( vertices.end(), colors.begin(), colors.end() );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(roadNumTris * 3 * 3 * 4) );

	return roadNumTris;
}


// использу€ pave_wall и paveRoof отрисовать дома
int paveBuildings(MapWorker* worker, std::vector<float>& vertices, std::vector<float>& colors )
{
	int buildingNumTris = 0;
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
			} else
				col = false;
			buildingNumTris += pave_wall( curCorner.first / 10., curCorner.second / 10., nextCorner.first / 10., nextCorner.second / 10., height / 10, vertices, colors, col );
			curCorner = nextCorner;
			//if( i != corners.size() - 1 ) {
			roof_corners.push_back( pair<float, float>( curCorner.first / 10., curCorner.second / 10. ) );
			//}
		}
		std::reverse( roof_corners.begin(), roof_corners.end() );
		Polygon poly( roof_corners );
		poly.triangulation();
		vector<vector<pair<float, float>>> roof_tris = poly.saveIntoVector();
		buildingNumTris += pave_roof( roof_tris, height / 10, vertices, colors );
	}
	//vertices.insert( vertices.end(), colors.begin(), colors.end() );
	vertices.insert( vertices.end(), colors.begin(), colors.end() );

	unsigned int vbo = 0;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( float ), vertices.data(), GL_STATIC_DRAW );

	GLuint buildingVao = 0;
	glGenVertexArrays( 1, &buildingVao );
	glBindVertexArray( buildingVao );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(buildingNumTris * 3 * 3 * 4) );

	return buildingNumTris;
}

// paving functions
int pave_floor( float first_x, float first_y, float second_x, float second_y, std::vector<float>& vertices, std::vector<float>& colors )
{
	float r = 0.7, g = .7, b = 0.7;
	int surfaceNumTris = 0;

	int localXResolution = int( abs( first_x - second_x ) ); // округленное количество квадратов замощени€
	int localYResolution = int( abs( first_y - second_y ) );

	float localA = first_x;
	float localB = first_y;
	float localC = C;

	int iDirection = int( abs( second_x - first_x ) / (second_x - first_x) );
	int jDirection = int( abs( second_y - first_y ) / (second_y - first_y) );

	std::cout << "iDirection = " << iDirection << std::endl;
	std::cout << "jDirection = " << jDirection << std::endl;
	std::cout << "localXResolution = " << localXResolution << std::endl;
	std::cout << "localYResolution = " << localYResolution << std::endl;

	for( int i = 0; i < localXResolution; i++ ) {
		for( int j = 0; j < localYResolution; j++ ) {

			//ѕервый треугольник, образующий квадрат
			addPoint( vertices, A + (float)j + jDirection, B + i, C );
			addPoint( vertices, A + (float)j, B + i, C );
			addPoint( vertices, A + (float)j, B + i + iDirection, C );

			addColor( colors, r, g, b, 1.0 );
			addColor( colors, r, g, b, 1.0 );
			addColor( colors, r, g, b, 1.0 );

			surfaceNumTris++;

			//¬торой треугольник, образующий квадрат
			addPoint( vertices, A + 1. + (float)j, B + i, C );
			addPoint( vertices, A + (float)j + jDirection, B + i + iDirection, C );
			addPoint( vertices, A + (float)j, B + i + iDirection, C );

			addColor( colors, r, g, b, 1.0 );
			addColor( colors, r, g, b, 1.0 );
			addColor( colors, r, g, b, 1.0 );

			surfaceNumTris++;
		}
	}
	return surfaceNumTris;
}

int pave_wall( double first_x, double first_y, double second_x, double second_y, double height, std::vector<float>& vertices, std::vector<float>& colors, bool col )
{
	int buildingNumTris = 0;
	float r, g, b;
	if( col ) {
		r = 1.;
		g = .70;
		b = 0.0;
	} else {
		r = 0.9; g = .60; b = 0.0;
	}


	//ѕервый треугольник, образующий квадрат
	addPoint( vertices, first_x, first_y, C );
	addPoint( vertices, second_x, second_y, C );
	addPoint( vertices, first_x, first_y, height );

	addColor( colors, r, g, b, 1.0 );
	addColor( colors, r, g, b, 1.0 );
	addColor( colors, r, g, b, 1.0 );

	buildingNumTris++;

	//¬торой треугольник, образующий квадрат
	addPoint( vertices, first_x, first_y, height );
	addPoint( vertices, second_x, second_y, C );
	addPoint( vertices, second_x, second_y, height );

	addColor( colors, r, g, b, 1.0 );
	addColor( colors, r, g, b, 1.0 );
	addColor( colors, r, g, b, 1.0 );

	buildingNumTris++;

	return buildingNumTris;
}

int pave_roof( vector<vector<pair<float, float>>> triangles, double height, std::vector<float>& vertices, std::vector<float>& colors )
{
	float r, g, b;
	r = 0.; g = 0.; b = 0.;
	int buildingNumTris = 0;

	vector < vector<pair<float, float>>>::iterator it = triangles.begin();
	for( ; it != triangles.end(); it++ ) {
		addPoint( vertices, (*it)[0].first, (*it)[0].second, height );
		addPoint( vertices, (*it)[1].first, (*it)[1].second, height );
		addPoint( vertices, (*it)[2].first, (*it)[2].second, height );

		addColor( colors, r, g, b, 1.0 );
		addColor( colors, r, g, b, 1.0 );
		addColor( colors, r, g, b, 1.0 );

		buildingNumTris++;
	}
	return buildingNumTris;
}

int pave_road( double first_x, double first_y, double second_x, double second_y, std::vector<float>& vertices, std::vector<float>& colors )
{
	float r, g, b;
	r = 0.3; g = .3; b = 0.3;
	int roadNumTris = 0;

	float roadHalfWidth = .5;
	// требуетс€ получить точку, лежащуу на фикс рассто€нии от первой на перпердикул€ре пр€мой a_1, a_2
	std::pair< std::pair<std::pair<double, double>, std::pair<double, double>>, std::pair<std::pair<double, double>, std::pair<double, double>>>
		corners = getRoadCorners( first_x, first_y, second_x, second_y, roadHalfWidth );
	std::pair<std::pair<double, double>, std::pair<double, double>> firstCorners = corners.first;
	std::pair<std::pair<double, double>, std::pair<double, double>> secondCorners = corners.second;


	//ѕервый треугольник, образующий квадрат
	/*addPoint(vertices, firstCorners.first.first, firstCorners.first.second, C + .05);
	addPoint(vertices, secondCorners.first.first, secondCorners.first.second, C + .05);
	addPoint(vertices, firstCorners.second.first, firstCorners.second.second, C + .05);*/

	//addPoint(vertices, firstCorners.first.first, firstCorners.first.second, C + .05);
	addPoint( vertices, firstCorners.first.first, firstCorners.first.second, C + .05 );
	addPoint( vertices, secondCorners.first.first, secondCorners.first.second, C + .05 );
	addPoint( vertices, firstCorners.second.first, firstCorners.second.second, C + .05 );

	addColor( colors, r, g, b, 1.0 );
	addColor( colors, r, g, b, 1.0 );
	addColor( colors, r, g, b, 1.0 );

	roadNumTris++;

	//¬торой треугольник, образующий квадрат
	addPoint( vertices, firstCorners.second.first, firstCorners.second.second, C + .05 );
	addPoint( vertices, secondCorners.first.first, secondCorners.first.second, C + .05 );
	addPoint( vertices, secondCorners.second.first, secondCorners.second.second, C + .05 );


	addColor( colors, r, g, b, 1.0 );
	addColor( colors, r, g, b, 1.0 );
	addColor( colors, r, g, b, 1.0 );

	roadNumTris++;

	return roadNumTris;
}

// road utils
std::pair< std::pair<std::pair<double, double>, std::pair<double, double>>, std::pair<std::pair<double, double>, std::pair<double, double>>> getRoadCorners( double first_x, double first_y, double second_x, double second_y, double roadHalfWidth )
{
	std::pair< std::pair<std::pair<double, double>, std::pair<double, double>>, std::pair<std::pair<double, double>, std::pair<double, double>>> corners;
	std::pair<std::pair<double, double>, std::pair<double, double>> coords =
		std::pair<std::pair<double, double>, std::pair<double, double>>(
		std::pair<double, double>( first_x, first_y ),
		std::pair<double, double>( second_x, second_y )
		);
	double angle = getAngleToXO( coords );

	if( angle != 0. ) {
		std::pair<std::pair<double, double>, std::pair<double, double>> newCoords = turnOnAngle( coords, -angle );
		corners = getNormRoadCorners( newCoords, roadHalfWidth );
		corners.first = turnOnAngle( corners.first, angle );
		corners.second = turnOnAngle( corners.second, angle );
	} else {
		corners = getNormRoadCorners( coords, roadHalfWidth );
	}

	return corners;
}
std::pair< std::pair<std::pair<double, double>, std::pair<double, double>>, std::pair<std::pair<double, double>, std::pair<double, double>>> getNormRoadCorners( std::pair<std::pair<double, double>, std::pair<double, double>> vertexes, double roadHalfWidth )
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

	return std::pair<std::pair<std::pair<double, double>, std::pair<double, double >>,
		std::pair<std::pair<double, double>, std::pair<double, double>>
					>( std::pair<std::pair<double, double>, std::pair<double, double >>
					( std::pair<double, double >( first_right_x, first_right_y ),
					std::pair<double, double >( first_left_x, first_left_y ) ),
					std::pair<std::pair<double, double>, std::pair<double, double >>
					( std::pair<double, double >( second_right_x, second_right_y ),
					std::pair<double, double >( second_left_x, second_left_y ) )
					);
}
std::pair<std::pair<double, double>, std::pair<double, double>> turnOnAngle( std::pair<std::pair<double, double>, std::pair<double, double>> corners, double angle )
{
	double first_x = corners.first.first;
	double first_y = corners.first.second;
	double second_x = corners.second.first;
	double second_y = corners.second.second;

	// левосторонн€€ истема координат и положитеьное вращение против часовой
	double res_first_x = first_x * cos( angle ) - first_y * sin( angle );
	double res_first_y = first_x * sin( angle ) + first_y * cos( angle );

	double res_second_x = second_x * cos( angle ) - second_y * sin( angle );
	double res_second_y = second_x * sin( angle ) + second_y * cos( angle );

	return std::pair<std::pair<double, double>, std::pair<double, double>>(
		std::pair<double, double>( res_first_x, res_first_y ),
		std::pair<double, double>( res_second_x, res_second_y )
		);
}
double getAngleToXO( std::pair<std::pair<double, double>, std::pair<double, double>> corners )
{
	double first_x = corners.first.first;
	double first_y = corners.first.second;
	double second_x = corners.second.first;
	double second_y = corners.second.second;

	// get angle to XO
	if( first_y == second_y ) {
		return 0.;
	} else if( first_x == second_x ) {
		return 90.;
	} else {
		// interesting part
		double a = (second_y - first_y) / (second_x - first_x);
		return atan( -a );
	}
}

//
//Mesh Mesh::makeSphere( float radius )
//{
//	int N = 100;
//	int M = 50;
//	int numVertices = 0;
//
//	std::vector<float> vertices;
//	std::vector<float> normals;
//	std::vector<float> texcoords;
//	for( int i = 0; i < M; i++ ) {
//		float theta = (float)M_PI * i / M;
//		float theta1 = (float)M_PI * (i + 1) / M;
//
//		for( int j = 0; j < N; j++ ) {
//			float phi = 2.0f * (float)M_PI * j / N + (float)M_PI;
//			float phi1 = 2.0f * (float)M_PI * (j + 1) / N + (float)M_PI;
//
//			//ѕервый треугольник, образующий квад
//			addVec3( vertices, cos( phi ) * sin( theta ) * radius, sin( phi ) * sin( theta ) * radius, cos( theta ) * radius );
//			addVec3( vertices, cos( phi1 ) * sin( theta ) * radius, sin( phi1 ) * sin( theta ) * radius, cos( theta ) * radius );
//			addVec3( vertices, cos( phi1 ) * sin( theta1 ) * radius, sin( phi1 ) * sin( theta1 ) * radius, cos( theta1 ) * radius );
//
//			addVec3( normals, cos( phi ) * sin( theta ), sin( phi ) * sin( theta ), cos( theta ) );
//			addVec3( normals, cos( phi1 ) * sin( theta ), sin( phi1 ) * sin( theta ), cos( theta ) );
//			addVec3( normals, cos( phi1 ) * sin( theta1 ), sin( phi1 ) * sin( theta1 ), cos( theta1 ) );
//
//			addVec2( texcoords, (float)j / N, 1.0f - (float)i / M );
//			addVec2( texcoords, (float)(j + 1) / N, 1.0f - (float)i / M );
//			addVec2( texcoords, (float)(j + 1) / N, 1.0f - (float)(i + 1) / M );
//
//			numVertices += 3;
//
//			//¬торой треугольник, образующий квад
//			addVec3( vertices, cos( phi ) * sin( theta ) * radius, sin( phi ) * sin( theta ) * radius, cos( theta ) * radius );
//			addVec3( vertices, cos( phi1 ) * sin( theta1 ) * radius, sin( phi1 ) * sin( theta1 ) * radius, cos( theta1 ) * radius );
//			addVec3( vertices, cos( phi ) * sin( theta1 ) * radius, sin( phi ) * sin( theta1 ) * radius, cos( theta1 ) * radius );
//
//			addVec3( normals, cos( phi ) * sin( theta ), sin( phi ) * sin( theta ), cos( theta ) );
//			addVec3( normals, cos( phi1 ) * sin( theta1 ), sin( phi1 ) * sin( theta1 ), cos( theta1 ) );
//			addVec3( normals, cos( phi ) * sin( theta1 ), sin( phi ) * sin( theta1 ), cos( theta1 ) );
//
//			addVec2( texcoords, (float)j / N, 1.0f - (float)i / M );
//			addVec2( texcoords, (float)(j + 1) / N, 1.0f - (float)(i + 1) / M );
//			addVec2( texcoords, (float)j / N, 1.0f - (float)(i + 1) / M );
//
//			numVertices += 3;
//		}
//	}
//
//	vertices.insert( vertices.end(), normals.begin(), normals.end() );
//	vertices.insert( vertices.end(), texcoords.begin(), texcoords.end() );
//
//	GLuint vbo = 0;
//	glGenBuffers( 1, &vbo );
//	glBindBuffer( GL_ARRAY_BUFFER, vbo );
//	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( float ), vertices.data(), GL_STATIC_DRAW );
//
//	GLuint sphereVao = 0;
//	glGenVertexArrays( 1, &sphereVao );
//	glBindVertexArray( sphereVao );
//	glEnableVertexAttribArray( 0 );
//	glEnableVertexAttribArray( 1 );
//	glEnableVertexAttribArray( 2 );
//	glBindBuffer( GL_ARRAY_BUFFER, vbo );
//	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
//	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4) );
//	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4 * 2) );
//
//	glBindVertexArray( 0 );
//
//	return Mesh( sphereVao, numVertices );
//}
//
//Mesh Mesh::makeCube( float size )
//{
//	std::vector<float> vertices;
//	std::vector<float> normals;
//	std::vector<float> texcoords;
//
//	//front 1
//	addVec3( vertices, size, -size, size );
//	addVec3( vertices, size, size, size );
//	addVec3( vertices, size, size, -size );
//
//	addVec3( normals, 1.0, 0.0, 0.0 );
//	addVec3( normals, 1.0, 0.0, 0.0 );
//	addVec3( normals, 1.0, 0.0, 0.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 1.0, 1.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//
//	//front 2
//	addVec3( vertices, size, -size, size );
//	addVec3( vertices, size, size, -size );
//	addVec3( vertices, size, -size, -size );
//
//	addVec3( normals, 1.0, 0.0, 0.0 );
//	addVec3( normals, 1.0, 0.0, 0.0 );
//	addVec3( normals, 1.0, 0.0, 0.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//	addVec2( texcoords, 0.0, 0.0 );
//
//	//left 1
//	addVec3( vertices, -size, -size, size );
//	addVec3( vertices, size, -size, size );
//	addVec3( vertices, size, -size, -size );
//
//	addVec3( normals, 0.0, -1.0, 0.0 );
//	addVec3( normals, 0.0, -1.0, 0.0 );
//	addVec3( normals, 0.0, -1.0, 0.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 1.0, 1.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//
//	//left 2
//	addVec3( vertices, -size, -size, size );
//	addVec3( vertices, size, -size, -size );
//	addVec3( vertices, -size, -size, -size );
//
//	addVec3( normals, 0.0, -1.0, 0.0 );
//	addVec3( normals, 0.0, -1.0, 0.0 );
//	addVec3( normals, 0.0, -1.0, 0.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//	addVec2( texcoords, 0.0, 0.0 );
//
//	//top 1
//	addVec3( vertices, -size, size, size );
//	addVec3( vertices, size, size, size );
//	addVec3( vertices, size, -size, size );
//
//	addVec3( normals, 0.0, 0.0, 1.0 );
//	addVec3( normals, 0.0, 0.0, 1.0 );
//	addVec3( normals, 0.0, 0.0, 1.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 1.0, 1.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//
//	//top 2
//	addVec3( vertices, -size, size, size );
//	addVec3( vertices, -size, -size, size );
//	addVec3( vertices, size, -size, size );
//
//	addVec3( normals, 0.0, 0.0, 1.0 );
//	addVec3( normals, 0.0, 0.0, 1.0 );
//	addVec3( normals, 0.0, 0.0, 1.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 0.0, 0.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//
//	//back 1
//	addVec3( vertices, -size, -size, size );
//	addVec3( vertices, -size, size, -size );
//	addVec3( vertices, -size, size, size );
//
//	addVec3( normals, -1.0, 0.0, 0.0 );
//	addVec3( normals, -1.0, 0.0, 0.0 );
//	addVec3( normals, -1.0, 0.0, 0.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//	addVec2( texcoords, 1.0, 1.0 );
//
//	//back 2
//	addVec3( vertices, -size, -size, size );
//	addVec3( vertices, -size, -size, -size );
//	addVec3( vertices, -size, size, -size );
//
//	addVec3( normals, -1.0, 0.0, 0.0 );
//	addVec3( normals, -1.0, 0.0, 0.0 );
//	addVec3( normals, -1.0, 0.0, 0.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 0.0, 0.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//
//	//right 1
//	addVec3( vertices, -size, size, size );
//	addVec3( vertices, size, size, -size );
//	addVec3( vertices, size, size, size );
//
//	addVec3( normals, 0.0, 1.0, 0.0 );
//	addVec3( normals, 0.0, 1.0, 0.0 );
//	addVec3( normals, 0.0, 1.0, 0.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//	addVec2( texcoords, 1.0, 1.0 );
//
//	//right 2
//	addVec3( vertices, -size, size, size );
//	addVec3( vertices, -size, size, -size );
//	addVec3( vertices, +size, size, -size );
//
//	addVec3( normals, 0.0, 1.0, 0.0 );
//	addVec3( normals, 0.0, 1.0, 0.0 );
//	addVec3( normals, 0.0, 1.0, 0.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 0.0, 0.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//
//	//bottom 1
//	addVec3( vertices, -size, size, -size );
//	addVec3( vertices, size, -size, -size );
//	addVec3( vertices, size, size, -size );
//
//	addVec3( normals, 0.0, 0.0, -1.0 );
//	addVec3( normals, 0.0, 0.0, -1.0 );
//	addVec3( normals, 0.0, 0.0, -1.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//	addVec2( texcoords, 1.0, 1.0 );
//
//	//bottom 2
//	addVec3( vertices, -size, size, -size );
//	addVec3( vertices, size, -size, -size );
//	addVec3( vertices, -size, -size, -size );
//
//	addVec3( normals, 0.0, 0.0, -1.0 );
//	addVec3( normals, 0.0, 0.0, -1.0 );
//	addVec3( normals, 0.0, 0.0, -1.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//	addVec2( texcoords, 0.0, 0.0 );
//
//	vertices.insert( vertices.end(), normals.begin(), normals.end() );
//	vertices.insert( vertices.end(), texcoords.begin(), texcoords.end() );
//
//	int numVertices = 36;
//
//	GLuint vbo = 0;
//	glGenBuffers( 1, &vbo );
//	glBindBuffer( GL_ARRAY_BUFFER, vbo );
//	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( float ), vertices.data(), GL_STATIC_DRAW );
//
//	GLuint cubeVao = 0;
//	glGenVertexArrays( 1, &cubeVao );
//	glBindVertexArray( cubeVao );
//	glEnableVertexAttribArray( 0 );
//	glEnableVertexAttribArray( 1 );
//	glEnableVertexAttribArray( 2 );
//	glBindBuffer( GL_ARRAY_BUFFER, vbo );
//	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
//	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4) );
//	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4 * 2) );
//
//	glBindVertexArray( 0 );
//
//	return Mesh( cubeVao, numVertices );
//}
//
//Mesh Mesh::makeYZPlane( float size )
//{
//	std::vector<float> vertices;
//	std::vector<float> normals;
//	std::vector<float> texcoords;
//
//	//front 1
//	addVec3( vertices, 0.0, -size, size );
//	addVec3( vertices, 0.0, size, size );
//	addVec3( vertices, 0.0, size, -size );
//
//	addVec3( normals, 1.0, 0.0, 0.0 );
//	addVec3( normals, 1.0, 0.0, 0.0 );
//	addVec3( normals, 1.0, 0.0, 0.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 1.0, 1.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//
//	//front 2
//	addVec3( vertices, 0.0, -size, size );
//	addVec3( vertices, 0.0, size, -size );
//	addVec3( vertices, 0.0, -size, -size );
//
//	addVec3( normals, 1.0, 0.0, 0.0 );
//	addVec3( normals, 1.0, 0.0, 0.0 );
//	addVec3( normals, 1.0, 0.0, 0.0 );
//
//	addVec2( texcoords, 0.0, 1.0 );
//	addVec2( texcoords, 1.0, 0.0 );
//	addVec2( texcoords, 0.0, 0.0 );
//
//	vertices.insert( vertices.end(), normals.begin(), normals.end() );
//	vertices.insert( vertices.end(), texcoords.begin(), texcoords.end() );
//
//	int numVertices = 6;
//
//	GLuint vbo = 0;
//	glGenBuffers( 1, &vbo );
//	glBindBuffer( GL_ARRAY_BUFFER, vbo );
//	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( float ), vertices.data(), GL_STATIC_DRAW );
//
//	GLuint planeVao = 0;
//	glGenVertexArrays( 1, &planeVao );
//	glBindVertexArray( planeVao );
//	glEnableVertexAttribArray( 0 );
//	glEnableVertexAttribArray( 1 );
//	glEnableVertexAttribArray( 2 );
//	glBindBuffer( GL_ARRAY_BUFFER, vbo );
//	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
//	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4) );
//	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4 * 2) );
//
//	glBindVertexArray( 0 );
//
//	return Mesh( planeVao, numVertices );
//}
//
//Mesh Mesh::makeGroundPlane( float size, float numTiles )
//{
//	std::vector<float> vertices;
//	std::vector<float> normals;
//	std::vector<float> texcoords;
//
//	//front 1
//	addVec3( vertices, -size, size, 0.0 );
//	addVec3( vertices, size, size, 0.0 );
//	addVec3( vertices, size, -size, 0.0 );
//
//	addVec3( normals, 0.0, 0.0, 1.0 );
//	addVec3( normals, 0.0, 0.0, 1.0 );
//	addVec3( normals, 0.0, 0.0, 1.0 );
//
//	addVec2( texcoords, -numTiles, numTiles );
//	addVec2( texcoords, numTiles, numTiles );
//	addVec2( texcoords, numTiles, -numTiles );
//
//	//front 2
//	addVec3( vertices, -size, size, 0.0 );
//	addVec3( vertices, size, -size, 0.0 );
//	addVec3( vertices, -size, -size, 0.0 );
//
//	addVec3( normals, 0.0, 0.0, 1.0 );
//	addVec3( normals, 0.0, 0.0, 1.0 );
//	addVec3( normals, 0.0, 0.0, 1.0 );
//
//	addVec2( texcoords, -numTiles, numTiles );
//	addVec2( texcoords, numTiles, -numTiles );
//	addVec2( texcoords, -numTiles, -numTiles );
//
//	vertices.insert( vertices.end(), normals.begin(), normals.end() );
//	vertices.insert( vertices.end(), texcoords.begin(), texcoords.end() );
//
//	int numVertices = 6;
//
//	GLuint vbo = 0;
//	glGenBuffers( 1, &vbo );
//	glBindBuffer( GL_ARRAY_BUFFER, vbo );
//	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( float ), vertices.data(), GL_STATIC_DRAW );
//
//	GLuint chessVao = 0;
//	glGenVertexArrays( 1, &chessVao );
//	glBindVertexArray( chessVao );
//	glEnableVertexAttribArray( 0 );
//	glEnableVertexAttribArray( 1 );
//	glEnableVertexAttribArray( 2 );
//	glBindBuffer( GL_ARRAY_BUFFER, vbo );
//	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
//	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4) );
//	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4 * 2) );
//
//	glBindVertexArray( 0 );
//
//	return Mesh( chessVao, numVertices );
//}