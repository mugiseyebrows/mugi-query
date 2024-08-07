#ifndef SQLPARSE_H
#define SQLPARSE_H

#include "sqlparse_global.h"

#include <QString>
#include <QList>

class SQLPARSE_EXPORT SqlParse
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

    QString test() const;

    static QList<int> colorQueries(const QString &queries);
};

#endif // SQLPARSE_H
