#include "doublelineedit.h"

#include <math.h>
#include <QDebug>

DoubleLineEdit::DoubleLineEdit(QWidget *parent, int prec) :
    QLineEdit(parent), mPrec(prec)
{
    connect(this,SIGNAL(textChanged(QString)),this,SLOT(onTextChanged(QString)));
}

double DoubleLineEdit::value(bool *ok) const
{
    return text().toDouble(ok);
}

void DoubleLineEdit::onTextChanged(QString text)
{
    if (text.isEmpty()) {
        return;
    }
    bool ok;
    double value = text.toDouble(&ok);
    if (!ok) {
        return;
    }
    emit valueChanged(value);
}

void DoubleLineEdit::setIfNoValue(double value, int prec) {
    bool ok;
    this->value(&ok);
    if (ok) {
        return;
    }
    setValue(value,prec);
}

void DoubleLineEdit::setValue(double value, int prec)
{
    bool ok;
    double value_ = this->value(&ok);
    if (prec == mPrec || prec == -1) {
        double eps = pow(10.0,-(mPrec+1));
        if (fabs(value_ - value) < eps) {
            return;
        }
    } else if (prec != -1){
        mPrec = prec;
    }
    setText(QString::number(value,'f',mPrec));
}

void DoubleLineEdit::setPrec(int value)
{
    if (value == mPrec) {
        return;
    }
    mPrec = value;
    setValue(this->value());
}

int DoubleLineEdit::prec() const
{
    return mPrec;
}
