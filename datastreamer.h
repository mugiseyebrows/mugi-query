#ifndef DATASTREAMER_H
#define DATASTREAMER_H

#include <QVariant>
#include <QString>

class QSqlQueryModel;
class QTextStream;

#include "dataformat.h"

class DataStreamer
{
public:
    DataStreamer();
    static QString variantToString(const QVariant &value, DataFormat::Format format);

    static void stream(QTextStream& stream, QSqlQueryModel* model, DataFormat::Format format, const QString& table,
                 QList<bool> data, QList<bool> keys);

    static QStringList variantListToStringList(const QVariantList &values, DataFormat::Format format);
};

#endif // DATASTREAMER_H
