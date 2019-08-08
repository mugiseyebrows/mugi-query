#ifndef DATAFORMAT_H
#define DATAFORMAT_H

class QComboBox;
#include <QString>

namespace DataFormat
{
    enum Format {
        Csv,
        Tsv,
        SqlInsert,
        SqlUpdate
    };

    enum ActionType {
        ActionNone,
        ActionCopy,
        ActionSave
    };

    void initComboBox(QComboBox* comboBox, bool onlySql = false);

    Format value(QComboBox* comboBox);

    QString extension(Format format);

    QString extensionsRegExp();

    QString fileDialogFilter(Format format);

}

#endif // DATAFORMAT_H
