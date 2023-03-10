#include "displaytask.h"
#include "tifffunctions.h"
#include "pngfunctions.h"

DisplayTask::DisplayTask()
{

}

void DisplayTask::run()
{
    auto tileWidthAndHeight = std::pair<unsigned int, unsigned int> {endX-startX+1, endY-startY+1};
            switch (outputMode) {
                case Util::OutputMode::Grayscale16_MinToMax:
                    {
                        auto minAndMax = std::pair<double,double>{};
                        if (!Tiff::GetMinAndMaxValues(inputPath, minAndMax.first, minAndMax.second)) return;
                        Png::PreviewPng(Png::CreateG16_MinToMax(inputPath, minAndMax, startX, startY, endX, endY), tileWidthAndHeight, Util::PixelSize::SixteenBit);
                    }
                    break;
                case Util::OutputMode::Grayscale16_TrueValue:
                    Png::PreviewPng(Png::CreateG16_TrueValue(inputPath, parameters.offset.value_or(0), startX, startY, endX, endY), tileWidthAndHeight, Util::PixelSize::SixteenBit);
                    break;
                case Util::OutputMode::RGB_UserValues:
                    if (parameters.colorValues.value_or(std::map<double,color>()).empty()) {
                        Gui::ThrowError("Invalid values for the RGB_UserValues method.");
                        return;
                    }
                    Png::PreviewPng(Png::CreateRGB_UserValues(inputPath, parameters.colorValues.value(), startX, startY, endX, endY), tileWidthAndHeight, Util::PixelSize::ThirtyTwoBit);
                    break;
                case Util::OutputMode::RGB_UserRanges:
                if (parameters.colorValues.value_or(std::map<double,color>()).empty()) {
                        Gui::ThrowError("Invalid values for the RGB_UserRanges method.");
                        return;
                    }
                    Png::PreviewPng(Png::CreateRGB_UserRanges(inputPath, parameters.colorValues.value(), parameters.gradient.value_or(false), startX, startY, endX, endY), tileWidthAndHeight, Util::PixelSize::ThirtyTwoBit);
                    break;
                case Util::OutputMode::RGB_Formula:
                    Png::PreviewPng(Png::CreateRGB_Formula(inputPath, startX, startY, endX, endY), tileWidthAndHeight, Util::PixelSize::ThirtyTwoBit);
                    break;
                default:
                    return;
            }
}
