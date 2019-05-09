#include "dirstibutionplotoptionsedit.h"
#include "ui_dirstibutionplotoptionsedit.h"

#include <math.h>
#include <QDebug>

DirstibutionPlotOptionsEdit::DirstibutionPlotOptionsEdit(QWidget *parent) :
    QWidget(parent),
    mSwitchMode(true),
    ui(new Ui::DirstibutionPlotOptionsEdit)
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
    int prec = qMax(0, (int) log10(500.0/(vmax-vmin)));
    ui->minAuto->setValue(vmin,prec);
    ui->minManual->setIfNoValue(vmin,prec);
    ui->maxAuto->setValue(vmax,prec);
    ui->maxManual->setIfNoValue(vmax,prec);
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

    bool ok[3];
    int bins = this->bins(ok);
    double min = this->min(ok + 1);
    double max = this->max(ok + 2);
    if (!ok[0] || !ok[1] || !ok[2]) {
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

void DirstibutionPlotOptionsEdit::on_autoRange_clicked()
{
    onValuesChanged(SenderRangeSelect);
}

void DirstibutionPlotOptionsEdit::on_manualRange_clicked()
{
    onValuesChanged(SenderRangeSelect);
}
