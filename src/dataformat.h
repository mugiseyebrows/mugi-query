#ifndef DATAFORMAT_H
#define DATAFORMAT_H

class QComboBox;
#include <QString>

namespace DataFormat
{
    enum Format {
        Csv,
        Tsv,
        Xlsx,
        Json,
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

    Format value(const QString& text);

    QString name(Format format);

    QString extension(Format format);

    QString extensionsRegExp();

    QString fileDialogFilter(Format format);

}

#endif // DATAFORMAT_H
