#include "Struct.h"

Vec3<double> PositionTranslator::ToVector(Direction D){
    Vec3<double> P[6] = {
        { 1,  0,  0},
        {-1,  0,  0},
        { 0,  1,  0},
        { 0, -1,  0},
        { 0,  0,  1},
        { 0,  0, -1}
    };
    return P[D];  
}

std::string PositionTranslator::ToGcode(std::string V[3]){
    return "G01 " + V[0] + V[1] + V[2];
}