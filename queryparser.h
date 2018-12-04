#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include <QStringList>

class QueryParser
{
public:
    QueryParser();

    static QStringList split(const QString& queries);

    static void testSplit();
    static void testSplitCompare(const QStringList a, const QStringList b);
    static bool isAlterSchemaQuery(const QString &query);
};

#endif // QUERYPARSER_H
