#include "sizeswidget.h"
#include "ui_sizeswidget.h"

#include <QComboBox>

SizesWidget::SizesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SizesWidget)
{
    ui->setupUi(this);

    ui->rowSize->addItems({"Fit", "S", "M", "L", "XL"});
    ui->colSize->addItems({"Fit", "S", "M", "L", "XL"});

    ui->rowSize->setCurrentIndex(1);
    ui->colSize->setCurrentIndex(0);
}

SizesWidget::~SizesWidget()
{
    delete ui;
}

void SizesWidget::on_rowSize_currentIndexChanged(int index)
{

}

void SizesWidget::on_colSize_currentIndexChanged(int index)
{

}

