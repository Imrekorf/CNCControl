#pragma once

#include <string>
#include "Vec3.h"

enum Direction {
    Right,
    Left,
    Up,
    Down,
    Z_Up,
    Z_Down
};

namespace PositionTranslator {

Vec3<double> ToVector(Direction D);
std::string ToGcode(std::string V[3]);

}