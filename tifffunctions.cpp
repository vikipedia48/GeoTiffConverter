#include "qtfunctions.h"
#include "tifffunctions.h"
#include "float.h"
#include "tiff.h"
#include "tiffio.h"


bool Tiff::LoadTiff(const QString &path, const std::function<void (std::vector<double> &&)> &func, int startY, int endY)
{
    TIFF* tif = TIFFOpen(path.toStdString().data(),"r");
        if (!tif) {
            Gui::ThrowError("Error loading image.");
            TIFFClose(tif);
            return false;
        }
        TiffProperties properties {};
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,&properties.width);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &properties.height);
        TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE,&properties.bitsPerSample);
        TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT,&properties.sampleFormat);
        if (properties.width == 0 || properties.height == 0) {
            Gui::ThrowError("Error. Image is invalid.");
            TIFFClose(tif);
            return false;
        }
        if (!(properties.bitsPerSample == 8 || properties.bitsPerSample == 16 || properties.bitsPerSample == 32 || properties.bitsPerSample == 64) || properties.sampleFormat > SAMPLEFORMAT_IEEEFP) {
            Gui::ThrowError("Unsupported file");
            TIFFClose(tif);
            return false;
        }
        if (endY == -1) endY = properties.height-1;
        void* buf = _TIFFmalloc(TIFFScanlineSize(tif));
        for (auto row = startY; row <= endY; ++row) {
                TIFFReadScanline(tif, buf, row);
                auto pixels = GetVectorFromScanline(buf,properties);
                func(std::move(pixels));
        }
        _TIFFfree(buf);
        TIFFClose(tif);
        return true;
}


std::pair<unsigned int, unsigned int> Tiff::GetWidthAndHeight(const QString &path)
{
    std::pair<unsigned int, unsigned int> rv = {0,0};
    TIFF* tif = TIFFOpen(path.toStdString().data(),"r");
    if (!tif) {
        Gui::ThrowError("Error loading image.");
        return rv;
    }
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,&rv.first);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH,&rv.second);
    TIFFClose(tif);
    return rv;
}

std::vector<double> Tiff::GetVectorFromScanline(void *data, const TiffProperties &properties)
{
    std::vector<double> rv(properties.width);
        if (properties.sampleFormat == SAMPLEFORMAT_UINT) {
            if (properties.bitsPerSample == 8) {
                auto _data = (uint8*)data;
                for (auto i = 0; i < properties.width; ++i) rv[i] = _data[i];
            }
            else if (properties.bitsPerSample == 16) {
                auto _data = (uint16*)data;
                for (auto i = 0; i < properties.width; ++i) rv[i] = _data[i];
            }
            else if (properties.bitsPerSample == 32) {
                auto _data = (uint32*)data;
                for (auto i = 0; i < properties.width; ++i) rv[i] = _data[i];
            }
            else if (properties.bitsPerSample == 64) {
                auto _data = (uint64*)data;
                for (auto i = 0; i < properties.width; ++i) rv[i] = _data[i];
            }
        }
        else if (properties.sampleFormat == SAMPLEFORMAT_INT) {
            if (properties.bitsPerSample == 8) {
                auto _data = (int8*)data;
                for (auto i = 0; i < properties.width; ++i) rv[i] = _data[i];
            }
            else if (properties.bitsPerSample == 16) {
                auto _data = (int16*)data;
                for (auto i = 0; i < properties.width; ++i) rv[i] = _data[i];
            }
            else if (properties.bitsPerSample == 32) {
                auto _data = (int32*)data;
                for (auto i = 0; i < properties.width; ++i) rv[i] = _data[i];
            }
            else if (properties.bitsPerSample == 64) {
                auto _data = (int64*)data;
                for (auto i = 0; i < properties.width; ++i) rv[i] = _data[i];
            }
        }
        else if (properties.sampleFormat == SAMPLEFORMAT_IEEEFP) {
            if (properties.bitsPerSample == 32) {
                auto _data = (float*)data;
                for (auto i = 0; i < properties.width; ++i) rv[i] = _data[i];
            }
            else if (properties.bitsPerSample == 64) {
                auto _data = (double*)data;
                for (auto i = 0; i < properties.width; ++i) rv[i] = _data[i];
            }
        }
        return rv;
}

bool Tiff::GetMinAndMaxValues(const QString &path, double &min, double &max)
{
    double _min = DBL_MAX, _max = -DBL_MAX;
        if (!LoadTiff(path,[&_min,&_max](std::vector<double>&& pixels) {
            for (auto v : pixels) {
                if (v < _min) _min = v;
                if (v > _max) _max = v;
            }
        })) return false;
        min = _min;
        max = _max;
        return true;
}

std::set<double> Tiff::GetDistinctValues(const QString &path)
{
    std::set<double> rv;
        bool ok = LoadTiff(path,[&rv](std::vector<double>&& pixels) {
                for (auto v : pixels) {
                    rv.insert(v);
                }
        });
        if (!ok) return {};
        return rv;
}




