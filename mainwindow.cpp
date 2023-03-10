#include <QFileInfo>
#include <QDir>
#include <algorithm>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "qtfunctions.h"
#include "pngfunctions.h"
#include "tifffunctions.h"
#include "displaytask.h"
#include <QThreadPool>

#define emptyInput -1
#define invalidInput -2

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->configWindow = nullptr;
    this->setWindowTitle("GeoTiff Converter");

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_inputFile_clicked()
{
    ui->lineEdit_inputFile->setText(Gui::GetInputPath("Choose tiff file","TIFF files (*.tiff *tif)"));
    auto widthAndHeight = Tiff::GetWidthAndHeight(ui->lineEdit_inputFile->text());
    if (widthAndHeight.first == 0 || widthAndHeight.second == 0) {
        Gui::ThrowError("Selected file is not a valid TIFF image");
        ui->lineEdit_inputFile->clear();
        return;
    }
    ui->pushButton_inputFile->setEnabled(false);
    ui->lineEdit_cropStartX->setText("0");
    ui->lineEdit_cropStartY->setText("0");
    ui->lineEdit_cropEndX->setText(QString::number(widthAndHeight.first-1));
    ui->lineEdit_cropEndY->setText(QString::number(widthAndHeight.second-1));
}


void MainWindow::on_comboBox_outputMode_currentIndexChanged(int index)
{
    if (configWindow != nullptr) {
        if (configWindow->isVisible() && configWindow->mode != getOutputModeSelected()) {
            Gui::ThrowError("Output mode changed. Closing ConfigureRGB window.");
            configWindow->close();
        }
    }
    switch (getOutputModeSelected()) {
        case Util::OutputMode::No:
            changeSuccessState(Util::SuccessStateColor::Red);
            break;
        case Util::OutputMode::Grayscale16_TrueValue:
            changeSuccessState((!parameters.offset.has_value()) ? Util::SuccessStateColor::Yellow : Util::SuccessStateColor::Green);
            break;
        case Util::OutputMode::Grayscale16_MinToMax: case Util::OutputMode::RGB_Formula:
            changeSuccessState(Util::SuccessStateColor::Green);
            break;
        case Util::OutputMode::RGB_UserRanges: case Util::OutputMode::RGB_UserValues:
            changeSuccessState((!parameters.colorValues.has_value()) ? Util::SuccessStateColor::Red : Util::SuccessStateColor::Green);
            break;
    }
}


void MainWindow::on_pushButton_outputModeConfigure_clicked()
{
    auto index = getOutputModeSelected();
    switch (index) {
        case Util::OutputMode::No:
            Gui::ThrowError("Please select output mode");
            break;
        case Util::OutputMode::Grayscale16_TrueValue:
            parameters.offset = Gui::GetNumberValueFromInputDialog("Input offset", "Please input the optional offset for values.");
            if (parameters.offset == 0) {
                parameters.offset.reset();
                changeSuccessState(Util::SuccessStateColor::Yellow);
            }
            else changeSuccessState(Util::SuccessStateColor::Green);
            break;
        case Util::OutputMode::Grayscale16_MinToMax: case Util::OutputMode::RGB_Formula:
            Gui::PrintMessage("No settings","There are no configurations for this mode");
            break;
        case Util::OutputMode::RGB_UserRanges: case Util::OutputMode::RGB_UserValues:
            if (configWindow != nullptr) {
                if (configWindow->isVisible()) {
                    Gui::ThrowError("ConfigureRGB already opened.");
                    return;
                }
            }
            this->configWindow = new ConfigureRGBForm(ui->lineEdit_inputFile->text(), getOutputModeSelected());
            connect(configWindow,SIGNAL(sendColorValues(const std::map<double,color>&)),this,SLOT(receiveColorValues(const std::map<double,color>&)));
            connect(configWindow,SIGNAL(sendGradient(bool)),this,SLOT(receiveGradient(bool)));
            connect(configWindow,SIGNAL(sendPreviewRequest(const std::map<double,color>&, bool)),this,SLOT(receivePreviewRequest(const std::map<double,color>&, bool)));
            connect(configWindow,SIGNAL(sendPreviewRequest(const std::map<double,color>&)),this,SLOT(receivePreviewRequest(const std::map<double,color>&)));
            configWindow->show();
            break;
    }
}

