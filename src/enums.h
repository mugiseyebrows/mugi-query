#ifndef ENUMS_H
#define ENUMS_H

enum class CopyFormat {
    Table,
    List,
    Condition,
    Names,
    Insert
};

enum class TableType {
    Undefined,
    Table,
    View,
};

enum class IfExists {
    Fail,
    Replace,
    Append,
};

#endif // ENUMS_H
