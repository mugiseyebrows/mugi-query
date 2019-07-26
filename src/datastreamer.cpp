#include "datastreamer.h"

#include <QDebug>
#include <QTextStream>
#include <QSqlQueryModel>

#include <QDate>
#include <QDateTime>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QSqlField>

#include "sqldatatypes.h"
#include "settings.h"

namespace {

QString escapeChars(const QString& value) {
    QString result = value;
    result.replace("'","''").replace("\\","\\\\").replace("\n","\\n").replace("\r","\\r").replace("\t","\\t");
    return result;
}

QStringList filterHeader(QAbstractItemModel *model, const QList<bool>& filter) {
    QStringList res;
    for(int c=0; c<model->columnCount(); c++) {
        if (filter[c]) {
            res << model->headerData(c,Qt::Horizontal).toString();
        }
    }
    return res;
}

QVariantList filterData(QAbstractItemModel *model, int row, const QList<bool>& filter) {
    QVariantList res;
    for(int c=0; c<model->columnCount(); c++) {
        if (filter[c]) {
            res << model->data(model->index(row,c));
        }
    }
    return res;
}

QVariantList modelRow(QAbstractItemModel *model, int row) {
    QVariantList res;
    for(int c=0; c<model->columnCount(); c++) {
        res << model->data(model->index(row,c));
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


bool allAreNull(const QVariantList& vs) {
    foreach (const QVariant& v, vs) {
        if (!v.isNull()) {
            return false;
        }
    }
    return true;
}

}

QStringList DataStreamer::variantListToStringList(const QVariantList& values,
                                                  DataFormat::Format format,
                                                  const Formats& formats,
                                                  const QLocale& locale,
                                                  QString& error) {
    QStringList res;
    foreach(const QVariant& value, values) {
        res.append(variantToString(value,format,formats,locale, error));
        if (!error.isEmpty()) {
            return QStringList();
        }
    }
    return res;
}

QString DataStreamer::variantToString(const QVariant& value,
                                      DataFormat::Format format,
                                      const Formats& formats,
                                      const QLocale& locale,
                                      QString& error) {

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
            error = QString("DataStreamer::variantToString(format == %2) is not defined for value.type() == %1").arg(value.type()).arg(format);
            return QString();
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
            return "'" + escapeChars(value.toString()) + "'";
        case QVariant::ByteArray:
            return "0x" + value.toByteArray().toHex();
        default:
            error = QString("DataStreamer::variantToString(format == %2) is not defined for value.type() == %1").arg(value.type()).arg(format);
            return QString();
        }
    } else {
        error = QString("variantToString not implemented for format").arg(format);
        return QString();
    }
    return QString();
}

QSqlRecord createDataRecord(const QStringList& columns,
                      const QStringList& types) {

    QMap<QString,QVariant::Type> m = SqlDataTypes::mapToVariant();
    QSqlRecord record;
    for(int c=0;c<columns.size();c++) {
        if (columns[c].isEmpty()) {
            continue;
        }
        record.append(QSqlField(columns[c],m[types[c]]));
    }
    return record;
}

QString DataStreamer::stream(DataFormat::Format format,
                             const QSqlDatabase& db,
                             QAbstractItemModel* model,
                             int rowCount,
                             const QString &table,
                             const QStringList& columns,
                             const QStringList& types,
                             int dataColumns,
                             const QLocale& locale,
                             bool* hasMore,
                             QString& error) {

    QSqlDriver* driver = db.driver();

    QString result;

    QTextStream stream(&result);

    QList<bool> filter;
    for(int i=0;i<model->columnCount();i++) {
        filter << (!columns.value(i).isEmpty());
    }

    QMap<QString,QVariant::Type> m = SqlDataTypes::mapToVariant();

    QSqlRecord dataRecord = createDataRecord(columns.mid(0,dataColumns), types.mid(0,dataColumns));
    QSqlRecord keysRecord = createDataRecord(columns.mid(dataColumns),types.mid(dataColumns));

    *hasMore = false;

    int nonEmpty = 0;

    for(int r=0;r < model->rowCount();r++) {

        //QVariantList rowValues = filterData(model,r,filter);

        bool empty = true;

        for(int c=0;c<columns.size();c++) {
            if (columns[c].isEmpty()) {
                continue;
            }
            QVariant v = SqlDataTypes::tryConvert(model->data(model->index(r,c)), m[types[c]], locale);
            if (!v.isNull()) {
                empty = false;
            }
            if (c < dataColumns) {
                dataRecord.setValue(columns[c],v);
            } else {
                keysRecord.setValue(columns[c],v);
            }
        }

        if (empty) {
            continue;
        }

        if (nonEmpty == rowCount) {
            *hasMore = true;
            break;
        }

        QString statement;

        if (format == DataFormat::SqlInsert) {
            statement = driver->sqlStatement(QSqlDriver::InsertStatement,table,dataRecord,false);
        } else if (format == DataFormat::SqlUpdate) {
            statement = driver->sqlStatement(QSqlDriver::UpdateStatement,table,dataRecord,false) + " " +
                    driver->sqlStatement(QSqlDriver::WhereStatement,table,keysRecord,false);
        } else {
            qDebug() << "DataStreamer::stream error: not sql format";
        }

        stream << statement << ";\n";

        nonEmpty++;
    }
    return result;
}