void MainWindow::changeSuccessState(Util::SuccessStateColor color)
{
    if (color == Util::SuccessStateColor::Yellow) {
            ui->label_outputModeSuccess->setStyleSheet("background-color: rgb(237, 212, 0);color: rgb(255, 255, 255);margin-left:25%;border-radius:6%;");
            ui->label_outputModeSuccess->setText(" ! ");
        }
        else if (color == Util::SuccessStateColor::Red) {
            ui->label_outputModeSuccess->setStyleSheet("background-color: rgb(204, 0, 0);color: rgb(255, 255, 255);margin-left:25%;border-radius:6%;");
            ui->label_outputModeSuccess->setText(" ! ");
        }
        else if (color == Util::SuccessStateColor::Green) {
            ui->label_outputModeSuccess->setStyleSheet("background-color: rgb(78, 155, 6);color: rgb(255, 255, 255);margin-left:25%;border-radius:6%;");
            ui->label_outputModeSuccess->setText(" ✓ ");
    }
}

void MainWindow::on_comboBox_splitIntoTiles_currentIndexChanged(int index)
{
    if (index==0) {
        ui->lineEdit_tilesX->setEnabled(false);
        ui->lineEdit_tilesY->setEnabled(false);
    }
    else {
        ui->lineEdit_tilesX->setEnabled(true);
        ui->lineEdit_tilesY->setEnabled(true);
    }
}


void MainWindow::receiveColorValues(const std::map<double, color> &colors)
{
    if (colors.empty()) {
        parameters.colorValues.reset();
        return;
    }
    parameters.colorValues = colors;
    changeSuccessState(Util::SuccessStateColor::Green);
}

void MainWindow::receiveGradient(bool yes)
{
    parameters.gradient = yes;
}

void MainWindow::on_pushButton_reset_clicked()
{
    if (!Gui::GiveQuestion("Are you sure you want to reset your settings?")) {
        return;
    }
    this->parameters.colorValues.reset();
    this->parameters.gradient.reset();
    this->parameters.offset.reset();
    ui->lineEdit_inputFile->clear();
    ui->pushButton_inputFile->setEnabled(true);
    ui->comboBox_outputMode->setCurrentIndex(0);
    changeSuccessState(Util::SuccessStateColor::Red);
    ui->lineEdit_cropStartX->clear();
    ui->lineEdit_cropStartY->clear();
    ui->lineEdit_cropEndX->clear();
    ui->lineEdit_cropEndY->clear();
    ui->comboBox_splitIntoTiles->setCurrentIndex(0);
    ui->lineEdit_tilesX->clear();
    ui->lineEdit_tilesY->clear();
    ui->lineEdit_tilesX->setEnabled(false);
    ui->lineEdit_tilesY->setEnabled(false);
}

void MainWindow::getTileSize(int &tileSizeX, int &tileSizeY, std::pair<int,int> widthAndHeight)
{
    if (getTileModeSelected() == Util::TileMode::No) return;
        bool ok1, ok2;
        unsigned int t1,t2;
        t1 = ui->lineEdit_tilesX->text().toUInt(&ok1);
        t2 = ui->lineEdit_tilesY->text().toUInt(&ok2);
        if (!ok1 || !ok2) {
            Gui::ThrowError("Error. Invalid input for tile size.");
            return;
        }
        if (getTileModeSelected() == Util::TileMode::DefineBySize) {
            tileSizeX = t1;
            tileSizeY = t2;
        }
        else {
            double c1 = (double)widthAndHeight.first/t1, c2 = (double)widthAndHeight.second/t2;
            tileSizeX = qCeil(c1);
            tileSizeY = qCeil(c2);
        }
}

bool MainWindow::checkIfEmpty(QLineEdit* lineEdit, bool printError)
{
    if (lineEdit->text().isEmpty()) {
        if (printError) Gui::ThrowError("Input empty.");
        return true;
    }
    return false;
}

int MainWindow::getIntegerFromLine(QLineEdit *lineEdit, bool printError)
{
    int rv = -1;
    bool ok;
    rv = lineEdit->text().toInt(&ok);
    if (!ok || rv<0) {
        if (printError) Gui::ThrowError("Invalid integer input.");
        return -1;
    }
    return rv;
}

