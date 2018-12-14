#ifndef DISPLAYFORMAT_H
#define DISPLAYFORMAT_H

class QRadioButton;

namespace DisplayFormat
{
    enum Locale{
        C,
        Current,
        Special
    };

    void init(Locale format, QRadioButton* c, QRadioButton* current, QRadioButton* defined = 0);
    Locale value(QRadioButton* c, QRadioButton* current, QRadioButton* defined = 0);
}

#endif // DISPLAYFORMAT_H
