#include "pngfunctions.h"
#include "tifffunctions.h"
#include "commonfunctions.h"
#include <limits.h>



std::unique_ptr<unsigned short[]> Png::CreateG16_MinToMax(const QString &path, std::pair<double, double> minAndMax, int startX, int startY, int endX, int endY)
{
    auto buf = std::unique_ptr<unsigned short[]>(new unsigned short[(endX-startX+1)*(endY-startY+1)]);
    auto counter = 0;
    if (!Tiff::LoadTiff(path, [&buf,&counter,minAndMax, startX, endX](std::vector<double>&& pixels) {
        for (auto x = startX; x <= endX; ++x) {
            auto remapped = Util::Remap(pixels[x], minAndMax.first, minAndMax.second, 0, USHRT_MAX);
            unsigned short cell = round(remapped);
            buf[counter++] = cell;
        }
    }, startY, endY)) return {};
    return buf;
}

std::unique_ptr<unsigned short[]> Png::CreateG16_TrueValue(const QString &path, double offset, int startX, int startY, int endX, int endY)
{
    auto buf = std::unique_ptr<unsigned short[]>(new unsigned short[(endX-startX+1)*(endY-startY+1)]);
    auto counter = 0;
    if (!Tiff::LoadTiff(path, [&buf,&counter,offset, startX, endX](std::vector<double>&& pixels) {
        for (auto x = startX; x <= endX; ++x) {
                        auto cell = pixels[x];
                        unsigned short value;
                                    if (cell+offset < 0) value = 0;
                                    else if (cell+offset > USHRT_MAX) value = USHRT_MAX;
                                    else value = (unsigned short)(cell+offset);
                                    buf[counter++] = value;
        }
    }, startY, endY)) return {};
    return buf;
}

std::unique_ptr<unsigned char[]> Png::CreateRGB_UserValues(const QString &path, std::map<double, color> colorValues, int startX, int startY, int endX, int endY)
{
    constexpr auto numberOfChannels = 4;
    auto numberOfPixels = (endX-startX+1)*(endY-startY+1);
    auto buf = std::unique_ptr<unsigned char[]>(new unsigned char[numberOfChannels*numberOfPixels]);
    auto counter = 0;
    if (!Tiff::LoadTiff(path, [&buf,&counter,&colorValues, startX, endX, numberOfPixels](std::vector<double>&& pixels) {
        for (auto x = startX; x <= endX; ++x) {
                        auto ar = colorValues.at(pixels[x]);
                        buf[counter] = ar[0];
                        buf[counter+1*numberOfPixels] = ar[1];
                        buf[counter+2*numberOfPixels] = ar[2];
                        buf[counter+3*numberOfPixels] = ar[3];
                        ++counter;
        }
    }, startY, endY)) return {};
    return buf;
}

std::unique_ptr<unsigned char[]> Png::CreateRGB_UserRanges(const QString &path, std::map<double, color> colorValues, bool useGradient, int startX, int startY, int endX, int endY)
{
    constexpr auto numberOfChannels = 4;
    auto numberOfPixels = (endX-startX+1)*(endY-startY+1);
    auto buf = std::unique_ptr<unsigned char[]>(new unsigned char[numberOfChannels*numberOfPixels]);
    auto counter = 0;
    if (!Tiff::LoadTiff(path, [&buf,&counter,&colorValues, startX, endX, numberOfPixels, useGradient](std::vector<double>&& pixels) {
        for (auto x = startX; x <= endX; ++x) {
                        auto value = pixels[x];
                        color ar = {0,0,0,0};
                        for (auto it = colorValues.begin(); it != colorValues.end();) {

                            if (value > it->first) {
                                ++it;
                                continue;
                            }
                            if (!useGradient || value == it->first) ar = it->second;
                            else {
                                auto previousIt = it;
                                --previousIt;
                                double from1 = previousIt->first, to1 = it->first;
                                color from2 = previousIt->second, to2 = it->second;
                                ar[0] = Util::Remap(value, from1, to1, from2[0], to2[0]);
                                ar[1] = Util::Remap(value, from1, to1, from2[1], to2[1]);
                                ar[2] = Util::Remap(value, from1, to1, from2[2], to2[2]);
                                ar[3] = Util::Remap(value, from1, to1, from2[3], to2[3]);
                            }
                            break;
                        }
                        buf[counter] = ar[0];
                        buf[counter+1*numberOfPixels] = ar[1];
                        buf[counter+2*numberOfPixels] = ar[2];
                        buf[counter+3*numberOfPixels] = ar[3];
                        ++counter;

        }
    }, startY, endY)) return {};
    return buf;
}

std::unique_ptr<unsigned char[]> Png::CreateRGB_Formula(const QString &path, int startX, int startY, int endX, int endY)
{
    constexpr auto numberOfChannels = 4;
    auto numberOfPixels = (endX-startX+1)*(endY-startY+1);

    auto buf = std::unique_ptr<unsigned char[]>(new unsigned char[numberOfChannels*numberOfPixels]);
    auto counter = 0;

    if (!Tiff::LoadTiff(path, [&buf,&counter, startX, endX, numberOfPixels](std::vector<double>&& pixels) {
        for (auto x = startX; x <= endX; ++x) {
                        long value = pixels[x];
                        color ar = {0,0,0,255};
                        if (value < 0) {
                            value*=-1;
                            ar[3] = 128;
                        }
                        if (value >= 256*256*256) ar = {255,255,255, ar[3]};
                        else {
                            ar[0] = value/(256*256);
                            value-=(256*256);
                            ar[1] = value/256;
                            ar[2] = value%256;
                        }
                        buf[counter] = ar[0];
                        buf[counter+1*numberOfPixels] = ar[1];
                        buf[counter+2*numberOfPixels] = ar[2];
                        buf[counter+3*numberOfPixels] = ar[3];
                        ++counter;
        }
    }, startY, endY)) return {};

    return buf;
}