Util::TileMode MainWindow::getTileModeSelected()
{
    return static_cast<Util::TileMode>(ui->comboBox_splitIntoTiles->currentIndex());
}

Util::OutputMode MainWindow::getOutputModeSelected()
{
    return static_cast<Util::OutputMode>(ui->comboBox_outputMode->currentIndex());
}

std::array<int, 4> MainWindow::getCropCoordinatesInputted()
{
    if (checkIfEmpty(ui->lineEdit_cropStartX) && checkIfEmpty(ui->lineEdit_cropStartY) && checkIfEmpty(ui->lineEdit_cropEndX) && checkIfEmpty(ui->lineEdit_cropEndY)) {
        return {emptyInput,0,0,0};
    }
    auto startX = getIntegerFromLine(ui->lineEdit_cropStartX, true);
    if (startX == -1) return {invalidInput,0,0,0};
    auto startY = getIntegerFromLine(ui->lineEdit_cropStartY, true);
    if (startY == -1) return {invalidInput,0,0,0};
    auto endX = getIntegerFromLine(ui->lineEdit_cropEndX, true);
    if (endX == -1) return {invalidInput,0,0,0};
    auto endY = getIntegerFromLine(ui->lineEdit_cropEndY, true);
    if (endY == -1) return {invalidInput,0,0,0};
    if (startX >= endX) {
        Gui::ThrowError("Start X must be lower than end X");
        return {invalidInput,0,0,0};
    }
    if (startY >= endY) {
        Gui::ThrowError("Start X must be lower than end X");
        return {invalidInput,0,0,0};
    }
    return {startX, startY, endX, endY};
}

void MainWindow::receivePreviewRequest(const std::map<double, color> &colorMap, bool gradient)
{
    previewImage(colorMap, gradient);
}

void MainWindow::receivePreviewRequest(const std::map<double, color> &colorMap)
{
    previewImage(colorMap);
}

void MainWindow::previewImage(const std::map<double, color> &colorMap, bool gradient)
{
    this->parameters.gradient = gradient;
    this->parameters.colorValues = colorMap;
    previewImage();
}

void MainWindow::previewImage(const std::map<double, color> &colorMap)
{
    this->parameters.colorValues = colorMap;
    previewImage();
}


void MainWindow::previewImage() {

    if (getOutputModeSelected() == Util::OutputMode::No) {
        Gui::ThrowError("Invalid output method");
        return;
    }

    if (checkIfEmpty(ui->lineEdit_inputFile)) return;
    auto inputPath = ui->lineEdit_inputFile->text();

    auto widthAndHeight = Tiff::GetWidthAndHeight(inputPath);
    if (widthAndHeight.first == 0 || widthAndHeight.second == 0) return;

    auto cropCoordinates = getCropCoordinatesInputted();
    if (cropCoordinates[0] == invalidInput) {
        Gui::ThrowError("Error while reading crop coordinates");
        return;
    }
    if (cropCoordinates[0] == emptyInput) {
        cropCoordinates[0] = 0;
        cropCoordinates[1] = 0;
        cropCoordinates[2] = widthAndHeight.first-1;
        cropCoordinates[3] = widthAndHeight.second-1;
    }
    auto startX = cropCoordinates[0];
    auto startY = cropCoordinates[1];
    auto endX = cropCoordinates[2];
    auto endY = cropCoordinates[3];

    DisplayTask* displayImageTask = new DisplayTask(getOutputModeSelected(),inputPath,startX,startY,endX,endY,ConversionParameters(parameters.offset, parameters.colorValues, parameters.gradient));
    QThreadPool::globalInstance()->start(displayImageTask);
}

