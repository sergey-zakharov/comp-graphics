#include "mapWorker.h"
#include <iostream>

using namespace std;
// Gets all nodes and put them to map by id

MapWorker::MapWorker(XMLDocument& doc) {
	nodes = getNodes(doc);
	ways = getWays(doc, nodes);

	const XMLElement* bounds = doc.FirstChildElement("osm")->FirstChildElement("bounds");

	minlat = bounds->FloatAttribute("minlat");
	maxlat = bounds->FloatAttribute("maxlat");
	minlon = bounds->FloatAttribute("minlon");
	maxlon = bounds->FloatAttribute("maxlon");

	std::cout << "Extracting buildings" << std::endl;
	buildings = extractBuildings(ways);
	std::cout << "Extracting roads" << std::endl;
	roads = extractRoads(ways);

}

double MapWorker::getMinLat() {
	return minlat;
}
double MapWorker::getMaxLat(){
	return maxlat;
}
double MapWorker::getMinLon(){
	return minlon;
}
double MapWorker::getMaxLon(){
	return maxlon;
}
map< long, XMLElement*> MapWorker::getNodes(XMLDocument &doc)
{
	XMLElement* curNode;
	const XMLElement* lastNode;
	long lastNodeId;
	std::map< long, XMLElement*> nodes;

	curNode = doc.FirstChildElement("osm")->FirstChildElement("node");
	lastNode = doc.FirstChildElement("osm")->LastChildElement("node");
	lastNodeId = lastNode->IntAttribute("id");

	do{
		nodes.insert(std::pair<long, XMLElement*>(curNode->IntAttribute("id"), curNode));
		if( curNode != lastNode ) {
			curNode = curNode->NextSiblingElement("node");
		} else break;

	} while( true );
	return nodes;
}

float MapWorker::toFloat(long coord)
{
	return 0;
}

// collect nodes to ways (maps node vector to way id)
map< XMLElement*, vector<long> /*nodes ids*/> MapWorker::getWays(XMLDocument &doc, map< long, XMLElement*> &nodes)
{
	// проходит по путям и вынимает оттуда все <nd ref='###' />, кладя их в вектор

	XMLElement* curWay;
	XMLElement* firstWay;
	const XMLElement* lastWay;

	const char* lastWayId;
	map< XMLElement*, vector<long>> ways;

	
	firstWay = doc.FirstChildElement("osm")->FirstChildElement("way");
	lastWay = doc.FirstChildElement("osm")->LastChildElement("way");
	lastWayId = lastWay->Attribute("id");
	curWay = firstWay;

	// для каждого way собираем nodes в vector
	do {
		XMLElement* curNd;
		XMLElement* firstNd;
		const XMLElement* lastNd;
		firstNd = curWay->FirstChildElement("nd");
		lastNd = curWay->LastChildElement("nd");
		curNd = firstNd;

		vector<long> wayNodes;
		do {
			wayNodes.push_back(curNd->IntAttribute("ref"));

			if( curNd != lastNd ) {
				curNd = curNd->NextSiblingElement();
			} else break;
		} while( true );

		ways.insert(pair<XMLElement*, vector<long>>(curWay, wayNodes));

		if( curWay != lastWay ) {
			curWay = curWay->NextSiblingElement();
		} else break;
	} while( true );

	return ways;
}

// filter ways by buildings
vector<Building*> MapWorker::extractBuildings(map< XMLElement*, vector<long>> &ways)
{
	vector<Building*> buildings;
	cout << "Found: " << buildings.size();
	for( map< XMLElement*, vector<long>>::iterator it = ways.begin(); it != ways.end(); it++ ) {
		//тег с этим атибутом может быть не первым
		
		XMLElement* firstTag = it->first->FirstChildElement("tag");
		XMLElement* lastTag = it->first->LastChildElement("tag");
		XMLElement* curTag = firstTag;
		
		if( firstTag != NULL ) { // хотя бы один тег есть
			do {

				if( strcmp(curTag->Attribute("k"), "building") == 0 ) {
					Building* newBuilding = new Building(*it, this->nodes, minlat, minlon); // может произойти копирование
					buildings.push_back(newBuilding);
					cout << '\r' << "Found: " << buildings.size();
					break;
				}

				if( curTag != lastTag ) {
					curTag = curTag->NextSiblingElement();
				} else break; // мы обработали последний тег, теперь можно выходить
			} while( true );
		}
	}
	cout << endl;
	return buildings;
}

