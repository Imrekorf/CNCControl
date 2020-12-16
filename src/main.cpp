#include <wiringPi.h>
#include <iostream>
#include <fstream>

#include "Manager.h"
#include "JobExecuter.h"

#define DistanceBetweenPointsmm	30
#define	DistanceBetweenLinesmm	30
#define ScanHeightmm			90
#define ScanWidthmm				90

// used for debugging
int main(int argc, char** argv);

std::ofstream file1;
std::ofstream HeightMap;

void MoveTo(Vec3<double> P){
	std::string Gcode;
	Gcode += "G1";
	if(P.Y())
		Gcode += "X" + std::to_string(P.Y());
	if(P.X())
		Gcode += "Y" + std::to_string(P.X());
	if(P.Z())
		Gcode += "Z" + std::to_string(P.Z());

	file1 << Gcode << std::endl;
	std::cout << Gcode << std::endl << "Finished? ";
	std::cin.get();
}

void SendGCode(std::string gcode){
	std::cout << "Gcode: " << gcode << std::endl;
	file1 << gcode << std::endl;
	std::cout << "Finished? ";
	std::cin.get();
}


int main(int argc, char** argv)
{	
	Sensor S;
	wiringPiSetup();

	file1.open("temp.tap");
	HeightMap.open("Heightmap.dat");
	if (!file1.is_open() || !HeightMap.is_open()) 
  	{ 
  	  std::cout << "error opening temp.tap or Heightmap.dat" << std::endl; 
  	  return 1; 
  	} 

	//S.StartPosFrees({0, 0, 0});
	//Frees F({0, 0, 0});
	//S.SetFrees(F);

	// make sure wiringPi and objects are instantiated
	delay(15);

	// put CNC mill to relative positioning mode
	SendGCode("G91");

	// Move sensor down till it touches the surface
	double SensorValue = 0;
	double StartingHeightFromFreesTop = 0;
	
	while(SensorValue < 2){
		MoveTo(Vec3<double>(0, 0, -15));
		SensorValue = S.GetDistance(); 
		std::cout << "Distance: " << SensorValue << std::endl;
		StartingHeightFromFreesTop += 15;
	}
	
	// move CNC head down so that sensor reads 15.0
	StartingHeightFromFreesTop += 15.0 - SensorValue;
	MoveTo(Vec3<double>(0, 0, -1 * (15.0 - SensorValue)));
	
	// debugging
	std::cout << "HeightToTop: " << StartingHeightFromFreesTop << std::endl;

	// assume sensor is now in position

	// calculate needed points for heightmatrix
	int PointsPerLine = ScanWidthmm / DistanceBetweenPointsmm + 1;
	int ScanLines = ScanHeightmm / DistanceBetweenLinesmm + 1;

	// HeightReference at first height scan = 0
	double RelativeHeight = 0;
	// keep track of height when sensor was leveled
	double LeveledHeight = S.GetDistance();


	// create heightmap matrix
	double** HeightMapMatrix = new double*[ScanLines];
	for(int j = 0; j < PointsPerLine; j++){
		HeightMapMatrix[j] = new double[PointsPerLine];
	}

	// scan Area
	for(int i = 0; i < ScanLines; i++){
		// zigzagging logic
		int j = i % 2 ? PointsPerLine-1 : 0;
		int increase = i % 2 ? -1 : 1;



		for(;i % 2 ? j >= 0 : j < PointsPerLine; j += increase){
			// tell user to input GCode
			if(i % 2 ? j != PointsPerLine-1 : j != 0){
				MoveTo(Vec3<double>(increase * DistanceBetweenPointsmm, 0, 0));
			}

			// get new height value
			SensorValue = S.GetDistance();
			double SensorValueDiff = SensorValue - LeveledHeight;	// get difference based to leveled height
			std::cout << "SensorDiff: " << SensorValueDiff << std::endl;
			// clear interference
			// only note height difference if it is bigger than 0.1
			if(fabs(SensorValueDiff) > 0.1){
				RelativeHeight += SensorValueDiff;

				// Move Z back to LeveledHeight
				MoveTo(Vec3<double>(0, 0, SensorValueDiff));
			}
			// Keep track of how much it has moved relative to position 0
			// Store RelativeHeight
			HeightMapMatrix[i][j] = RelativeHeight;
			//HeightMap << RelativeHeight << " ";
		}
		//HeightMap << std::endl;

		MoveTo(Vec3<double>(0, 0, StartingHeightFromFreesTop - RelativeHeight));
		MoveTo(Vec3<double>(0, DistanceBetweenLinesmm, 0));
		if(i+1 < ScanLines){
			MoveTo(Vec3<double>(0, 0, -(StartingHeightFromFreesTop - RelativeHeight)));
		}
		//MoveTo(Vec3<double>(j * DistanceBetweenPointsmm, (i - 1) * DistanceBetweenLinesmm, 16));

	}
	
	// write data to file and free heightmapmatrix memory
	for(int i = 0; i < ScanLines; i++){
		for(int j = 0; j < PointsPerLine; j++){
			HeightMap << HeightMapMatrix[i][j] << " ";
		}
		HeightMap << std::endl;
		delete HeightMapMatrix[i];
	}
	delete HeightMapMatrix;

	file1.close();
	HeightMap.close();

	std::cout << "job's done" << std::endl;


	return 0;
}