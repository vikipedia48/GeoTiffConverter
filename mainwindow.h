#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <optional>
#include "commonfunctions.h"
#include "configurergbform.h"
#include "conversionparameters.h"
//#include "imagedisplaythread.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_inputFile_clicked();

    void on_comboBox_outputMode_currentIndexChanged(int index);

    void on_pushButton_outputModeConfigure_clicked();

    void on_comboBox_splitIntoTiles_currentIndexChanged(int index);


    void on_pushButton_reset_clicked();

    void on_pushButton_save_clicked();

    void on_pushButton_preview_clicked();

public slots:
    void receiveColorValues(const std::map<double,color>& colors);
    void receiveGradient(bool yes);
    void receivePreviewRequest(const std::map<double,color>& colorMap, bool gradient);
    void receivePreviewRequest(const std::map<double,color>& colorMap);

private:
    Ui::MainWindow *ui;
    ConfigureRGBForm* configWindow;
    ConversionParameters parameters;

    void exportImage();
    void previewImage();
    void previewImage(const std::map<double,color>& colorMap, bool gradient);
    void previewImage(const std::map<double,color>& colorMap);
    void changeSuccessState(Util::SuccessStateColor color);
    void getTileSize(int& tileSizeX, int& tileSizeY, std::pair<int,int> widthAndHeight);

    bool checkIfEmpty(QLineEdit* lineEdit, bool printError = false);
    int getIntegerFromLine(QLineEdit* lineEdit, bool printError = false);
    Util::TileMode getTileModeSelected();
    Util::OutputMode getOutputModeSelected();
    std::array<int,4> getCropCoordinatesInputted();
};

#endif // MAINWINDOW_H
