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
            case ';': state = Separator; nextState = Query; break;
            }
        //} else if (state == Separator) {

        } else if (state == String) {
            if (c == '\'') {
                if ((countSlashes(queries, i-1) % 2) == 0) {
                    nextState = Query;
                }
            }
        } else if (state == InlineComment) {
            if (c == '\n') {
                state = Query;
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

static QList<int> indexOf(const QList<int>& cs, int c) {
    QList<int> res;
    int p = cs.indexOf(c);
    while (p > -1) {
        res.append(p);
        if (p > -1) {
            p = cs.indexOf(c, p + 1);
        }
    }
    return res;
}

QStringList SqlParse::splitQueries(const QString &queries)
{
    QList<int> colors = colorQueries(queries);
    QList<int> ixs = indexOf(colors, SqlParse::Separator);
    ixs.prepend(-1);
    ixs.append(colors.size());
    QStringList res;
    for(int i=0;i<ixs.size()-1;i++) {
        int p1 = ixs[i]+1;
        int p2 = ixs[i+1];
        int l = p2 - p1;
        res.append(queries.mid(p1, l));
    }
    return res;
}
