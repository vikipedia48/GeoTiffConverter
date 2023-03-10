#ifndef CONFIGURERGBFORM_H
#define CONFIGURERGBFORM_H

#include <QWidget>
#include "commonfunctions.h"

using color = std::array<unsigned char,4>;

namespace Ui {
class ConfigureRGBForm;
}

class ConfigureRGBForm : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigureRGBForm(QWidget *parent = nullptr);
    ConfigureRGBForm(const QString& inputPath, Util::OutputMode mode, QWidget *parent = nullptr);
    ~ConfigureRGBForm();
    Util::OutputMode mode;

private slots:
    void on_pushButton_clear_clicked();

    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_preview_clicked();

    void on_tableWidget_cellClicked(int row, int column);

signals:
    void sendColorValues(const std::map<double,color>& colors);
    void sendGradient(bool yes);
    void sendMinAndMax(std::array<double,2>& minAndMax);
    void sendPreviewRequest(const std::map<double,color>& colors, bool gradient);
    void sendPreviewRequest(const std::map<double,color>& colors);
private:
    Ui::ConfigureRGBForm *ui;

    std::pair<double,color> getRowValues(int row, bool& ok);
    void addNewRowToTable(QString firstValue = "", QString secondValue = "", bool isFirstColumnEditable = true);
    std::map<double,color> readTable();

};

#endif // CONFIGURERGBFORM_H
