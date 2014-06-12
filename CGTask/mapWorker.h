#pragma once

#include <map>
#include <vector>
#include <tinyxml2\tinyxml2.h>

using namespace tinyxml2;


class Building
{
private:
	double heigth;
	std::vector<std::pair<double, double>> corners;
public:
	Building(std::pair<XMLElement*, std::vector<long>> way, std::map< long, XMLElement*> nodes,
			 double minlat, double minlon);
	double getHeight();
	std::vector<std::pair<double, double>> getCorners();
};

class Road
{
private:
	double level;
	std::vector<std::pair<double, double>> vertexes;
public:
	Road(std::pair<XMLElement*, std::vector<long>> way, std::map< long, XMLElement*> nodes,
			 double minlat, double minlon);
	double getLevel();
	std::vector<std::pair<double, double>> getVertexes();
};

class MapWorker
{
	
private:
	std::map< XMLElement*, std::vector<long> /*nodes ids*/> ways;
	std::map< long, XMLElement*> nodes;
	double minlat;
	double maxlat;
	double minlon;
	double maxlon;
public:
	std::vector<Building*> buildings;
	std::vector<Road*> roads;
	
public:
	MapWorker(XMLDocument& doc);
	float toFloat(long coord);
	double getMinLat();
	double getMaxLat();
	double getMinLon();
	double getMaxLon();
	friend class Building;
private:
	std::map< long /*node id*/, XMLElement*> getNodes(XMLDocument &doc);
	std::map< XMLElement*, std::vector<long> /*nodes ids*/> getWays(XMLDocument &doc, std::map< long, XMLElement*> &nodes);

	std::vector<Building*> extractBuildings(std::map< XMLElement*, std::vector<long>> &ways);
	std::vector<Road* /*way*/> extractRoads(std::map< XMLElement*, std::vector<long>> &ways);

};

