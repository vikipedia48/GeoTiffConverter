#include "commonfunctions.h"
#include <sstream>

double Util::Remap(double value, double from1, double to1, double from2, double to2) {
    return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
}

bool Util::isAValidColor(QString cell, color &array)
{
    std::array<unsigned char,4> colorValue;
        auto val = 0, lastcomma = 0;
        auto commaCount = std::count(cell.begin(),cell.end(),',');
        if (!(commaCount == 2 || commaCount == 3)) return false;
        for (auto i = 0;; ++i) {
                    bool conversionCorrect;
                    if (cell[i] == ',') {
                        colorValue[val++] = (unsigned char)cell.mid(lastcomma,i-lastcomma).toUInt(&conversionCorrect);
                        if (!conversionCorrect) return false;
                        lastcomma = i+1;
                        if (val == commaCount) {
                            colorValue[commaCount] = (unsigned char) cell.mid(lastcomma).toUInt(&conversionCorrect);
                            if (!conversionCorrect) return false;
                            if (commaCount == 2) colorValue[3] = 255;
                            break;
                        }
                    }
         }
        array = colorValue;
        return true;
}
