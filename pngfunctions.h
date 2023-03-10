#ifdef __linux__
#define cimg_display 1
#elif _WIN32
#define cimg_display 2
#else
#define cimg_display 0
#endif

#ifndef PNGFUNCTIONS_H
#define PNGFUNCTIONS_H

#define cimg_use_png
#include <memory>
#include <QString>
#include <QtDebug>
#include <map>
#include "commonfunctions.h"
#include "qtfunctions.h"
#include "CImg.h"

using color = std::array<unsigned char,4>;

namespace Png {
std::unique_ptr<unsigned short[]> CreateG16_TrueValue(const QString& path, double offset, int startX, int startY, int endX, int endY);
std::unique_ptr<unsigned short[]> CreateG16_MinToMax(const QString &path, std::pair<double,double> minAndMax, int startX, int startY, int endX, int endY);
std::unique_ptr<unsigned char[]> CreateRGB_UserValues(const QString& path, std::map<double,color> colorValues, int startX, int startY, int endX, int endY);
std::unique_ptr<unsigned char[]> CreateRGB_UserRanges(const QString& path, std::map<double,color> colorValues, bool useGradient, int startX, int startY, int endX, int endY);
std::unique_ptr<unsigned char[]> CreateRGB_Formula(const QString& path, int startX = 0, int startY = 0, int endX = -1, int endY = -1);
template<typename T>
void CreatePng(std::unique_ptr<T[]> img, std::pair<unsigned int, unsigned int> widthAndHeight, Util::PixelSize pixelSize, const QString &path)
{
    if (pixelSize == Util::PixelSize::ThirtyTwoBit) {
        cimg_library::CImg<unsigned char> png(img.get(), widthAndHeight.first, widthAndHeight.second, 1, 4);
        png.save_png(path.toStdString().data());
    }
    else if (pixelSize == Util::PixelSize::SixteenBit) {
        cimg_library::CImg<unsigned short> png(img.get(), widthAndHeight.first, widthAndHeight.second, 1, 1);
        png.save_png(path.toStdString().data());
    }
    else {
        Gui::ThrowError("Error. Unrecognized pixel size.");
        return;
    }
}
template<typename T>
void PreviewPng(std::unique_ptr<T[]> img, std::pair<unsigned int, unsigned int> widthAndHeight, Util::PixelSize pixelSize) {
    if (pixelSize == Util::PixelSize::ThirtyTwoBit) {
        cimg_library::CImg<unsigned char> png(img.get(), widthAndHeight.first, widthAndHeight.second, 1, 4);
        png.display();
    }
    else if (pixelSize == Util::PixelSize::SixteenBit) {
        cimg_library::CImg<unsigned short> png(img.get(), widthAndHeight.first, widthAndHeight.second, 1, 1);
        png.display();
    }
    else {
        Gui::ThrowError("Error. Unrecognized pixel size.");
        return;
    }
}

}

#endif // PNGFUNCTIONS_H
