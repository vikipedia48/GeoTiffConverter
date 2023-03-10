#include "configurergbform.h"
#include "ui_configurergbform.h"
#include "qtfunctions.h"
#include "tifffunctions.h"
#include <QFile>

ConfigureRGBForm::ConfigureRGBForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigureRGBForm)
{
    ui->setupUi(this);
}

ConfigureRGBForm::ConfigureRGBForm(const QString& inputPath, Util::OutputMode mode, QWidget *parent) : QWidget(parent), ui(new Ui::ConfigureRGBForm), mode(mode)
{
    setWindowModality(Qt::WindowModal);
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    auto title = " ", desc = " ";
    switch(mode) {
        case Util::OutputMode::RGB_UserRanges:
            title = "RGB with user defined ranges";
            desc = "Clicking a cell on the last row will add a new one.";
            ui->checkBox_gradient->setEnabled(true);
            break;
        case Util::OutputMode::RGB_UserValues:
            title = "RGB with user defined values";
            desc = "If you are loading an image with many distinct values, it might take a long time to compute.";
            break;
        default:
            this->close();
            return;
    }
    ui->label_title->setText(title);
    ui->label_desc->setText(desc);
    ui->lineEdit->setText(inputPath);
    ui->lineEdit->setEnabled(false);
    ui->pushButton_preview->setEnabled(false);
}

ConfigureRGBForm::~ConfigureRGBForm()
{
    delete ui;
}

void ConfigureRGBForm::on_pushButton_clear_clicked()
{
    if (mode == Util::OutputMode::RGB_UserRanges) {
            ui->tableWidget->item(0,1)->setText("");
            ui->tableWidget->item(1,1)->setText("");
            ui->tableWidget->item(2,0)->setText("");
            ui->tableWidget->item(2,1)->setText("");
            for (auto i = ui->tableWidget->rowCount()-1; i > 2; --i) {
                ui->tableWidget->removeRow(i);
            }
        }
    else if (mode == Util::OutputMode::RGB_UserValues) {
            for (auto i = 0; i < ui->tableWidget->rowCount(); ++i) {
                ui->tableWidget->item(i,1)->setText("");
            }
        }
}


void ConfigureRGBForm::on_pushButton_ok_clicked()
{
    auto colorMap = readTable();
    if (colorMap.empty()) return;
    emit sendColorValues(colorMap);
    if (mode == Util::OutputMode::RGB_UserRanges) emit sendGradient(ui->checkBox_gradient->isChecked());
    this->close();
}


void ConfigureRGBForm::on_pushButton_cancel_clicked()
{
    if (Gui::GiveQuestion("Are you sure want to exit?")) this->close();
}

void ConfigureRGBForm::on_pushButton_2_clicked()
{
    if (ui->lineEdit->text().isEmpty() || !QFile::exists(ui->lineEdit->text())) {
        Gui::ThrowError("No such file exists.");
        return;
    }
    if (mode == Util::OutputMode::RGB_UserRanges) {
            double min,max;
            if (!Tiff::GetMinAndMaxValues(ui->lineEdit->text(),min,max)) return;
            addNewRowToTable(QString::number(min,'g',17),"0,0,0,255",false);
            addNewRowToTable(QString::number(max,'g',17),"255,255,255,255",false);
            addNewRowToTable();
        }
        else if (mode == Util::OutputMode::RGB_UserValues) {
            auto distinctValues = Tiff::GetDistinctValues(ui->lineEdit->text());
            if (distinctValues.empty()) return;
            for (auto value : distinctValues) addNewRowToTable(QString::number(value,'g',17),"0,0,0,255",false);
        }
    ui->pushButton_clear->setEnabled(true);
    ui->pushButton_preview->setEnabled(true);
}

std::pair<double, color> ConfigureRGBForm::getRowValues(int row, bool &ok)
{
    bool _ok;
        auto value = ui->tableWidget->item(row,0)->text().toDouble(&_ok);
        if (!_ok) {
            Gui::ThrowError("Invalid value at row " + QString::number(row));
            ok = false;
            return {};
        }
        std::array<unsigned char,4> colorValue;
        if (!Util::isAValidColor(ui->tableWidget->item(row,1)->text(),colorValue)) {
            Gui::ThrowError("Invalid color at row " + QString::number(row));
            ok = false;
            return {};
        }
        ok = true;
        return {value,colorValue};
}

void ConfigureRGBForm::addNewRowToTable(QString firstValue, QString secondValue, bool isFirstColumnEditable)
{
    auto row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        auto item0 = new QTableWidgetItem(firstValue);
        auto item1 = new QTableWidgetItem(secondValue);
        if (!isFirstColumnEditable) item0->setFlags(item0->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(row,0,item0);
        ui->tableWidget->setItem(row,1,item1);
}

std::map<double, color> ConfigureRGBForm::readTable()
{
    std::map<double,color> colorMap;
    for (auto i = 0; i < ui->tableWidget->rowCount(); ++i) {
                if (ui->tableWidget->item(i,0)->text().isEmpty() && ui->tableWidget->item(i,1)->text().isEmpty()) continue;
                bool ok;
                colorMap.emplace(getRowValues(i,ok));
                if (!ok) return {};
            }
    return colorMap;
}

void ConfigureRGBForm::on_pushButton_preview_clicked()
{
    auto colorMap = readTable();
    if (mode == Util::OutputMode::RGB_UserRanges) emit sendPreviewRequest(colorMap, ui->checkBox_gradient->isChecked());
    else if (mode == Util::OutputMode::RGB_UserValues) emit sendPreviewRequest(colorMap);
}


void ConfigureRGBForm::on_tableWidget_cellClicked(int row, int column)
{
    if (mode == Util::OutputMode::RGB_UserRanges && ui->tableWidget->rowCount() == row+1) addNewRowToTable();
}

