#pragma once

#include <string>
#include "Struct.h"

class Frees
{
private:
    Vec3<double> Head;

    unsigned int Feedrate = 200; // unit in ?? 100mmpm? max height diff 3cm over 30cm
    std::string GenGCode(Vec3<double> P);
    void SendToCNC(std::string GCode);


public:
    Frees(Vec3<double> StartPos);
    Frees();
    ~Frees();

    void StartPos(Vec3<double> StartPos);
    void MoveHead(Direction D, double Distance);
    void MoveHead(Vec3<double> P);

    void SetFeedrate(unsigned int feedrate);

    // return read only position
    const Vec3<double> GetPosition() { return Head; }
};
