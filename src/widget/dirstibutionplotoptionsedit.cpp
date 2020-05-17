#include "dirstibutionplotoptionsedit.h"
#include "ui_dirstibutionplotoptionsedit.h"

#include <math.h>
#include <QDebug>

DirstibutionPlotOptionsEdit::DirstibutionPlotOptionsEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DirstibutionPlotOptionsEdit),
    mSwitchMode(true),
    mPrec(0)
{
    ui->setupUi(this);

    connect(ui->bins,&IntLineEdit::valueChanged,[=](int){onValuesChanged(SenderBins);});
    connect(ui->minManual,&DoubleLineEdit::valueChanged,[=](double){onValuesChanged(SenderManualRange);});
    connect(ui->maxManual,&DoubleLineEdit::valueChanged,[=](double){onValuesChanged(SenderManualRange);});
    connect(ui->minAuto,&DoubleLineEdit::valueChanged,[=](double){onValuesChanged(SenderAutoRange);});
    connect(ui->maxAuto,&DoubleLineEdit::valueChanged,[=](double){onValuesChanged(SenderAutoRange);});

    connect(ui->div2,&QPushButton::clicked,[=](){ ui->bins->setValue(qMax(2,ui->bins->value() / 2)); });
    connect(ui->mul2,&QPushButton::clicked,[=](){ ui->bins->setValue(ui->bins->value() * 2); });
}

DirstibutionPlotOptionsEdit::~DirstibutionPlotOptionsEdit()
{
    delete ui;
}

void DirstibutionPlotOptionsEdit::init(int bins, double vmin, double vmax)
{
    mSwitchMode = false;
    ui->bins->setIfNoValue(bins);
    mPrec = vmin == vmax ? 0 : qMax(0, (int) log10(500.0/(vmax-vmin)));
    ui->minAuto->setValue(vmin,mPrec);
    ui->minManual->setIfNoValue(vmin,mPrec);
    ui->maxAuto->setValue(vmax,mPrec);
    ui->maxManual->setIfNoValue(vmax,mPrec);
    mSwitchMode = true;
}

void DirstibutionPlotOptionsEdit::onValuesChanged(Sender sender) {

    if (sender == SenderManualRange && autoRange()) {
        if (mSwitchMode) {
            ui->manualRange->setChecked(true);
            on_manualRange_clicked();
        }
        return;
    }
    if (sender == SenderAutoRange && !autoRange()) {
        return;
    }

    bool ok1,ok2,ok3;
    int bins = this->bins(&ok1);
    double min = this->min(&ok2);
    double max = this->max(&ok3);
    if (!(ok1 && ok2 && ok3) || min == max) {
        return;
    }

    emit valuesChanged(bins,min,max);
}

bool DirstibutionPlotOptionsEdit::autoRange() const
{
    return ui->autoRange->isChecked();
}

int DirstibutionPlotOptionsEdit::bins(bool* ok) const
{
    return ui->bins->value(ok);
}

double DirstibutionPlotOptionsEdit::min(bool* ok) const
{
    DoubleLineEdit* edit = autoRange() ? ui->minAuto : ui->minManual;
    return edit->value(ok);
}

double DirstibutionPlotOptionsEdit::max(bool* ok) const
{
    DoubleLineEdit* edit = autoRange() ? ui->maxAuto : ui->maxManual;
    return edit->value(ok);
}

void DirstibutionPlotOptionsEdit::setManualRange(double vmin, double vmax)
{
    ui->minManual->setValue(vmin, mPrec);
    ui->maxManual->setValue(vmax, mPrec);
}

void DirstibutionPlotOptionsEdit::setNAuto(int value)
{
    ui->nAuto->setValue(value);
}

void DirstibutionPlotOptionsEdit::setNManual(int value)
{
    ui->nManual->setValue(value);
}

void DirstibutionPlotOptionsEdit::on_autoRange_clicked()
{
    onValuesChanged(SenderRangeSelect);
}

void DirstibutionPlotOptionsEdit::on_manualRange_clicked()
{
    onValuesChanged(SenderRangeSelect);
}
