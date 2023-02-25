#ifndef TOOLS_H
#define TOOLS_H

#include <QSqlDatabase>
class QWidget;

namespace Tools
{
    void mysql(QSqlDatabase db, QWidget* widget);
    void mysqldump(QSqlDatabase db, QWidget* widget);
}

#endif // TOOLS_H