static QString prepareIdentifier(const QString &identifier,
        QSqlDriver::IdentifierType type, const QSqlDriver *driver)
{
    Q_ASSERT( driver != NULL );
    QString ret = identifier;
    if (!driver->isIdentifierEscaped(identifier, type)) {
        ret = driver->escapeIdentifier(identifier, type);
    }
    return ret;
}

#if 0
QString DataStreamer::multiInsert(const QSqlDatabase &db, const QString& tableName, const QList<QSqlRecord>& records) {

    QSqlDriver* driver = db.driver();

    QString head = QLatin1String("INSERT INTO ").append(tableName);
    QStringList fieldNames;
    QStringList vals;

    const QSqlRecord& rec = records[0];

    for (i = 0; i < rec.count(); ++i) {
        if (!rec.isGenerated(i))
            continue;
        fieldNames << prepareIdentifier(rec.fieldName(i), QSqlDriver::FieldName, driver);
        vals << driver->formatValue(rec.field(i));
    }
    if (vals.isEmpty()) {
        s.clear();
    } else {
        vals.chop(2); // remove trailing comma
        s[s.length() - 2] = QLatin1Char(')');
        s.append(QLatin1String("VALUES (")).append(vals).append(QLatin1Char(')'));
    }

}
#endif

QString DataStreamer::createTableStatement(const QSqlDatabase &db, const QString &table,
                                           const QStringList &columns, const QStringList &types,
                                           const QList<bool>& primaryKey,
                                           bool ifNotExists)
{
    QSqlDriver* driver = db.driver();

    QMap<QString,QVariant::Type> variant = SqlDataTypes::mapToVariant();
    QMap<QVariant::Type,QString> specific = SqlDataTypes::mapToDriver(db.driverName());

    QStringList typed;
    QStringList keys;
    for(int c=0;c<columns.size();c++) {
        if (columns[c].isEmpty()) {
            continue;
        }
        QString identifier = driver->escapeIdentifier(columns[c],QSqlDriver::FieldName);
        typed << QString("%1 %2")
                 .arg(identifier)
                 .arg(specific[variant[types[c]]]);
        if (primaryKey[c]) {
            keys << identifier;
        }
    }

    if (!keys.isEmpty()) {
        typed << QString("PRIMARY KEY (%1)").arg(keys.join(","));
    }

    QString statement = QString("CREATE TABLE %1(%2)")
            .arg(driver->escapeIdentifier(table, QSqlDriver::TableName))
            .arg(typed.join(", "));

    return statement;
}

void DataStreamer::stream(const QSqlDatabase& db, QTextStream &stream, QSqlQueryModel *model,  DataFormat::Format format,
                          const QString &table, QList<bool> data, QList<bool> keys,
                          DataFormat::ActionType action, const QLocale& locale, QString& error)
{
    Formats formats(action);

    if (format == DataFormat::Csv || format == DataFormat::Tsv) {

        QString sep = format == DataFormat::Csv ? ";" : "\t";

        stream << filterHeader(model,data).join(sep) << "\n";
        for(int r=0; r<model->rowCount(); r++) {
            stream << variantListToStringList(filterData(model,r,data),format,formats,locale,error).join(sep) << "\n";
            if (!error.isEmpty()) {
                return;
            }
        }

    } else if (format == DataFormat::SqlInsert) {

        QSqlDriver* driver = db.driver();

        QSqlRecord insertRecord;

        for(int c=0;c<model->columnCount();c++) {
            QSqlRecord record = model->record(0);
            if (data[c]) {
                insertRecord.append(record.field(c));
            }
        }

        for(int r=0;r<model->rowCount();r++) {
            QSqlRecord record = model->record(r);
            for(int c=0;c<model->columnCount();c++) {
                if (data[c]) {
                    insertRecord.setValue(record.fieldName(c),record.value(c));
                }
            }
            QString statement = driver->sqlStatement(QSqlDriver::InsertStatement,table,insertRecord,false);
            stream << statement << ";\n";
        }

    } else if (format == DataFormat::SqlUpdate) {

        QSqlDriver* driver = db.driver();

        QSqlRecord update;
        QSqlRecord where;

        for(int c=0;c<model->columnCount();c++) {
            QSqlRecord record = model->record(0);
            if (data[c]) {
                update.append(record.field(c));
            }
            if (keys[c]) {
                where.append(record.field(c));
            }
        }

        for(int r=0;r<model->rowCount();r++) {
            QSqlRecord record = model->record(r);
            for(int c=0;c<model->columnCount();c++) {
                if (data[c]) {
                    update.setValue(record.fieldName(c),record.value(c));
                }
                if (keys[c]) {
                    where.setValue(record.fieldName(c),record.value(c));
                }
            }
            QString update_ = driver->sqlStatement(QSqlDriver::UpdateStatement,table,update,false);
            QString where_ = driver->sqlStatement(QSqlDriver::WhereStatement,table,where,false);
            stream << update_ << " " << where_ << ";\n";
        }


    }


}