// filter ways by roads
vector<Road* /*way*/> MapWorker::extractRoads(map< XMLElement*, vector<long>> &ways)
{
	vector<Road*> roads;
	cout << "Found: " << buildings.size();
	for( map< XMLElement*, vector<long>>::iterator it = ways.begin(); it != ways.end(); it++ ) {
		//тег с этим атибутом может быть не первым
		XMLElement* firstTag = it->first->FirstChildElement("tag");
		XMLElement* lastTag = it->first->LastChildElement("tag");
		XMLElement* curTag = firstTag;

		if( firstTag != NULL ) { // хотя бы один тег есть

			do {
				if( strcmp(curTag->Attribute("k"), "highway") == 0 ) {
					Road* newRoad = new Road(*it, this->nodes, minlat, minlon); // может произойти копирование
					roads.push_back(newRoad);
					cout << '\r' << "Found: " << buildings.size();
					break;
				}
				if( curTag != lastTag ) {
					curTag = curTag->NextSiblingElement();
				} else break;
			} while( true );
		}
	}
	cout << endl;
	return roads;
}

Building::Building(pair<XMLElement*, vector<long>> buildingWay, map< long, XMLElement*> nodes, double minlat, double minlon)
{
	XMLElement* firstTag = buildingWay.first->FirstChildElement("tag");
	XMLElement* lastTag = buildingWay.first->LastChildElement("tag");
	XMLElement* curTag = firstTag;

	do {

		if( strcmp(curTag->Attribute("k"), "height") == 0 ) {
			heigth = curTag->FloatAttribute("v");
			break;
		}

		if( curTag != lastTag ) {
			curTag = curTag->NextSiblingElement();
		} else break; // мы обработали последний тег, теперь можно выходить
	} while( true );

	for each (long nodeId in buildingWay.second)
	{
		XMLElement* node = nodes[nodeId];
		double lat = node->FloatAttribute("lat");
		double lon = node->FloatAttribute("lon");

		lat = (lat - minlat) * 111000;
		lon = (lon - minlon) * 85395;
		corners.push_back(pair<double, double> (lon, lat));
	}

}

double Building::getHeight() 
{
	return heigth;
}

std::vector<std::pair<double, double>> Building::getCorners()
{
	return corners;
}

Road::Road(pair<XMLElement*, vector<long>> roadWay, map< long, XMLElement*> nodes, double minlat, double minlon)
{
	XMLElement* firstTag = roadWay.first->FirstChildElement("tag");
	XMLElement* lastTag = roadWay.first->LastChildElement("tag");
	XMLElement* curTag = firstTag;
	/*
	do {
		level = 0;
		if( strcmp(curTag->Attribute("k"), "height") == 0 ) {
			heigth = curTag->FloatAttribute("v");
			break;

			// primary, secondary, residential, primary_link

		}

		if( curTag != lastTag ) {
			curTag = curTag->NextSiblingElement();
		} else break; // мы обработали последний тег, теперь можно выходить
	} while( true );
	*/
	for each (long nodeId in roadWay.second)
	{
		XMLElement* node = nodes[nodeId];
		double lat = node->FloatAttribute("lat");
		double lon = node->FloatAttribute("lon");

		lat = (lat - minlat) * 111000;
		lon = (lon - minlon) * 85395;
		vertexes.push_back(pair<double, double>(lon, lat));
	}

}

double Road::getLevel()
{
	return level;
}

std::vector<std::pair<double, double>> Road::getVertexes()
{
	return vertexes;
}