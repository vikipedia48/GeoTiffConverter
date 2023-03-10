#ifndef DISPLAYTASK_H
#define DISPLAYTASK_H

#include <QRunnable>
#include "conversionparameters.h"
#include "commonfunctions.h"

class DisplayTask : public QRunnable
{
public:
    DisplayTask();
    DisplayTask(Util::OutputMode outputMode, QString inputPath, int startX, int startY, int endX, int endY, ConversionParameters parameters) : outputMode(outputMode), inputPath(inputPath), startX(startX), startY(startY), endX(endX), endY(endY), parameters(parameters) {}
        Util::OutputMode outputMode;
        QString inputPath;
        int startX, startY, endX, endY;
        ConversionParameters parameters;

public:
        void run() override;
};

#endif // DISPLAYTASK_H
