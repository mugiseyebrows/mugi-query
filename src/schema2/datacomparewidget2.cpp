#include "datacomparewidget2.h"
#include "ui_datacomparewidget2.h"

DataCompareWidget2::DataCompareWidget2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataCompareWidget2)
{
    ui->setupUi(this);
}

DataCompareWidget2::~DataCompareWidget2()
{
    delete ui;
}
