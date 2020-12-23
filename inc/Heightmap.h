#pragma once

#include <string>
#include <iostream>
#include <fstream>

#define ScanResolutie .5

#define AfstandTussenPuntenmm	30.0
#define	AfstandTussenLijnenmm	30.0
#define ScanHoogtemm			90.0
#define ScanBreedtemm			90.0


class Hoogtemap
{
public:
	template<typename type>
	class HoogtemapArrayProxy {
		public:
			class HoogtemapValueProxy {
				private:
					type waarde = 0;
				public:
					void operator=(const type& v){
						waarde = v;
					}

					friend std::ofstream& operator<<(std::ofstream& output, const HoogtemapValueProxy &D){
						output << D.waarde;
						return output;
					}

					double operator-(const HoogtemapValueProxy& HMVP){
						return this->value - HMVP.value;
					}
					double operator-(const double& v){
						return this->value - v;
					}

					operator double() const { return value; }
			};

			HoogtemapArrayProxy(HoogtemapValueProxy* _array) : _array(_array) { }

			HoogtemapValueProxy& operator[](unsigned int index) {
				return _array[index];
			}
		
		private:
			HoogtemapValueProxy* _array;
	};

	HoogtemapArrayProxy<double> operator[](unsigned int index) {
		return HoogtemapArrayProxy<double>(matrix[index]);
	}

	// Maakt een hoogtemap aan.
	Hoogtemap(unsigned int hoogte, unsigned int breedte);
	~Hoogtemap();

	// Schrijft de hoogtemap naar bestand genaamd "filename.dat"
	void SlaHoogtemapOp(std::string filename);

private:
	// bevat de gescande hoogtemap waardes
	HoogtemapArrayProxy<double>::HoogtemapValueProxy** matrix;
	unsigned int hoogte, breedte;

};