#include "displayformat.h"

#include <QRadioButton>

void DisplayFormat::init(Locale locale, QRadioButton *c, QRadioButton *current, QRadioButton *special)
{
    if (locale == C) {
        c->setChecked(true);
    } else if (locale == Current) {
        current->setChecked(true);
    } else if (locale == Special && special) {
        special->setChecked(true);
    }
}

DisplayFormat::Locale DisplayFormat::value(QRadioButton *c, QRadioButton *current, QRadioButton *special)
{
    if (c->isChecked()) {
        return C;
    } else if (current->isChecked()) {
        return Current;
    } else if (special && special->isChecked()) {
        return Special;
    }
}
