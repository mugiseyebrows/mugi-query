#ifndef TOOLS_H
#define TOOLS_H

#include <QSqlDatabase>
class Schema2Data;
class QWidget;
#include "enums.h"

class Tools
{
public:


    static void mysql(QSqlDatabase db, QWidget* widget);
    static void mysqldump(Schema2Data* data, QSqlDatabase db, QWidget* widget);

    static void pushCsv(QSqlDatabase db, const QString& path, const QString &name, IfExists ifExists, QWidget *widget);

};

#endif // TOOLS_H
