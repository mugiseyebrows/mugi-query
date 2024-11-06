#ifndef TOOLS_H
#define TOOLS_H

#include <QSqlDatabase>
class QWidget;

class Tools
{
public:
    static void mysql(QSqlDatabase db, QWidget* widget);
    static void mysqldump(QSqlDatabase db, QWidget* widget);
};

#endif // TOOLS_H
