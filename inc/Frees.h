#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "Vec3.h"

#include "ADCmanager.h"

class Frees
{
private:
	Vec3<double> position;

	std::ofstream GcodeTracker;

	bool Relative = 0;

	int SerialID;

	void _Move(Vec3<double> V);
	void WaitForGRBLResponse();

	ADCMaster* ADCreader;
	const unsigned int ADCChannel = 0;
	bool CNCmoving = false;
	void CheckCNCMoving();

public:
	Frees(Vec3<double> Position, ADCMaster* ADCreader);
	~Frees();

	void Move(Vec3<double> V);
	void MoveTo(Vec3<double> P);

	void SendGCode(std::string Gcode);

	const Vec3<double> GetPosition();
};
