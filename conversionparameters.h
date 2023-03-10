#ifndef CONVERSIONPARAMETERS_H
#define CONVERSIONPARAMETERS_H


#include <map>
#include <optional>

using color = std::array<unsigned char,4>;

class ConversionParameters
{
public:
        std::optional<double> offset;
        std::optional<std::map<double,color>> colorValues;
        std::optional<bool> gradient;
        ConversionParameters(std::optional<double> offset, std::optional<std::map<double,color>> colorValues, std::optional<bool> gradient) : offset(offset), colorValues(colorValues), gradient(gradient) {}
        ConversionParameters();
};

#endif // CONVERSIONPARAMETERS_H
