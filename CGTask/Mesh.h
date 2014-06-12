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
—одержит VAO дл€ меша (полигональной 3д-модели) и количество вершин.
“акже имеетс€ несколько статических методов дл€ создани€ конкретных мешей.
*/
class Mesh {
public:
	Mesh();
	Mesh( GLuint vao, int numVertices );

	GLuint getVao() { return _vao; }
	int getNumVertices() { return _numVertices; }

	static Mesh makeBuildings(MapWorker* worker);
	static Mesh makeRoads( MapWorker* worker);
	static Mesh makeGround();

protected:
	GLuint _vao;
	int _numVertices;
};