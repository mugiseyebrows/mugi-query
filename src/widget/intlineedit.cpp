#include "intlineedit.h"

IntLineEdit::IntLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    connect(this,SIGNAL(textChanged(QString)),this,SLOT(onTextChanged(QString)));
}

int IntLineEdit::value(bool *ok) const
{
    return text().toInt(ok);
}

void IntLineEdit::setIfNoValue(int value) {
    bool ok;
    this->value(&ok);
    if (ok) {
        return;
    }
    setValue(value);
}

void IntLineEdit::setValue(int value)
{
    bool ok;
    if (this->value(&ok) == value) {
        if (ok) {
            return;
        }
    }
    setText(QString::number(value));
}

void IntLineEdit::onTextChanged(QString text)
{
    if (text.isEmpty()) {
        return;
    }
    bool ok;
    int value = this->value(&ok);
    if (!ok) {
        return;
    }
    emit valueChanged(value);
}
