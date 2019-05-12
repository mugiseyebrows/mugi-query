#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include <QStringList>
#include "jointoken.h"

class QueryParser
{
public:

    static QStringList split(const QString& queries);
    static bool isAlterSchemaQuery(const QString &query);
    static QMap<QString, QString> aliases(const QString &query);
    static QList<QPair<JoinToken::JoinToken, QString> > joinSplit(const QString &joinExpr);
    static QMap<QString, QString> filterAliases(const QMap<QString, QString> &aliases, const QStringList &tables);
    static QStringList flatQueries(const QString &query);
    static QPair<QString,QStringList> parseCreateTable(const QString& query);
    static int closingBracket(const QString &s, int openBracketPos);
    static QStringList parseCreateTableCreateDefinition(const QString &definition, QString& error);
};

#endif // QUERYPARSER_H
