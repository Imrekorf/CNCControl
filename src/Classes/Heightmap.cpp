#include "Heightmap.h"
#include "Vec3.h"

#ifndef M_PI
#define M_PI 3.14159
#endif
#define degreesToRadians(angleDegrees) ((angleDegrees) * M_PI / 180.0)
#define radiansToDegrees(angleRadians) ((angleRadians) * 180.0 / M_PI)

// Datatype die een lijn representeerd
struct Lijn{
	double RC;
	double B;

	// bereken lijn tussen twee punten.
	Lijn(Vec3<double> P1, Vec3<double> P2){
		RC = (P2.Y() - P1.Y()) / (P2.X() - P1.X());
		B = P1.Y() - (RC * P1.X());
	}

	double F(double X){
		return RC * X + B;
	}
};


Hoogtemap::Hoogtemap(unsigned int MatrixGrote, unsigned int PuntenPerLijn)
{
	this->MatrixGrote = MatrixGrote;
	this->PuntenPerLijn = PuntenPerLijn;
	// Maak de matrix aan die de waarde van de hoogtemap bijhoudt aan.
	matrix = new HoogtemapArrayProxy<double>::HoogtemapValueProxy*[MatrixGrote];
	for(unsigned int j = 0; j < MatrixGrote; j++){
		matrix[j] = new HoogtemapArrayProxy<double>::HoogtemapValueProxy[MatrixGrote];
	}
}

Hoogtemap::~Hoogtemap()
{	
	// zet het gebruikte geheugen vrij.
	for(unsigned int j = 0; j < MatrixGrote; j++){
		delete matrix[j];
	}
	delete matrix;
}

void Hoogtemap::InterpoleerHoogtemap(){	
	// write data to file and free heightmapmatrix memory
	for(unsigned int i = 0; i < PuntenPerLijn-1; i++){
		for(unsigned int j = 0; j < PuntenPerLijn-1; j++){
			// Verticaal Links
			Lijn HL(
				{AfstandTussenPuntenmm*i, matrix[(int)(i * AfstandTussenPuntenmm * SCHAAL)][(int)(j * AfstandTussenPuntenmm * SCHAAL)], 0}, 
				{AfstandTussenPuntenmm*(i+1), matrix[(int)((i+1) * AfstandTussenPuntenmm * SCHAAL)][(int)(j * AfstandTussenPuntenmm * SCHAAL)], 0}
			);
			// Verticaal Rechts
			Lijn HR(
				{AfstandTussenPuntenmm*i, matrix[(int)(i * AfstandTussenPuntenmm * SCHAAL)][(int)((j+1) * AfstandTussenPuntenmm * SCHAAL)], 0}, 
				{AfstandTussenPuntenmm*(i+1), matrix[(int)((i+1) * AfstandTussenPuntenmm * SCHAAL)][(int)((j+1) * AfstandTussenPuntenmm * SCHAAL)], 0}
			);
			// maak een lijn tussen Verticaal Links en Verticaal. 
			for(double k = 0; k < AfstandTussenPuntenmm; k+=(1.0/SCHAAL)){
				Lijn W(
					{AfstandTussenPuntenmm*j, HL.F(k + i * AfstandTussenPuntenmm), 0}, 
					{AfstandTussenPuntenmm*(j+1), HR.F(k + i * AfstandTussenPuntenmm), 0}
				);
				// bereken de punten over de horizontale lijn.
				for(double l = 0; l < AfstandTussenPuntenmm; l+=(1.0/SCHAAL)){
					unsigned int i_height = i*(AfstandTussenPuntenmm * SCHAAL) + (k * SCHAAL);
					unsigned int i_width = j*(AfstandTussenPuntenmm * SCHAAL) + (l * SCHAAL);
					matrix[i_height][i_width] = W.F(l + j * AfstandTussenPuntenmm);
				}
			}
		}
	}
}

void Hoogtemap::VerkleinHoogtemap(Vec3<double> MiddelPunt, double Radius, double& Hoogstepunt){
	HoogtemapArrayProxy<double>::HoogtemapValueProxy** KleineHoogtemap = new HoogtemapArrayProxy<double>::HoogtemapValueProxy*[(int)(Radius * SCHAAL)];
	for (unsigned int i = 0; i < (unsigned int)(Radius * 2); i++)
	{
		KleineHoogtemap[i] = new HoogtemapArrayProxy<double>::HoogtemapValueProxy[(int)(Radius * 2)];
	}
	
	for(unsigned int y = (MiddelPunt.Y() - Radius); y < MiddelPunt.Y() + Radius; y++){
		for(unsigned int x = (MiddelPunt.X() - Radius); x < (MiddelPunt.X() + Radius); x++){
			if(matrix[y][x] > Hoogstepunt){
				Hoogstepunt = matrix[y][x];
			}
			KleineHoogtemap[y-(unsigned int)(MiddelPunt.Y() - Radius)][x-(unsigned int)(MiddelPunt.X() - Radius)] = matrix[y][x];
		}
	}

	// zet geheugen van grote matrix vrij.
	for(unsigned int j = 0; j < MatrixGrote; j++){
		delete matrix[j];
	}
	delete matrix;

	// update interne hoogtemap
	matrix = KleineHoogtemap;
	MatrixGrote = Radius * 2;
}

bool Hoogtemap::IsInCirkel(Vec3<double> P, Vec3<double> C, double R){
	double dx = C.X() - P.X();
	double dy = C.Y() - P.Y();
	return (dx*dx + dy*dy) <= (R*R);
}

void Hoogtemap::initCirkel(Vec3<double> C, double Radius, double StapGrote){
	Vec3<double> P = {0, 0, 0};
	for(unsigned int y = 0; y < MatrixGrote; y++){
		for(unsigned int x = 0; x < MatrixGrote; x++){
			P = {(float)x, (float)y, 0};
			if(IsInCirkel(P, C, Radius)){
				matrix[y][x] -= StapGrote;
			}
		}
	}
}

double Hoogtemap::MaakConischGat(double Radius, double ConischeHoek, double StapGrote){
	InterpoleerHoogtemap();

	Vec3<double> MiddelPunt = {(double)(MatrixGrote/2), (double)(MatrixGrote/2), 0};
	double Hoogstepunt = -20000; // lage waarde zodat er altijd een hogere waarde kan worden gevonden in de hoogtemap.
	VerkleinHoogtemap(MiddelPunt, Radius * SCHAAL, Hoogstepunt);

	MiddelPunt = {(double)(MatrixGrote/2), (double)(MatrixGrote/2), 0};
	
	double ConischeDiepte = std::tan(degreesToRadians(ConischeHoek)) * Radius;

	for(int i = 0; i <= std::ceil(ConischeDiepte/StapGrote); i++){
		float newradius = (ConischeDiepte-StapGrote*i)/std::tan(degreesToRadians(ConischeHoek));
		initCirkel(MiddelPunt, (double)newradius * SCHAAL, StapGrote);
	}
	
	return Hoogstepunt;
}