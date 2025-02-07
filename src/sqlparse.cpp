#include "sqlparse.h"

SqlParse::SqlParse() {}

#include <QRegularExpression>

static QString untick(const QString& s) {
    if (s.startsWith("`") && s.endsWith("`")) {
        return s.mid(1,s.size()-2);
    }
    return s;
}

QueryEffect SqlParse::queryEffect(const QString &query)
{
    QRegularExpression rx("(create|drop|alter)\\s+table\\s+([^ ]*)", QRegularExpression::CaseInsensitiveOption);
    auto m = rx.match(query);
    if (m.hasMatch()) {
        QueryEffect::Type type = QueryEffect::None;
        auto c = m.captured(1);
        auto table = untick(m.captured(2));
        if (c == "create") {
            type = QueryEffect::Create;
        } else if (c == "drop") {
            type = QueryEffect::Drop;
        } else if (c == "alter") {
            type = QueryEffect::Alter;
        }
        return QueryEffect(type, table, QString());
    }
    rx = QRegularExpression("rename\\s+table\\s+([^ ]*)\\s+to\\s+([^ ]*)", QRegularExpression::CaseInsensitiveOption);
    m = rx.match(query);
    if (m.hasMatch()) {
        auto oldName = untick(m.captured(1));
        auto table = untick(m.captured(2));
        return QueryEffect(QueryEffect::Rename, table, oldName);
    }
    return QueryEffect();
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

    QString DELIMITER = "DELIMITER";

    QString delimiter = ";";

    int nextState = Undefined;
    for(int i=0;i<queries.size();i++) {
        QChar c = queries[i];
        if (nextState != Undefined) {
            state = nextState;
            nextState = Undefined;
        }
        if (state == Query) {
            if (delimiter.startsWith(c) && queries.mid(i, delimiter.size()) == delimiter) {
                state = Delimiter;
                if (delimiter.size() == 1) {
                    nextState = Query;
                }
            } else if (c == 'd' || c == 'D') {
                if (queries.mid(i, DELIMITER.size()).toUpper() == DELIMITER) {
                    state = DelimiterKeyword;
                }
            } else {
                switch(c.unicode()) {
                case '/': if (queries.mid(i,2) == "/*") state = MultilineComment; break;
                case '-': if (queries.mid(i,2) == "--") state = InlineComment; break;
                case '\'': state = String; break;
                }
            }
        } else if (state == Delimiter) {
            if (queries.size() > i + 1) {
                QChar cn = queries[i + 1];
                if (!delimiter.contains(cn)) {
                    nextState = Query;
                }
            }
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
        } else if (state == DelimiterKeyword) {
            if (c.isSpace()) {
                delimiter = "";
                nextState = DelimiterValue;
            }
        } else if (state == DelimiterValue) {
            if (c.isSpace()) {
                nextState = Query;
            } else {
                delimiter.append(c);
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

static bool containsQuery(const QString &queries, const QList<int> colors, int p1, int p2) {
    for(int p=p1;p<p2;p++) {
        if (colors[p] == SqlParse::Query && !queries[p].isSpace()) {
            return true;
        }
    }
    return false;
}

static int skipDelimiter(const QString &queries, const QList<int>& colors, int p1) {

    //qDebug() << QChar(' ').isSpace() << QChar('\n').isSpace() << QChar('\t').isSpace() << QChar('\r').isSpace();
    int p = p1;
    if (colors.size() <= p) {
        return p;
    }
    while (colors[p] == SqlParse::Delimiter) {
        p++;
        if (colors.size() <= p) {
            return p;
        }
    }
    int p1_ = p; // delimiter trimmed
    while (queries[p].isSpace()) {
        p++;
        if (queries.size() <= p) {
            return p;
        }
    }
    if (colors[p] != SqlParse::DelimiterKeyword) {
        return p1_;
    }
    while (colors[p] == SqlParse::DelimiterKeyword || colors[p] == SqlParse::DelimiterValue) {
        p++;
        if (colors.size() <= p) {
            return p;
        }
    }
    return p;
}

QStringList SqlParse::splitQueries(const QString &queries)
{
    QList<int> colors = colorQueries(queries);
    QList<int> ixs = indexOf(colors, SqlParse::Delimiter);
    ixs.prepend(-1);
    ixs.append(colors.size());
    QStringList res;
    for(int i=0;i<ixs.size()-1;i++) {
        int p1 = ixs[i]+1;
        int p2 = ixs[i+1];
        p1 = skipDelimiter(queries, colors, p1);
        int l = p2 - p1;
        if (l > 0 && containsQuery(queries, colors, p1, p2)) {
            QString query = queries.mid(p1, l);
            res.append(query);
        }
    }
    return res;
}

bool SqlParse::isSimpleSelect(const QString &query, QString &tableName)
{
    QRegularExpression rx("^select\\s*[*]\\s*from\\s*([^ ]*)\\s*$");
    auto match = rx.match(query);
    if (match.hasMatch()) {
        tableName = match.captured(1);
        return true;
    }
    return false;
}

QueryEffect::QueryEffect() : type(None) {

}

QueryEffect::QueryEffect(Type type, const QString &table, const QString oldName)
    : type(type), table(table), oldName(oldName) {
}

bool QueryEffect::isNone() const {
    return type == None;
}
