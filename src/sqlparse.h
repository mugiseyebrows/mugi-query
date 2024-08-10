#ifndef SQLPARSE_H
#define SQLPARSE_H

#include <QString>
#include <QList>

class QueryEffect {
public:
    enum Type {
        None,
        Create,
        Drop,
        Alter,
        Rename
    };
    QueryEffect();
    QueryEffect(Type type, const QString& table, const QString oldName);

    bool isNone() const;
    QString oldName;
    QString table;
    Type type;
};

class SqlParse
{
public:
    enum CharColor {
        Undefined,
        Query,
        Separator,
        String,
        InlineComment,
        MultilineComment
    };

    SqlParse();

    static QueryEffect queryEffect(const QString& query);

    static QList<int> colorQueries(const QString &queries);

    static QStringList splitQueries(const QString &queries);
};

#endif // SQLPARSE_H
