#include <wiringPi.h>
#include <iostream>
#include <fstream>

#include "Heightmap.h"
#include "Sensor.h"
#include "Frees.h"
#include "Vec3.h"

// used for debugging
int main(int argc, char** argv);


int main(int argc, char** argv)
{	
	// calculate needed points for heightmatrix
	unsigned int PointsPerLine = ScanWidthmm / DistanceBetweenPointsmm + 1;
	unsigned int ScanLines = ScanHeightmm / DistanceBetweenLinesmm + 1;

	// initialisation
	if(wiringPiSetup() == -1){
		std::cerr << "unable to initialize wiringPi" << std::endl;
		return 1;
	}


	Sensor S;
	Frees F({0, 0, 0});
	Heightmap heightmap(ScanLines, PointsPerLine);

	// make sure wiringPi and objects are instantiated
	delay(15);
	
	F.SendGCode("F500");

	while(true){
		F.MoveTo({0, 0, 0});
		std::cout << "new command" << std::endl;
		F.MoveTo({50, 50, 0});
	}



	/*
	S.LevelSensor(F);

	// scan Area
	for(unsigned int i = 0; i < ScanLines; i++){
		// zigzagging logic
		unsigned int j = i % 2 ? PointsPerLine-1 : 0;
		int increase = i % 2 ? -1 : 1;
		double SensorValueDiff = 0;
		for(;i % 2 ? j > 0 : j < PointsPerLine - 1; j += increase){
			// measure the height and store to heightmap
			heightmap[i][j] = S.MeasureHeight(SensorValueDiff);

			// move sensor head
			F.Move({0, (double)increase * DistanceBetweenPointsmm, SensorValueDiff});
		}
		// currently at last point of line
		// std::cout << "finishing line: " << std::endl;
		heightmap[i][j] = S.MeasureHeight(SensorValueDiff);

		// move up, to the side and down ( if there is a next line)
		double Zheight = F.GetPosition().Z();
		F.Move({0, 0, -1 * Zheight});
		if(i+1 < ScanLines){
			F.Move({DistanceBetweenLinesmm, 0, 0});
			F.Move({0, 0, Zheight + SensorValueDiff});
		}
	}
	*/

	heightmap.WriteHeightMap("HeightmapTest.dat");

	std::cout << "job's done" << std::endl;


	return 0;
}