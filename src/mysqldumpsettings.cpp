#include "mysqldumpsettings.h"

#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

static QStringList toStringList(const QJsonValue& val) {
    QStringList res;
    QJsonArray arr = val.toArray();
    for(int i=0;i<arr.size();i++) {
        res.append(arr[i].toString());
    }
    return res;
}

static QJsonArray fromStringList(const QStringList& lst) {
    QJsonArray res;
    for(const QString& value: lst) {
        res.append(value);
    }
    return res;
}

// generated
QJsonObject MysqldumpSettings::toJson() const {
    QJsonObject obj;
    obj["output"] = output;
    obj["tables"] = fromStringList(tables);
    obj["format"] = format;
    obj["path"] = path;
    obj["schema"] = schema;
    obj["data"] = data;
    obj["routines"] = routines;
    obj["ssl"] = ssl;
    obj["tab"] = tab;
    obj["oneFileName"] = oneFileName;
    obj["completeInsert"] = completeInsert;
    obj["insertIgnore"] = insertIgnore;
    obj["extendedInsert"] = extendedInsert;
    obj["hexBlob"] = hexBlob;
    obj["quoteNames"] = quoteNames;
    return obj;
}

// generated
MysqldumpSettings MysqldumpSettings::fromJson(const QJsonObject& obj) {
    MysqldumpSettings res;
    res.output = obj["output"].toString();
    res.tables = toStringList(obj["tables"]);
    res.format = (Format) obj["format"].toInt(0);
    res.path = (Path) obj["path"].toInt(0);
    res.schema = obj["schema"].toBool(true);
    res.data = obj["data"].toBool(true);
    res.routines = obj["routines"].toBool(true);
    res.ssl = obj["ssl"].toBool(true);
    res.tab = obj["tab"].toBool(true);
    res.oneFileName = obj["oneFileName"].toString();
    res.completeInsert = obj["completeInsert"].toBool(false);
    res.insertIgnore = obj["insertIgnore"].toBool(false);
    res.extendedInsert = obj["extendedInsert"].toBool(true);
    res.hexBlob = obj["hexBlob"].toBool(false);
    res.quoteNames = obj["quoteNames"].toBool(true);
    return res;
}