void MainWindow::exportImage() {
    if (checkIfEmpty(ui->lineEdit_inputFile, true)) return;
    auto inputPath = ui->lineEdit_inputFile->text();
    auto path = Gui::GetSavePath();
    if (path.isEmpty()) {
        Gui::ThrowError("Please input save path.");
        return;
    }
    auto outputPath = QFileInfo(path).absolutePath()+QDir::separator()+QFileInfo(path).completeBaseName();
    if (path.right(4) != ".png") path.append(".png");

    auto widthAndHeight = Tiff::GetWidthAndHeight(inputPath);
    if (widthAndHeight.first == 0 || widthAndHeight.second == 0) return;

    auto cropCoordinates = getCropCoordinatesInputted();
    if (cropCoordinates[0] == invalidInput) return;
    if (cropCoordinates[0] == emptyInput) {
        cropCoordinates[0] = 0;
        cropCoordinates[1] = 0;
        cropCoordinates[2] = widthAndHeight.first-1;
        cropCoordinates[3] = widthAndHeight.second-1;
    }
    auto absoluteStartX = cropCoordinates[0];
    auto absoluteStartY = cropCoordinates[1];
    auto absoluteEndX = cropCoordinates[2];
    auto absoluteEndY = cropCoordinates[3];
    auto absoluteWidthAndHeight = std::pair<unsigned int, unsigned int>(absoluteEndX-absoluteStartX+1, absoluteEndY-absoluteStartY+1);

    int tileSize_x = absoluteWidthAndHeight.first, tileSize_y = absoluteWidthAndHeight.second;
    getTileSize(tileSize_x, tileSize_y, absoluteWidthAndHeight);

    for (auto startX = absoluteStartX; startX < absoluteEndX; startX+=tileSize_x) {
        for (auto startY = absoluteStartY; startY < absoluteEndY; startY+=tileSize_y) {
            if (getTileModeSelected() != Util::TileMode::No) path = outputPath+"_"+QString::number(startX/tileSize_x)+"_"+QString::number(startY/tileSize_y)+".png";
            auto endX = std::min((unsigned int)startX+tileSize_x-1,absoluteWidthAndHeight.first-1);
            auto endY = std::min((unsigned int)startY+tileSize_y-1,absoluteWidthAndHeight.second-1);
            auto tileWidthAndHeight = std::pair<unsigned int, unsigned int> {std::min(startX+tileSize_x,absoluteEndX-absoluteStartX+1)-startX,std::min(startY+tileSize_y,absoluteEndY-absoluteStartY+1)-startY};
            switch (getOutputModeSelected()) {
                case Util::OutputMode::Grayscale16_MinToMax:
                    {
                        auto minAndMax = std::pair<double,double>{};
                        if (!Tiff::GetMinAndMaxValues(inputPath, minAndMax.first, minAndMax.second)) return;
                        Png::CreatePng(Png::CreateG16_MinToMax(inputPath, minAndMax, startX, startY, endX, endY), tileWidthAndHeight, Util::PixelSize::SixteenBit, path);
                    }
                    break;
                case Util::OutputMode::Grayscale16_TrueValue:
                    Png::CreatePng(Png::CreateG16_TrueValue(inputPath, parameters.offset.value_or(0), startX, startY, endX, endY), tileWidthAndHeight, Util::PixelSize::SixteenBit, path);
                    break;
                case Util::OutputMode::RGB_UserValues:
                    if (parameters.colorValues.value_or(std::map<double,color>()).empty()) {
                        Gui::ThrowError("Invalid values for the RGB_UserValues method.");
                        return;
                    }
                    Png::CreatePng(Png::CreateRGB_UserValues(inputPath, parameters.colorValues.value(), startX, startY, endX, endY), tileWidthAndHeight, Util::PixelSize::ThirtyTwoBit, path);
                    break;
                case Util::OutputMode::RGB_UserRanges:
                    if (parameters.colorValues.value_or(std::map<double,color>()).empty()) {
                        Gui::ThrowError("Invalid values for the RGB_UserRanges method.");
                        return;
                    }
                    Png::CreatePng(Png::CreateRGB_UserRanges(inputPath, parameters.colorValues.value(), parameters.gradient.value_or(false), startX, startY, endX, endY), tileWidthAndHeight, Util::PixelSize::ThirtyTwoBit, path);
                    break;
                case Util::OutputMode::RGB_Formula:
                    Png::CreatePng(Png::CreateRGB_Formula(inputPath, startX, startY, endX, endY), tileWidthAndHeight, Util::PixelSize::ThirtyTwoBit, path);
                    break;
                default:
                    Gui::ThrowError("Invalid output method.");
                    return;
            }
        }
    }
}

void MainWindow::on_pushButton_save_clicked()
{
    exportImage();
}

void MainWindow::on_pushButton_preview_clicked()
{
    previewImage();
}

