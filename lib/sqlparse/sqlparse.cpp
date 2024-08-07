#include "sqlparse.h"

SqlParse::SqlParse() {}

QString SqlParse::test() const
{
    return "huest";
}

static int countSlashes(const QString &queries, int p) {
    int count = 0;
    while (p > -1) {
        if (queries[p] == '\\') {
            count += 1;
            p -= 1;
        } else {
            break;
        }
    }
    return count;
}

// todo nested multiline comments
QList<int> SqlParse::colorQueries(const QString &queries) {
    int state = Query;
    QList<int> res;
    int nextState = Undefined;
    for(int i=0;i<queries.size();i++) {
        QChar c = queries[i];
        if (nextState != Undefined) {
            state = nextState;
            nextState = Undefined;
        }
        if (state == Query) {
            switch(c.unicode()) {
            case '/': if (queries.mid(i,2) == "/*") state = MultilineComment; break;
            case '-': if (queries.mid(i,2) == "--") state = InlineComment; break;
            case '\'': state = String; break;
            case ';': state = Separator; break;
            }
        } else if (state == Separator) {
            state = Query;
        } else if (state == String) {
            if (c == '\'') {
                if ((countSlashes(queries, i-1) % 2) == 0) {
                    nextState = Query;
                }
            }
        } else if (state == InlineComment) {
            if (c == '\n') {
                nextState = Query;
            }
        } else if (state == MultilineComment) {
            if (c == '/' && i > 0 && queries[i-1] == '*') {
                nextState = Query;
            }
        }
        res.append(state);
    }
    return res;
}
