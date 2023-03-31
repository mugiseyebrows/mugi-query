#ifndef SQLESCAPER_H
#define SQLESCAPER_H

class QSqlDriver;
class QStringList;
class QString;

class SqlEscaper {
public:
    SqlEscaper(QSqlDriver* driver);
    QString table(const QString& table) const;
    QString field(const QString& field) const;
    QStringList columns(const QStringList& columns) const;
protected:
    QSqlDriver* mDriver;
};

#endif // SQLESCAPER_H
