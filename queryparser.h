#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include <QStringList>
#include "jointoken.h"

class QueryParser
{
public:
    typedef QPair<int,int> IntPair;
    typedef QList<IntPair> IntPairList;

    QueryParser();

    static QStringList split(const QString& queries);
    static bool isAlterSchemaQuery(const QString &query);
    static QString outer(const QString &query, const IntPairList &openParenthesisPos, const IntPairList &closeParenthesisPos);
    static QMap<QString, QString> aliases(const QString &query);
    static QList<QPair<JoinToken::JoinToken, QString> > joinSplit(const QString &joinExpr);
};

#endif // QUERYPARSER_H
