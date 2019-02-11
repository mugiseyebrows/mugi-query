#ifndef OUTPUTTYPE_H
#define OUTPUTTYPE_H

#include <QComboBox>

namespace OutputType
{
    enum Type {
        File,
        Session,
        Clipboard
    };

    void initComboBox(QComboBox* comboBox);

    Type value(QComboBox* comboBox);

};

#endif // OUTPUTTYPE_H
