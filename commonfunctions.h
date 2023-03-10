#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <vector>
#include <string>
#include <map>
#include <QString>

using color = std::array<unsigned char,4>;

namespace Util {
double Remap(double value, double from1, double to1, double from2, double to2);

enum class SuccessStateColor {
    Red,
    Yellow,
    Green
};

enum class OutputMode {
    No,
    Grayscale16_TrueValue,
    Grayscale16_MinToMax,
    RGB_UserRanges,
    RGB_UserValues,
    RGB_Formula
};

enum class TileMode {
    No,
    DefineByNumber,
    DefineBySize
};

enum class PixelSize {
    No,
    ThirtyTwoBit,
    SixteenBit
};

bool isAValidColor(QString cell, color& array);
}

#endif // COMMONFUNCTIONS_H
