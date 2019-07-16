#ifndef DATASTREAMER_H
#define DATASTREAMER_H

#include <QVariant>
#include <QString>

class QSqlQueryModel;
class QTextStream;
class QAbstractItemModel;

#include "dataformat.h"
#include "formats.h"

class DataStreamer
{
public:


    static void stream(QTextStream& stream, QSqlQueryModel* model, DataFormat::Format format, const QString& table,
                 QList<bool> data, QList<bool> keys, DataFormat::ActionType action, const QLocale &locale, QString &error);

    static QStringList variantListToStringList(const QVariantList &values,
                                               DataFormat::Format format,
                                               const Formats &formats,
                                               const QLocale &locale,
                                               QString& error);
    static QString variantToString(const QVariant &value, DataFormat::Format format,
                                   const Formats& formats, const QLocale& locale,
                                   QString& error);

    static QString stream(QAbstractItemModel *model, int rowCount, const QString &table,
                          const QStringList &columns, const QStringList &types, DataFormat::Format format, const QLocale &locale,
                          QString& error);
};


#endif // DATASTREAMER_H
