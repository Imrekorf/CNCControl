#pragma once

#include <string>
#include <iostream>
#include <fstream>

#define SCHAAL 2

#define AfstandTussenPuntenmm	30.0
#define ScanGrotemm				90.0

#define CONISCHEHOEK		    12
#define STAPGROTE 		  		 0.5

class Hoogtemap
{
private:
	struct point {
		double x;
		double y;
	};

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
						return this->value - HMVP.value;
					}
					double operator-(const double& v){
						return this->value - v;
					}

					HoogtemapValueProxy& operator-=(const double& v){
						return this->value -= v;
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
	Hoogtemap(unsigned int MatrixGrote);
	~Hoogtemap();

	void MaakConischGat(double Radius, double ConischeHoek = CONISCHEHOEK, double StapGrote = STAPGROTE);

	unsigned int GetMatrixGrote(){return MatrixGrote;}

private:
	// bevat de gescande hoogtemap waardes
	HoogtemapArrayProxy<double>::HoogtemapValueProxy** matrix;
	unsigned int MatrixGrote;

	// berekend of punt binnen de Frees cirkel valt
	bool IsInCirkel(point P, point C, double R);

	// interpoleert scanpunten
	void InterpoleerHoogtemap();
	// verkleint de hoogtemap tot grote: radius
	void VerkleinHoogtemap(point MiddelPunt, double Radius, double& Hoogstepunt);
	// Maakt conisch gat in de hoogtemap
	void initCirkel(point C, double Radius, double StapGrote = STAPGROTE);
};