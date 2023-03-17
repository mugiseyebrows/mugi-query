#include "fieldnames.h"

#include <QStringList>
#include <QSqlRecord>

QStringList fieldNames(const QSqlRecord& record) {
    QStringList res;
    for(int i=0;i<record.count();i++) {
        res.append(record.fieldName(i));
    }
    return res;
}
