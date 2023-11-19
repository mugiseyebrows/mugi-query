#ifndef SCHEMA2TABLECOLUMN_H
#define SCHEMA2TABLECOLUMN_H

#include <QString>

class Schema2TableColumn {
public:
    Schema2TableColumn();
    Schema2TableColumn(const QString& name, const QString& type, bool notNull, bool autoincrement);
    QString name;
    QString type;
    bool notNull;
    bool autoincrement;
};

#endif // SCHEMA2TABLECOLUMN_H
