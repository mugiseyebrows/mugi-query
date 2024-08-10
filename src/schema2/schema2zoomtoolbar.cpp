#include "schema2zoomtoolbar.h"
#include "ui_schema2zoomtoolbar.h"

QList<double> Schema2ZoomToolbar::mScales = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1};
QStringList Schema2ZoomToolbar::mScalesText = {"10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%"};

Schema2ZoomToolbar::Schema2ZoomToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Schema2ZoomToolbar), mScaleIndex(mScales.size() - 1)
{
    ui->setupUi(this);
    ui->scale->addItems(mScalesText);
    ui->scale->setCurrentIndex(mScaleIndex);
}

Schema2ZoomToolbar::~Schema2ZoomToolbar()
{
    delete ui;
}


void Schema2ZoomToolbar::on_zoomOut_clicked()
{
    int currentIndex = ui->scale->currentIndex();
    if (currentIndex - 1 > -1) {
        ui->scale->setCurrentIndex(currentIndex - 1);
    }
}

void Schema2ZoomToolbar::on_zoomIn_clicked()
{
    int currentIndex = ui->scale->currentIndex();
    if (currentIndex + 1 < mScales.size()) {
        ui->scale->setCurrentIndex(currentIndex + 1);
    }
}

void Schema2ZoomToolbar::on_scale_currentIndexChanged(int index)
{
    double scale = mScales[index];

    emit zoom(scale);
}
