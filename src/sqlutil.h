#ifndef SQLUTIL_H
#define SQLUTIL_H


#include <QSqlDatabase>
#include <QVariantList>
class QStringList;
class QString;

namespace sql {

QList<QVariantList> select_where(QSqlDatabase db, const QString& table, const QStringList& columns, const QString& whereExrp, const QVariantList& values);

void delete_where(QSqlDatabase db, const QString& table, const QString& whereExpr, const QVariantList& binds);

void insert(QSqlDatabase db, const QString& table, const QStringList& columns, const QVariantList& values);

void create_or_alter(QSqlDatabase db,
                                const QString& table,
                                const QStringList& columns,
                                const QString& primaryKey = QString());

}




#endif // SQLUTIL_H
