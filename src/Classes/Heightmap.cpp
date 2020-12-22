#include "Heightmap.h"
#include "Vec3.h"

struct Line{
	double RC;
	double B;

	Line(Vec3<double> P1, Vec3<double> P2){
		RC = (P2.Y() - P1.Y()) / (P2.X() - P1.X());
		B = P1.Y() - (RC * P1.X());
	}

	double F(double X){
		return RC * X + B;
	}
};


Heightmap::Heightmap(unsigned int height, unsigned int width)
{
	this->height = height;
	this->width = width;
	matrix = new HeightMapArrayProxy<double>::HeightMapValueProxy*[height];
	for(unsigned int j = 0; j < height; j++){
		matrix[j] = new HeightMapArrayProxy<double>::HeightMapValueProxy[width];
	}

	matrix[0][0] = 0;		 matrix[0][1] = 2.58738; matrix[0][2] = 5.19097; matrix[0][3] = 7.82596;
	matrix[1][0] = 0.129456; matrix[1][1] = 2.71583; matrix[1][2] = 5.33543; matrix[1][3] = 7.82596;
	matrix[2][0] = 0.129456; matrix[2][1] = 2.72131; matrix[2][2] = 5.33440; matrix[2][3] = 7.97176;
	matrix[3][0] = 0.265017; matrix[3][1] = 2.87979; matrix[3][2] = 5.47966; matrix[3][3] = 7.97176;
}

Heightmap::~Heightmap()
{
	for(unsigned int j = 0; j < height; j++){
		delete matrix[j];
	}
	delete matrix;
}

void Heightmap::WriteHeightMap(std::string filename){
	std::ofstream HeightMap;
	HeightMap.open(filename);

	if (!HeightMap.is_open()) 
  	{ 
  		std::cout << "error opening " << filename << std::endl;
		return;
  	}

	double** HeightMatrixTemp = new double*[(int)(ScanHeightmm/Resolution)];
	for(unsigned int i = 0; i < ScanHeightmm/Resolution; i++){
		HeightMatrixTemp[i] = new double[(int)(ScanWidthmm/Resolution)];
	}
	
	// write data to file and free heightmapmatrix memory
	for(unsigned int i = 0; i < height-1; i++){
		for(unsigned int j = 0; j < width-1; j++){
			Line HL(
				{DistanceBetweenLinesmm*i, matrix[i][j], 0}, 
				{DistanceBetweenLinesmm*(i+1), matrix[i+1][j], 0}
			);
			Line HR(
				{DistanceBetweenLinesmm*i, matrix[i][j+1], 0}, 
				{DistanceBetweenLinesmm*(i+1), matrix[i+1][j+1], 0}
			);
			for(double k = 0; k < DistanceBetweenLinesmm; k+=Resolution){
				Line W(
					{DistanceBetweenPointsmm*j, HL.F(k + i * DistanceBetweenLinesmm), 0}, 
					{DistanceBetweenPointsmm*(j+1), HR.F(k + i * DistanceBetweenLinesmm), 0}
				);
				
				for(double l = 0; l < DistanceBetweenPointsmm; l+=Resolution){
					unsigned int i_height = i*(DistanceBetweenLinesmm/Resolution) + (k/Resolution);
					unsigned int i_width = j*(DistanceBetweenLinesmm/Resolution) + (l/Resolution);
					HeightMatrixTemp[i_height][i_width] = W.F(l + j * DistanceBetweenLinesmm);
				}
			}
		}
	}

	
	for(unsigned int i = 0; i < ScanHeightmm/Resolution; i++){
		for(unsigned int j = 0; j < ScanWidthmm/Resolution; j++){
			HeightMap << HeightMatrixTemp[i][j] << " ";
		}
		delete HeightMatrixTemp[i];
		HeightMap << std::endl;
	}
	delete HeightMatrixTemp;

	HeightMap.close();
}