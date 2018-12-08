#include "schemafetcher.h"

#include <QSqlQuery>
#include <QTextCodec>
#include <QSet>
#include <QVariant>
#include <QSqlRecord>
#include <QProcess>
#include <QDir>
#include <QApplication>
#include <QDebug>
#include <QSqlRecord>

#include "queryparser.h"

SchemaFetcher::SchemaFetcher()
{
}


/*static*/


QStringList SchemaFetcher::specific(const QString &driver) {

}

QStringList SchemaFetcher::functions(const QString &driver)
{

}

/*static*/
QStringList SchemaFetcher::fieldNames(const QSqlDatabase& db, const QStringList& tables) {
    QStringList result;
    QSqlQuery q(db);
    foreach(QString table,tables) {
        QSqlRecord record = db.record(table);
        int n = record.count();
        for (int i=0;i<n;i++) {
            QString field = record.fieldName(i);
            result << QString(table + "." + field).toLower();
            result << QString(field).toLower();
        }
    }
    return result;
}

QStringList SchemaFetcher::common()
{

}


/*static*/
QStringList SchemaFetcher::fetch(const QSqlDatabase& db) {

    QStringList words = common();
    QStringList tables = db.tables();
    QString table;
    foreach(table,tables)
        words << table.toLower();

    QStringList fields = fieldNames(db,tables);
    words << fields;
    words << functions(db.driverName());
    words << specific(db.driverName());
    words = words.toSet().toList();
    qSort(words);
    return words;
}

/*static*/
QString SchemaFetcher::odbcUriFileName(const QString& databaseName) {
    QString file;
    QRegExp dbq("DBQ[\\s]*=[\\s]*[{]?([^;{}]*)[}]?",Qt::CaseInsensitive);
    int pos = dbq.indexIn(databaseName);
    if (pos > -1) {
        file = dbq.cap(1);
    }
    QRegExp dataSource("Data Source[\\s]*=[\\s]*[{]?([^;{}]*)[}]?",Qt::CaseInsensitive);
    pos = dataSource.indexIn(databaseName);
    if (pos > -1) {
        file = dataSource.cap(1);
    }
    return file.trimmed();
}

/*static*/
void SchemaFetcher::test() {

    QStringList samples;
    samples << "foo;dbq=D:\\foo bar\\test.mdb;bar"
            << "foo;DBQ= D:\\foo bar\\test.mdb ;bar"
            << "foo;DBQ = {D:\\foo bar\\test.mdb} ;bar"
            << "foo;DATA SOURCE=D:\\foo bar\\test.mdb;bar"
            << "foo;Data Source= D:\\foo bar\\test.mdb ;bar"
            << "foo;Data Source = { D:\\foo bar\\test.mdb } ;bar";
    QString file_ = "D:\\foo bar\\test.mdb";
    QString file;
    QString sample;
    foreach(sample,samples) {
        file = odbcUriFileName(sample);
        Q_ASSERT(file_ == file);
    }
}

