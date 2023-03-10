#ifndef TIFFFUNCTIONS_H
#define TIFFFUNCTIONS_H

#include <vector>
#include <QString>
#include <queue>
#include <set>

namespace Tiff {

struct TiffProperties {
    unsigned int width, height, bitsPerSample, sampleFormat;
};

std::pair<unsigned int, unsigned int> GetWidthAndHeight(const QString& path);
bool LoadTiff(const QString& path, const std::function<void(std::vector<double>&&)>& func, int startY = 0, int endY = -1);
std::vector<double> GetVectorFromScanline(void* data, const TiffProperties& properties);
bool GetMinAndMaxValues(const QString& path, double &min, double &max);
std::set<double> GetDistinctValues(const QString& path);
}


#endif // TIFFFUNCTIONS_H
