#ifndef OUTPUTTYPE_H
#define OUTPUTTYPE_H

#include <QComboBox>

namespace OutputType
{
    enum Type {
        File,
        NewSession,
        CurrentSession,
        Clipboard
    };

    void initComboBox(QComboBox* comboBox);

    Type value(QComboBox* comboBox);

};

#endif // OUTPUTTYPE_H
