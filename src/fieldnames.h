#ifndef FIELDNAMES_H
#define FIELDNAMES_H

#include <QStringList>
class QSqlRecord;

QStringList fieldNames(const QSqlRecord& record);

#endif // FIELDNAMES_H
