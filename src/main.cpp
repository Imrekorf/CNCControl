#include <wiringPi.h>
#include <iostream>
#include <fstream>

#include "Sensor.h"
#include "Vec3.h"

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

void WriteHeightMapToFile(double** Matrix, int ScanLines, int PointsPerLine){
	// write data to file and free heightmapmatrix memory
	for(int i = 0; i < ScanLines; i++){
		for(int j = 0; j < PointsPerLine; j++){
			HeightMap << Matrix[i][j] << " ";
		}
		HeightMap << std::endl;
		delete Matrix[i];
	}
	delete Matrix;
}

// get difference based relative to leveledheight and adds that difference to relativeheight. Store Relativeheight to heightmapmatrix.
double GetSensorDiff(Sensor &S, double LeveledHeight, double &RelativeHeight, double &HeightMapMatrix){
	double SensorValueDiff = S.GetDistance() - LeveledHeight;	// get difference based to leveled height
	std::cout << "SensorDiff: " << SensorValueDiff << std::endl;
	// clear interference
	// only note height difference if it is bigger than 0.1
	if(fabs(SensorValueDiff) > 0.1){
		RelativeHeight += SensorValueDiff;
	}
	// Store RelativeHeight
	HeightMapMatrix = RelativeHeight;
	return SensorValueDiff;
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
	for(int j = 0; j < ScanLines; j++){
		HeightMapMatrix[j] = new double[PointsPerLine];
	}

	// scan Area
	for(int i = 0; i < ScanLines; i++){
		// zigzagging logic
		int j = i % 2 ? PointsPerLine-1 : 0;
		int increase = i % 2 ? -1 : 1;

		for(;i % 2 ? j > 0 : j < PointsPerLine - 1; j += increase){
			// get Height Difference and store that to HeightMapMatrix
			double SensorValueDiff = GetSensorDiff(S, LeveledHeight, RelativeHeight, HeightMapMatrix[i][j]);

			// move sensorHead
			MoveTo(Vec3<double>(increase * DistanceBetweenPointsmm, 0, SensorValueDiff));
		}
		// currently at last point
		double SensorValueDiff = GetSensorDiff(S, LeveledHeight, RelativeHeight, HeightMapMatrix[i][j]);

		// move up, to the side and down
		MoveTo(Vec3<double>(0, 0, StartingHeightFromFreesTop - RelativeHeight));
		MoveTo(Vec3<double>(0, DistanceBetweenLinesmm, 0));
		if(i+1 < ScanLines){
			MoveTo(Vec3<double>(0, 0, -(StartingHeightFromFreesTop - RelativeHeight)));
		}
	}
	
	WriteHeightMapToFile(HeightMapMatrix, ScanLines, PointsPerLine);

	file1.close();
	HeightMap.close();

	std::cout << "job's done" << std::endl;


	return 0;
}