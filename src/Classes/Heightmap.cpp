#include "Heightmap.h"
#include "Vec3.h"

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


Hoogtemap::Hoogtemap(unsigned int hoogte, unsigned int breedte)
{
	this->hoogte = hoogte;
	this->breedte = breedte;
	// Maak de matrix aan die de waarde van de hoogtemap bijhoudt aan.
	matrix = new HoogtemapArrayProxy<double>::HoogtemapValueProxy*[hoogte];
	for(unsigned int j = 0; j < hoogte; j++){
		matrix[j] = new HoogtemapArrayProxy<double>::HoogtemapValueProxy[breedte];
	}
}

Hoogtemap::~Hoogtemap()
{	
	// zet het gebruikte geheugen vrij.
	for(unsigned int j = 0; j < hoogte; j++){
		delete matrix[j];
	}
	delete matrix;
}

void Hoogtemap::SlaHoogtemapOp(std::string filename){
	// open een filestream voor de hoogtemap.
	std::ofstream HeightMap;
	HeightMap.open(filename);

	if (!HeightMap.is_open()) 
  	{ 
  		std::cout << "error opening " << filename << std::endl;
		return;
  	}

	double** HoogtemapMatrixVerGroot = new double*[(int)(ScanHoogtemm/ScanResolutie)];
	for(unsigned int i = 0; i < ScanHoogtemm/ScanResolutie; i++){
		HoogtemapMatrixVerGroot[i] = new double[(int)(ScanBreedtemm/ScanResolutie)];
	}
	
	// write data to file and free heightmapmatrix memory
	for(unsigned int i = 0; i < hoogte-1; i++){
		for(unsigned int j = 0; j < breedte-1; j++){
			// Verticaal Links
			Lijn HL(
				{AfstandTussenLijnenmm*i, matrix[i][j], 0}, 
				{AfstandTussenLijnenmm*(i+1), matrix[i+1][j], 0}
			);
			// Verticaal Rechts
			Lijn HR(
				{AfstandTussenLijnenmm*i, matrix[i][j+1], 0}, 
				{AfstandTussenLijnenmm*(i+1), matrix[i+1][j+1], 0}
			);
			// maak een lijn tussen Verticaal Links en Verticaal. 
			for(double k = 0; k < AfstandTussenLijnenmm; k+=ScanResolutie){
				Lijn W(
					{AfstandTussenPuntenmm*j, HL.F(k + i * AfstandTussenLijnenmm), 0}, 
					{AfstandTussenPuntenmm*(j+1), HR.F(k + i * AfstandTussenLijnenmm), 0}
				);
				// bereken de punten over de horizontale lijn.
				for(double l = 0; l < AfstandTussenPuntenmm; l+=ScanResolutie){
					unsigned int i_height = i*(AfstandTussenLijnenmm/ScanResolutie) + (k/ScanResolutie);
					unsigned int i_width = j*(AfstandTussenLijnenmm/ScanResolutie) + (l/ScanResolutie);
					HoogtemapMatrixVerGroot[i_height][i_width] = W.F(l + j * AfstandTussenLijnenmm);
				}
			}
		}
	}

	// schrijf de berekende waardes naar het bestand en verwijder vervolgens die regel in de matrix.
	for(unsigned int i = 0; i < ScanHoogtemm/ScanResolutie; i++){
		for(unsigned int j = 0; j < ScanBreedtemm/ScanResolutie; j++){
			HeightMap << HoogtemapMatrixVerGroot[i][j] << " ";
		}
		delete HoogtemapMatrixVerGroot[i];
		HeightMap << std::endl;
	}
	delete HoogtemapMatrixVerGroot;

	// sluit de filestream
	HeightMap.close();
}