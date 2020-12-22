#pragma once

#include <string>
#include <iostream>
#include <fstream>

#define Resolution .5

#define DistanceBetweenPointsmm	30.0
#define	DistanceBetweenLinesmm	30.0
#define ScanHeightmm			90.0
#define ScanWidthmm				90.0


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

					double operator-(const HeightMapValueProxy& HMVP){
						return this->value - HMVP.value;
					}
					double operator-(const double& v){
						return this->value - v;
					}

					operator double() const { return value; }
			};

			HeightMapArrayProxy(HeightMapValueProxy* _array) : _array(_array) { }

			HeightMapValueProxy& operator[](unsigned int index) {
				return _array[index];
			}
		
		private:
			HeightMapValueProxy* _array;
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