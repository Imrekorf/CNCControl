#pragma once

#include <string>
#include <iostream>
#include <fstream>

class Heightmap
{
public:
	template<typename type>
	class HeightMapArrayProxy {
		public:
			class HeightMapValueProxy {
				private:
					type value = 0;
				public:
					void operator=(const type& v){
						value = v;
					}

					friend std::ofstream& operator<<(std::ofstream& output, const HeightMapValueProxy &D){
						output << D.value;
						return output;
					}

					friend std::ostream& operator<<(std::ostream& output, const HeightMapValueProxy &D){
						output << D.value;
						return output;
					}

					type get(){
						return value;
					}
			};

			HeightMapArrayProxy(HeightMapValueProxy* _array) : _array(_array) { }

			HeightMapValueProxy& operator[](unsigned int index) {
				return &_array[index];
			}
		
		private:
			type* _array;
	};

	HeightMapArrayProxy<double> operator[](unsigned int index) {
		return HeightMapArrayProxy<double>(matrix[index]);
	}

	Heightmap(unsigned int height, unsigned int width);
	~Heightmap();

	void WriteHeightMap(std::string filename);

	private:
		HeightMapArrayProxy<double>::HeightMapValueProxy** matrix;
		unsigned int height, width;

};

Heightmap::Heightmap(unsigned int height, unsigned int width)
{
	this->height = height;
	this->width = width;
	matrix = new HeightMapArrayProxy<double>::HeightMapValueProxy*[height];
	for(int j = 0; j < height; j++){
		matrix[j] = new HeightMapArrayProxy<double>::HeightMapValueProxy[width];
	}
}

Heightmap::~Heightmap()
{
	for(int j = 0; j < height; j++){
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
  	}

	// write data to file and free heightmapmatrix memory
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			HeightMap << matrix[i][j] << " ";
		}
		HeightMap << std::endl;
	}
}