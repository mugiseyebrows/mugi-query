#include "datastreamer.h"

#include <QDebug>
#include <QTextStream>
#include <QSqlQueryModel>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTextCodec>

#include <QDate>
#include <QDateTime>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QSqlField>
#include <QRegularExpression>

#include "sqldatatypes.h"
#include "settings.h"
#include "drivernames.h"

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

QString spaced(const QString& s) {
    if (s.isEmpty()) {
        return " ";
    }
    return " " + s + " ";
}

QString jsonToString(const QJsonObject& v) {
    QJsonDocument document;
    document.setObject(v);
    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    return codec->toUnicode(document.toJson());
}

QString joinNonEmpty(const QStringList& values, const QString& glue) {
    QStringList res;
    foreach(const QString& value, values) {
        if (!value.isEmpty()) {
            res << value;
        }
    }
    return res.join(glue);
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

        /*{
            auto* codec = QTextCodec::codecForName("UTF-8");
            auto* decoder = codec->makeDecoder();
            QString text = decoder->toUnicode(value.toByteArray());
            if (!decoder->hasFailure()) {
                return text;
            }
        }*/

            return "0x" + value.toByteArray().toHex();

        case QVariant::List:
        {
            QVariantList vs = value.toList();
            QVariant v1 = vs.value(0);
            QVariant v2 = vs.value(1);
            return QString("%1 -> %2")
                    .arg(variantToString(v1, format, formats, locale, error))
                    .arg(variantToString(v2, format, formats, locale, error));
            break;
        }

        default:
            error = QString("DataStreamer::variantToString(format == %2) is not defined for value.type() == %1").arg(value.type()).arg(format);
            return QString();
        }

    } else if (format == DataFormat::Json) {
        return variantToJson(value).toString();
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

QSqlRecord createDataRecord(const QList<Field>& fields) {
    QMap<QString,QVariant::Type> m = SqlDataTypes::mapToVariant();
    QSqlRecord record;

    foreach(const Field& field, fields) {
        if (field.name().isEmpty()) {
            continue;
        }
        record.append(QSqlField(field.name(),m[field.type()]));
    }
    return record;
}

QJsonValue DataStreamer::variantToJson(const QVariant& v) {
    QVariant::Type type = v.type();
    if (v.isNull()) {
        return QJsonValue();
    } else if (type == QVariant::Int) {
        return v.toInt();
    } else if (type == QVariant::Double) {
        return v.toDouble();
    } else if (type == QVariant::String) {
        return v.toString();
    } else if (type == QVariant::Date || type == QVariant::DateTime) {
        return v.toDateTime().toString(Qt::ISODateWithMs);
    } else if (type == QVariant::Time) {
        return v.toTime().toString("hh:mm:ss.zzz");
    } else if (type == QVariant::ByteArray) {
        QJsonArray res;
        QByteArray d = v.toByteArray();
        for(int i=0;i<d.size();i++) {
            res.append((uint8_t) d[i]);
        }
        return res;
    }
    return QJsonValue();
}

QString DataStreamer::streamJson(const QSqlDatabase& db,
                             QAbstractItemModel* model,
                             int rowCount,
                             const QString &table,
                             const QList<Field>& fields,
                             int dataColumns,
                             int minYear,
                             bool inLocal,
                             bool outUtc,
                             const QLocale& locale,
                             bool* hasMore,
                             QString& error) {

    QJsonArray data;
    QMap<QString,QVariant::Type> m = SqlDataTypes::mapToVariant();
    *hasMore = false;
    int nonEmpty = 0;
    for(int r=0;r<model->rowCount();r++) {
        bool empty = true;
        QJsonObject record;
        for(int c=0;c<fields.size();c++) {
            const Field& field = fields[c];
            if (field.name().isEmpty()) {
                continue;
            }
            QModelIndex index = model->index(r,c);
            bool ok = false;

            QVariant::Type type = m[field.type()];

            QVariant v = SqlDataTypes::tryConvert(model->data(index),type,
                                                  locale, minYear, inLocal, outUtc, &ok);
            if (!v.isNull()) {
                empty = false;
            }
            record[field.name()] = variantToJson(v);
        }

        if (empty) {
            continue;
        }

        if (nonEmpty == rowCount) {
            *hasMore = true;
            break;
        }

        data.append(record);
        nonEmpty++;
    }

    QJsonObject root;
    root["table"] = table;
    root["data"] = data;
    return jsonToString(root);
}

QString DataStreamer::stream(DataFormat::Format format,
                             const QSqlDatabase& db,
                             QAbstractItemModel* model,
                             int rowCount,
                             const QString &table,
                             const QList<Field>& fields,
                             int dataColumns,
                             int minYear,
                             bool inLocal,
                             bool outUtc,
                             const QLocale& locale,
                             bool* hasMore,
                             QString& error) {

#if 0
    if (format == DataFormat::Json) {
        return streamJson(db, model, rowCount, table, fields, dataColumns, minYear, inLocal, outUtc, locale, hasMore, error);
    }
#endif

    QSqlDriver* driver = db.driver();

    QString result;

    QTextStream stream(&result);

    QMap<QString,QVariant::Type> m = SqlDataTypes::mapToVariant();


    QSqlRecord keysRecord = createDataRecord(fields.mid(dataColumns));

    *hasMore = false;

    int nonEmpty = 0;

    for(int r=0;r < model->rowCount();r++) {


        QSqlRecord dataRecord = createDataRecord(fields.mid(0,dataColumns));

        bool empty = true;

        for(int c=0;c<fields.size();c++) {
            const Field& field = fields[c];
            if (field.name().isEmpty()) {
                continue;
            }
            QModelIndex index = model->index(r,c);
            bool ok = false;
            QVariant v = SqlDataTypes::tryConvert(model->data(index),
                        m[field.type()], locale, minYear, inLocal, outUtc, &ok);
            if (!v.isNull()) {
                empty = false;
            }
            if (c < dataColumns) {
                if (format == DataFormat::SqlInsert && field.autoincrement() && v.isNull()) {
                    int index = dataRecord.indexOf(field.name());
                    if (index > -1) {
                        dataRecord.remove(index);
                    }
                } else {
                    dataRecord.setValue(field.name(),v);
                }
            } else {
                keysRecord.setValue(field.name(),v);
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

#if 0
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


QStringList DataStreamer::createIndexStatements(const QSqlDatabase &db, const QString &table,
                                             const QList<Field>& fields) {

    QSqlDriver* driver = db.driver();
    QMap<QString,QVariant::Type> variant = SqlDataTypes::mapToVariant();
    QString driverName = db.driverName();
    QStringList result;
    for(int c=0;c<fields.size();c++) {
        const Field& field = fields[c];
        if (field.name().isEmpty() || (!field.unique() && !field.index())) {
            continue;
        }
        QVariant::Type type = variant[field.type()];
        QString indexType;
        if (field.unique()) {
            indexType = "UNIQUE";
        } else if (driverName == DRIVER_MYSQL && type == QVariant::String && field.size() < 0) {
            indexType = "FULLTEXT";
        }
        result << QString("CREATE" + spaced(indexType) + "INDEX %1 ON %2(%1)")
                  .arg(driver->escapeIdentifier(field.name(),QSqlDriver::FieldName))
                  .arg(driver->escapeIdentifier(table,QSqlDriver::TableName));
    }
    return result;

}


QString fixForeignKey(const QString& key) {
    QRegularExpression exp("^(.*)[.](.*)$");
    QRegularExpressionMatch m = exp.match(key);
    if (m.hasMatch()) {
        return QString("%1(%2)").arg(m.captured(1)).arg(m.captured(2));
    }
    return key;
}

QString DataStreamer::createTableStatement(const QSqlDatabase &db, const QString &table,
                                           const QList<Field>& fields,
                                           bool ifNotExists)
{
    QSqlDriver* driver = db.driver();

    QMap<QString,QVariant::Type> variant = SqlDataTypes::mapToVariant();
    QMap<QVariant::Type,QString> specific = SqlDataTypes::mapToDriver(db.driverName());

    QString driverName = db.driverName();

    QStringList typed;
    QStringList keys;


    int primaryKeyCount = 0;
    for(int c=0;c<fields.size();c++) {
        const Field& field = fields[c];
        if (field.name().isEmpty()) {
            continue;
        }
        if (field.primaryKey()) {
            primaryKeyCount += 1;
        }
    }

    QStringList foreignKeys;

    for(int c=0;c<fields.size();c++) {

        const Field& field = fields[c];

        if (field.name().isEmpty()) {
            continue;
        }

        QString identifier = driver->escapeIdentifier(field.name(),QSqlDriver::FieldName);

        QString type = specific[variant[field.type()]];

        if (driverName == DRIVER_PSQL && field.autoincrement() && variant[field.type()] == QVariant::Int) {
            type = "SERIAL";
        }

        static QMap<QString,QString> varchar = {
            {DRIVER_MYSQL, "VARCHAR"},
            {DRIVER_SQLITE, "VARCHAR"},
            {DRIVER_ODBC, "VARCHAR"},
        };

        if (variant[field.type()] == QVariant::String) {
            if (field.size() > -1) {
                type = QString("%1(%2)")
                        .arg(varchar.value(driverName,"VARCHAR"))
                        .arg(field.size());
            }
        }

        if (driverName == DRIVER_SQLITE && field.autoincrement()) {
            type = "INTEGER";
        }

        static QMap<QString,QString> driverAutoincrement =
        {
            {DRIVER_MYSQL, "AUTO_INCREMENT"},
            {DRIVER_SQLITE, "AUTOINCREMENT"},
        };

        QString columnSpec = joinNonEmpty({identifier,
                                           type,
                                           primaryKeyCount == 1 && field.primaryKey() ? "PRIMARY KEY" : QString(),
                                           field.autoincrement() ? driverAutoincrement.value(driverName) : QString()
                                          }, " ");

        typed << columnSpec;

        if (field.primaryKey()) {
            keys << identifier;
        }

        if (!field.foreignKey().isEmpty()) {
            foreignKeys << QString("FOREIGN KEY(%1) REFERENCES %2").arg(field.name()).arg(fixForeignKey(field.foreignKey()));
        }

    }

    if (keys.size() > 1) {
        typed << QString("PRIMARY KEY (%1)").arg(keys.join(", "));
    }
    typed.append(foreignKeys);

    QString statement = QString("CREATE TABLE %1(%2)")
            .arg(driver->escapeIdentifier(table, QSqlDriver::TableName))
            .arg(typed.join(", "));

    return statement;
}

void DataStreamer::stream(const QSqlDatabase& db, QTextStream &stream, QSqlQueryModel *model,  DataFormat::Format format,
                          const QString &table, QList<bool> data, QList<bool> keys,
                          DataFormat::ActionType action,
                          bool preview, bool* hasMore, const QLocale& locale, QString& error)
{
    Formats formats(action);

    int rowCount = model->rowCount();
    if (preview) {
        *hasMore = rowCount > 5;
        rowCount = 5;
    }

    if (format == DataFormat::Csv || format == DataFormat::Tsv) {

        QString sep = format == DataFormat::Csv ? ";" : "\t";

        stream << filterHeader(model,data).join(sep) << "\n";

        for(int r=0; r<rowCount; r++) {
            stream << variantListToStringList(filterData(model,r,data),format,formats,locale,error).join(sep) << "\n";
            if (!error.isEmpty()) {
                return;
            }
        }

    } else if (format == DataFormat::Json) {

        QJsonArray rows;
        QStringList names = filterHeader(model,data);
        for(int r=0; r<rowCount; r++) {
            QJsonObject row;
            QVariantList values = filterData(model,r,data);
            for(int i=0;i<values.size();i++) {
                row[names[i]] = variantToJson(values[i]);
            }
            rows.append(row);
        }

        QJsonObject root;
        root["table"] = table;
        root["data"] = rows;
        stream << jsonToString(root);

    } else if (format == DataFormat::SqlInsert) {

        QSqlDriver* driver = db.driver();

        QSqlRecord insertRecord;

        for(int c=0;c<model->columnCount();c++) {
            QSqlRecord record = model->record(0);
            if (data[c]) {
                insertRecord.append(record.field(c));
            }
        }

        for(int r=0;r<rowCount;r++) {
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

        for(int r=0;r<rowCount;r++) {
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
