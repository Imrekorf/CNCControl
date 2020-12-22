#pragma once

#include <thread>
#include <mutex>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include "Vec3.h"

class Frees
{
private:
	Vec3<double> position;

	std::ofstream GcodeTracker;

	bool Relative = 0;

	int SerialID;

	void _Move(Vec3<double> V);
	void WaitForGRBLResponse();

	static double ADCValue;
    static std::mutex ADCmutex;
	static void ADCreadThread();

public:
	Frees(Vec3<double> Position);
	~Frees();

	void Move(Vec3<double> V);
	void MoveTo(Vec3<double> P);

	void SendGCode(std::string Gcode);

	const Vec3<double> GetPosition();
};
