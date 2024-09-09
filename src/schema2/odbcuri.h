#ifndef ODBCURI_H
#define ODBCURI_H

#include <QStringList>
#include <QMap>
#include <QRegularExpression>

class OdbcUri {
public:
    OdbcUri();

    OdbcUri(const QString uri);

    QString dbq() const;

    QString driver() const;

protected:
    QMap<QString, QString> mCase;
    QMap<QString, QString> mProps;
    QStringList mPositional;
};


#endif // ODBCURI_H
