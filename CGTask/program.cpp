#include <tinyxml2\tinyxml2.h>
#include <cstdio>
#include <map>

#include "Application.h"
#include "mapWorker.h"
#include "geometry.h"
#include "parse.h"

using namespace tinyxml2;
int main()
{
	Application app;
	
	app.initContext();
	app.initGL();
	app.makeScene();
	app.run();	

// testing tesselation
	/*vector<pair<float, float>> input;
	input.push_back(pair<float, float>(0., 0.));
	input.push_back(pair<float, float>(1., 0.));
	input.push_back(pair<float, float>(2., 1.));
	input.push_back(pair<float, float>(1., 2.));
	input.push_back(pair<float, float>(0., 1.));*/
	//input.push_back(pair<float, float>(0., 0.));

	//Options options;
	//string filename = "../example.bdm";//argv[options.fileindex]; //input bdm file name; 	
	//Polygon poly( filename, options.parsebdm );

	/*Polygon poly(input);
	poly.triangulation();
	vector<vector<pair<float, float>>> result = poly.saveIntoVector();

	for( int i = 0; i < result.size(); i++ ) {
		for( int j = 0; j < result[i].size(); j++ ) {
			cout << "<" << result[i][j].first << " " << result[i][j].second << ">";
		}
		cout << endl;
	}
	system( "pause" );*/

	//poly.triangulation();                    //main triangulation function

	//output results;   
	//poly.saveAsShowme();
	//if( options.showme ) poly.saveAsShowme();
	//if( options.metapost ) poly.saveAsMetaPost();
	//if( options.tecplot ) poly.saveAsTecplot();

	return 0;
}

