#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "Vec3.h"

#ifndef SCHAAL
#define SCHAAL 2
#endif

#define AfstandTussenPuntenmm	30.0
#define ScanGrotemm				90.0

#define CONISCHEHOEK		    12
#define STAPGROTE 		  		 0.5

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

					void operator=(const HoogtemapValueProxy& v){
						waarde = v.waarde;
					}

					friend std::ofstream& operator<<(std::ofstream& output, const HoogtemapValueProxy& D){
						output << D.waarde;
						return output;
					}

					double operator-(const HoogtemapValueProxy& HMVP){
						return this->waarde - HMVP.value;
					}
					double operator-(const double& v){
						return this->waarde - v;
					}

					HoogtemapValueProxy& operator-=(const double& v){
						this->waarde -= v;
						return *this;
					}

					operator double() const { return waarde; }
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
	Hoogtemap(unsigned int MatrixGrote, unsigned int PuntenPerLijn);
	~Hoogtemap();

	double MaakConischGat(double Radius, double ConischeHoek = CONISCHEHOEK, double StapGrote = STAPGROTE);

	unsigned int GetMatrixGrote(){return MatrixGrote;}

	// interpoleert scanpunten
	void InterpoleerHoogtemap();

private:
	// bevat de gescande hoogtemap waardes
	HoogtemapArrayProxy<double>::HoogtemapValueProxy** matrix;
	unsigned int MatrixGrote;
	unsigned int PuntenPerLijn;
	
	// berekend of punt binnen de Frees cirkel valt
	bool IsInCirkel(Vec3<double> P, Vec3<double> C, double R);

	
	// verkleint de hoogtemap tot grote: radius
	void VerkleinHoogtemap(Vec3<double> MiddelPunt, double Radius, double& Hoogstepunt);
	// Maakt conisch gat in de hoogtemap
	void initCirkel(Vec3<double> C, double Radius, double StapGrote = STAPGROTE);
};