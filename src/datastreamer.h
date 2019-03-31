#ifndef DATASTREAMER_H
#define DATASTREAMER_H

#include <QVariant>
#include <QString>

class QSqlQueryModel;
class QTextStream;

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
};


#endif // DATASTREAMER_H
