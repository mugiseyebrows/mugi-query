#ifndef MYSQLDUMPSETTINGS_H
#define MYSQLDUMPSETTINGS_H

#include <QStringList>
class QJsonObject;


class MysqldumpSettings {
public:
    enum Path {
        Table,
        DatabaseTable,
        DatabaseDatetimeTable
    };
    enum Format {
        OneFile,
        MultipleFiles
    };

    QJsonObject toJson() const;
    static MysqldumpSettings fromJson(const QJsonObject& obj);

    QString output;
    QStringList tables;

    Format format;
    Path path;
    bool schema;
    bool data;
    bool routines;
    bool ssl;
    bool tab;
    QString oneFileName;

    bool completeInsert;
    bool insertIgnore;
    bool extendedInsert;
    bool hexBlob;
    bool quoteNames;
};

#endif // MYSQLDUMPSETTINGS_H
