#ifndef DATAFORMAT_H
#define DATAFORMAT_H

class QComboBox;

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

    void initComboBox(QComboBox* comboBox);

    Format value(QComboBox* comboBox);

}

#endif // DATAFORMAT_H
