#include "Manager.h"

#include <iostream>
#include <fstream>
#include <sstream>

// Feedrate = mm per minute
// Feedrate / 60 = mm per second
// FRS = mm per second

// TimePerRow = 3s
// Distance = 300mm
// ((Distance / (Feedrate / 60)) * 1000) / Readings = TimePerRead (in milliseconds)
// Simplified: (300000 / (Feedrate / 60)) / Readings = TimePerRead

// (Distance * 60) / TimePerRow = FeedRate
// Simplified: 18000 / TimePerRow = FeedRate

#define Feedrate   6000.0
#define Distance 	  5.0
//! calculate this before hand to make sure timeperrow and timeperread are whole numbers !//
const unsigned int TimePerRow  = (Distance / (Feedrate / 60.0)) * 1000.0;
const unsigned int TimePerRead = (double)TimePerRow / (double)HEIGHTMAPSIZE;

double 			Manager::heightmap[HEIGHTMAPSIZE][HEIGHTMAPSIZE];
const char 		Manager::filename[] 	= "heightmap.dat";
unsigned int 	Manager::rowcounter 	= 0;
unsigned int 	Manager::columncounter 	= 0;
bool			Manager::rowfinished 	= false;
unsigned int 	Manager::RowJobID		= 0;
std::mutex 		Manager::HeightMutex;
JobExecuter*	Manager::JE;
Frees 			Manager::F;
Sensor 			Manager::S;

Manager::Manager(JobExecuter* JE, Vec3<double> StartPos)
{
	this->JE = JE;
	F.StartPos(StartPos);
	F.SetFeedrate(6000);	// 100mm per second
	S.StartPosFrees(StartPos);
	S.SetFrees(F);
	// read data every 0.5mm: .05 second = 50milliseconds
	// wait 1 second for sensor to be up and running
	J = new Job(1000, TimePerRow, ControlCNC);
	JE->AddJob(J);	// start job

}

Manager::~Manager()
{
}

void Manager::StoreHeight(JobBase* thisjob){
	double distance = S.GetADCAverage();	// save distance now, this will remove stress on the heightmutex lock (GetDistance also had a mutex)
	
	HeightMutex.lock();
	heightmap[rowcounter][columncounter] = distance;
	columncounter += rowcounter % 2 ? -1 : 1 ;	// increase columncounter if row is even, else decrease ( this is to help with the zigzagging pattern )
	//std::cout << "column up" << std::endl;
	if(columncounter >= HEIGHTMAPSIZE){	// thus we check if columncounter = HEIGHTMAPSIZE
		columncounter += rowcounter % 2 ? 1 : -1;	// decrease columncounter if row was even, increase if it was odd.
		rowcounter++;
		rowfinished = true;
		thisjob->KillJob(); // finished row
	}
	HeightMutex.unlock();
}

void Manager::WriteHeightMapToFile(){
	std::ofstream ClearFile;
	ClearFile.open(filename, std::ofstream::out | std::ofstream::trunc);
	ClearFile.close();
	std::ofstream OutputFile(filename);
	if(OutputFile.is_open()){
		for(unsigned int i = 0; i < HEIGHTMAPSIZE; i++){
			for(unsigned int j = 0; j < HEIGHTMAPSIZE; j++){
				OutputFile << heightmap[i][j] << " ";
			}
			OutputFile << "\n";
		}
		OutputFile.close();
	}
	else{
		std::cout << "Unable to open file: " << filename << std::endl;
	}
}

void Manager::ControlCNC(JobBase* thisjob){
	// feedrate = 6000, so 3 seconds for a row
	if(!rowfinished && RowJobID){
		thisjob->SetInterval(TimePerRead); // check every TimePerRead seconds if row has finished scanning
		std::cout << "had to wait" << std::endl;
		return;	// row is still being scanned
	}
	else if(rowfinished && RowJobID){
		thisjob->SetInterval(TimePerRow); // set interval back to 3s
		F.MoveHead({rowcounter % 2 ? 0 : 300.0, 0, 10});
		F.MoveHead({rowcounter % 2 ? 0 : 300.0, rowcounter * .5, 0});
		rowfinished = false;
	}
	// check if area is scanned
	if(rowcounter >= HEIGHTMAPSIZE){
		thisjob->KillJob();
		WriteHeightMapToFile();
		return;
	}
	Job* heightjob = new Job(0, TimePerRead, StoreHeight);
	JE->AddJob(heightjob);
	RowJobID = heightjob->GetJobID();
	F.MoveHead({rowcounter % 2 ? 300.0 : 0, 0, 0}); // this will take 3 seconds, move head to opposite direction
}