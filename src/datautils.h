#ifndef DATAUTILS_H
#define DATAUTILS_H

#include <QPolygonF>
#include <QVariantList>
#include <QPair>
#include <QStringList>
class QAbstractItemModel;
class QSqlDatabase;

namespace DataUtils {

QPolygonF toPolygon(const QList<QPair<QVariant,QVariant> >& data);
//QList<QPair<QVariant,QVariant> > filterNull(const QList<QPair<QVariant,QVariant> >& data);
QVariantList filterNumeric(const QVariantList& data);
QList<QPair<QVariant,QVariant> > filterNumeric(const QList<QPair<QVariant,QVariant> >& data);

QVariantList columnData(const QAbstractItemModel* model,int column);
QStringList toLower(const QStringList& vs);
QStringList headerData(const QAbstractItemModel* model, Qt::Orientation orientation);
QStringList filterEmpty(const QStringList& items);
QList<double> toDouble(const QVariantList& vs);
QString windowTitle(const QString& prefix, const QSqlDatabase& db, const QString suffix);

}

#endif // DATAUTILS_H
