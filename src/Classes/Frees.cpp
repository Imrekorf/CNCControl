#include "Frees.h"

Frees::Frees(Vec3<double> StartPos) : Head(StartPos) {}

Frees::Frees() {}

Frees::~Frees() {}

void Frees::StartPos(Vec3<double> StartPos) {
    Head = StartPos;
}

namespace util
{
    template <typename T>
    std::string to_string(const T &t)
    {
        std::string str{std::to_string(t)};
        int offset{1};
        if (str.find_last_not_of('0') == str.find('.'))
        {
            offset = 0;
        }
        str.erase(str.find_last_not_of('0') + offset, std::string::npos);
        return str;
    }
} // namespace util

std::string Frees::GenGCode(Vec3<double> P)
{   
    std::string Feedrate = "F" + std::to_string(this->Feedrate);
    std::string V[3] = {"X" + util::to_string<double>(P.X()),
                        "Y" + util::to_string<double>(P.Y()),
                        "Z" + util::to_string<double>(P.Y())};
    //std::cout << PositionTranslator::ToGcode(V) << std::endl;
    return Feedrate + "\n" + PositionTranslator::ToGcode(V);
}

void Frees::SendToCNC(std::string GCode){
    //std::cout << GCode << std::endl;
}

void Frees::MoveHead(Direction D, double distance)
{
    Vec3<double> V = PositionTranslator::ToVector(D);
    V *= distance;
    MoveHead(V);
}

void Frees::MoveHead(Vec3<double> P)
{
    Head += P;
    SendToCNC(GenGCode(P));
}

void Frees::SetFeedrate(unsigned int feedrate){
    this->Feedrate = feedrate;
}