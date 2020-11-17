#pragma once

#include "Sensor.h"
#include "JobExecuter.h"
#include <thread>
#include <mutex>

#define HEIGHTMAPSIZE 10

class Manager
{
private:
	static const char filename[];
	static double heightmap[HEIGHTMAPSIZE][HEIGHTMAPSIZE];
	static unsigned int rowcounter;
	static unsigned int columncounter;
	static unsigned int RowJobID;
	static bool rowfinished;
	static std::mutex HeightMutex;
	
	static JobExecuter* JE;

	static Frees F;
	static Sensor S;
	Job *J;

public:
	Manager(JobExecuter* JE, Vec3<double> StartPos);
	~Manager();

	static void StoreHeight(JobBase* thisjob);
	static void ControlCNC(JobBase* thisjob);
	static void WriteHeightMapToFile();
};


