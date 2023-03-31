#include "datacomparewidgetitem.h"
#include "ui_datacomparewidgetitem.h"

DataCompareWidgetItem::DataCompareWidgetItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataCompareWidgetItem)
{
    ui->setupUi(this);
}

DataCompareWidgetItem::~DataCompareWidgetItem()
{
    delete ui;
}
