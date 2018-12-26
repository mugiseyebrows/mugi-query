#include "datastreamer.h"

#include <QDebug>
#include <QTextStream>
#include <QSqlQueryModel>

#include <QDate>
#include <QDateTime>

#include "settings.h"


QStringList DataStreamer::variantListToStringList(const QVariantList& values,
                                                  DataFormat::Format format,
                                                  const Formats& formats,
                                                  const QLocale& locale) {
    QStringList res;
    foreach(const QVariant& value, values) {
        res << variantToString(value,format,formats,locale);
    }
    return res;
}

QString DataStreamer::variantToString(const QVariant& value,
                                      DataFormat::Format format,
                                      const Formats& formats,
                                      const QLocale& locale) {

    QString t;

    if (format == DataFormat::Csv || format == DataFormat::Tsv) {
        if (value.isNull()) {
            return "";
        }
        switch(value.type()) {
        case QVariant::Int:
            return QString::number(value.toInt());
        case QVariant::UInt:
            return QString::number(value.toUInt());
        case QVariant::LongLong:
            return QString::number(value.toLongLong());
        case QVariant::ULongLong:
            return QString::number(value.toULongLong());
        case QVariant::Double:
            if (formats.realUseLocale) {
                return locale.toString(value.toDouble());
            } else {
                return QString::number(value.toDouble());
            }
        case QVariant::Bool:
            return QString::number(value.toInt());
        case QVariant::Date:
        {
            QDate value_ = value.toDate();
            if (formats.dateTimeUseLocale) {
                return locale.toString(value_,QLocale::ShortFormat);
            }
            return value_.toString(formats.dateFormat);
        }
        case QVariant::DateTime:
        {
            QDateTime value_ = value.toDateTime();
            if (formats.dateTimeUseLocale) {
                return locale.toString(value_,QLocale::ShortFormat);
            }
            return value_.toString(formats.dateFormat + " " + formats.timeFormat);
        }
        case QVariant::Time:
        {
            QTime value_ = value.toTime();
            if (formats.dateTimeUseLocale) {
                return locale.toString(value_,QLocale::ShortFormat);
            }
            return value_.toString(formats.timeFormat);
        }
        case QVariant::String:
            return value.toString().replace(QRegExp("\\n[\\r]?\\s*")," ");
        case QVariant::ByteArray:
            return "0x" + value.toByteArray().toHex();
        default:
            qDebug() << QString("DataStreamer::variantToString(format == %2) is not defined for value.type() == %1").arg(value.type()).arg(format);
            return value.toString();
        }
    } else if (format == DataFormat::SqlInsert || format == DataFormat::SqlUpdate) {
        if (value.isNull()) {
            return "null";
        }
        switch(value.type()) {
        case QVariant::Bool:
            return QString::number(value.toInt());
        case QVariant::UInt:
            return QString::number(value.toUInt());
        case QVariant::Int:
            return QString::number(value.toInt());
        case QVariant::ULongLong:
            return QString::number(value.toULongLong());
        case QVariant::LongLong:
            return QString::number(value.toLongLong());
        case QVariant::Double:
            return QString::number(value.toDouble());
        case QVariant::Date:
            return "'" + value.toDate().toString("yyyy-MM-dd") + "'";
        case QVariant::DateTime:
            return "'" + value.toDateTime().toString("yyyy-MM-dd hh:mm:ss") + "'";
        case QVariant::String:
            return "'" + value.toString().replace("'","''").replace("\\","\\\\") + "'";
        case QVariant::ByteArray:
            return "0x" + value.toByteArray().toHex();
        default:
            qDebug() << QString("DataStreamer::variantToString(format == %2) is not defined for value.type() == %1").arg(value.type()).arg(format);
            return value.toString();
        }
    } else {
        qDebug() << "variantToString not implemented for format" << format;
    }
    return QString();
}

namespace {

QStringList filterHeader(QSqlQueryModel *model, const QList<bool>& filter) {
    QStringList res;
    for(int c=0; c<model->columnCount(); c++) {
        if (filter[c]) {
            res << model->headerData(c,Qt::Horizontal).toString();
        }
    }
    return res;
}

QVariantList filterData(QSqlQueryModel *model, int row, const QList<bool>& filter) {
    QVariantList res;
    for(int c=0; c<model->columnCount(); c++) {
        if (filter[c]) {
            res << model->data(model->index(row,c));
        }
    }
    return res;
}

QStringList zipJoin(const QStringList& vs1, const QString& glue, const QStringList vs2) {
    QStringList res;
    for(int i=0;i<vs1.size();i++) {
        res << vs1[i] + glue + vs2[i];
    }
    return res;
}

QStringList zipJoinNull(const QStringList& vs1, const QStringList vs2) {
    QStringList res;
    for(int i=0;i<vs1.size();i++) {
        if (vs2[i] == "null") {
            res << vs1[i] + " is " + vs2[i];
        } else {
            res << vs1[i] + "=" + vs2[i];
        }
    }
    return res;
}

}

void DataStreamer::stream(QTextStream &stream, QSqlQueryModel *model, DataFormat::Format format,
                          const QString &table, QList<bool> data, QList<bool> keys,
                          DataFormat::ActionType action, const QLocale& locale)
{
    Formats formats(action);

    if (format == DataFormat::Csv || format == DataFormat::Tsv) {

        QString sep = format == DataFormat::Csv ? ";" : "\t";

        stream << filterHeader(model,data).join(sep) << "\n";
        for(int r=0; r<model->rowCount(); r++) {
            stream << variantListToStringList(filterData(model,r,data),format,formats,locale).join(sep) << "\n";
        }

    } else if (format == DataFormat::SqlInsert) {

        QString columns = filterHeader(model, data).join(",");
        for(int r=0;r<model->rowCount();r++) {
            QString values = variantListToStringList(filterData(model,r,data),format,formats,locale).join(",");
            stream << "insert into " << table << "(" << columns << ") "
                   << "values(" << values << ");\n";
        }

    } else if (format == DataFormat::SqlUpdate) {

        QStringList columns1 = filterHeader(model, data);
        QStringList columns2 = filterHeader(model, keys);

        for(int r=0;r<model->rowCount();r++) {

            QStringList values1 = variantListToStringList(filterData(model,r,data),format,formats,locale);
            QStringList values2 = variantListToStringList(filterData(model,r,keys),format,formats,locale);

            stream << "update " << table << " set "
                   << zipJoin(columns1,"=",values1).join(", ")
                   << " where " << zipJoinNull(columns2,values2).join(" and ") << ";\n";
        }

    }


}
