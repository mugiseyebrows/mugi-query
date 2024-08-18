#ifndef SCHEMA2TABLECOLUMN_H
#define SCHEMA2TABLECOLUMN_H

#include <QString>

#if 0

class Schema2TableColumn {
public:
    Schema2TableColumn();
    Schema2TableColumn(const SColumn& column);
    QString name;
    QString type;
    bool notNull;
    QString default_;
    bool autoincrement;
};

#endif

#endif // SCHEMA2TABLECOLUMN_H
