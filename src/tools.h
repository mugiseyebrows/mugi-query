#ifndef TOOLS_H
#define TOOLS_H

#include <QSqlDatabase>
class Schema2Data;
class QWidget;

class Tools
{
public:
    static void mysql(QSqlDatabase db, QWidget* widget);
    static void mysqldump(Schema2Data* data, QSqlDatabase db, QWidget* widget);
};

#endif // TOOLS_H
