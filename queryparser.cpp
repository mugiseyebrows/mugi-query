#include "queryparser.h"

#include "sl.h"
#include <QDebug>
#include <QRegularExpression>
#include "zipunzip.h"

QueryParser::QueryParser()
{

}


bool QueryParser::isAlterSchemaQuery(const QString& query) {
    return query.indexOf(QRegExp("(create|drop|alter)\\s+table",Qt::CaseInsensitive)) > -1;
}

QStringList QueryParser::split(const QString &queries)
{
    QList<int> bounds;
    bounds << -1;

    QChar quote = '\'';
    QChar semicolon = ';';
    QChar minus = '-';
    QChar newline = '\n';
    QChar slash = '/';
    QChar star = '*';
    bool comment = false;
    bool multilineComment = false;
    bool literal = false;
    for(int i=0;i<queries.size();i++) {
        QChar c = queries[i];
        bool hasNext = i + 1 < queries.size();
        bool hasPrev = i > 0;
        if (c == quote) {
            if (!comment && !multilineComment) {
                literal = !literal;
            }
        } else if (c == semicolon) {
            if (!literal && !comment && !multilineComment) {
                bounds << i;
            }
        } else if (c == minus) {
            if (hasNext && queries[i+1] == minus && !literal && !multilineComment) {
                comment = true;
            }
        } else if (c == newline) {
            comment = false;
        } else if (c == slash) {
            if (!comment && !literal) {
                if (!multilineComment) {
                    if (hasNext && queries[i+1] == star) {
                        multilineComment = true;
                    }
                } else {
                    if (hasPrev && queries[i-1] == star) {
                        multilineComment = false;
                    }
                }
            }
        }
    }
    bounds << queries.size();

    if (literal) {
        return QStringList();
    }

    QStringList res;
    for(int i=0;i<bounds.size()-1;i++) {
        int pos = bounds[i] + 1;
        int size = bounds[i+1] - pos;
        QString query = size > 0 ? queries.mid(pos,size) : QString();
        res << query;
    }
    return res;
}

QString QueryParser::outer(const QString& query, const IntPairList& openParenthesisPos, const IntPairList& closeParenthesisPos) {
    return query;
}

void appendIfHasMatch(QList<QRegularExpressionMatch>& ms, const QRegularExpressionMatch& m) {
    if (m.hasMatch()) {
        ms.append(m);
    }
}

QList<QRegularExpressionMatch> matched(const QRegularExpressionMatch& m1, const QRegularExpressionMatch& m2, const QRegularExpressionMatch& m3) {
    QList<QRegularExpressionMatch> res;
    appendIfHasMatch(res,m1);
    appendIfHasMatch(res,m2);
    appendIfHasMatch(res,m3);
}

#if 0
QList<QRegularExpressionMatch> filterHasMatch(const QList<QRegularExpressionMatch>& ms) {
    QList<QRegularExpressionMatch> res;
    foreach (const QRegularExpressionMatch&m, ms) {
        if (m.hasMatch()) {
            res << m;
        }
    }
    return res;
}
#endif

bool goesFirst(const QRegularExpressionMatch& m1, const QList<QRegularExpressionMatch>& ms) {

    if (!m1.hasMatch()) {
        return false;
    }
    foreach (const QRegularExpressionMatch&m, ms) {
        if (m.hasMatch() && m.capturedStart(0) < m1.capturedStart(0)) {
            return false;
        }
    }
    return true;
}

bool goesFirst(const QRegularExpressionMatch& m1, const QRegularExpressionMatch& m2, const QRegularExpressionMatch& m3) {
    QList<QRegularExpressionMatch> ms;
    ms << m2 << m3;
    return goesFirst(m1,ms);
}

bool goesFirst(const QRegularExpressionMatch& m1, const QRegularExpressionMatch& m2) {
    QList<QRegularExpressionMatch> ms;
    ms << m2;
    return goesFirst(m1,ms);
}



QList<QPair<JoinToken::JoinToken,QString> > QueryParser::joinSplit(const QString& joinExpr) {

    using namespace JoinToken;

    QRegularExpression::PatternOptions opt = QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption;
    QRegularExpression comma(",", opt);
    QRegularExpression join("(left|right|inner|cross|)\\s+join",opt);
    QRegularExpression on("\\s+on\\s+",opt);

    QString tail = joinExpr;

    QRegularExpressionMatch commaMatch = comma.match(tail);
    QRegularExpressionMatch joinMatch = join.match(tail);
    QRegularExpressionMatch onMatch = on.match(tail);

    QStringList tokens;
    QList<JoinToken::JoinToken> types;

    JoinToken::JoinToken nextType = JoinTokenTable;

    while(commaMatch.hasMatch() || joinMatch.hasMatch() || onMatch.hasMatch()) {

        if (goesFirst(onMatch, commaMatch, joinMatch)) {

            QString token = tail.mid(0,onMatch.capturedStart());
            tokens << token;
            types << nextType;

            tokens << onMatch.captured(0);
            types << JoinTokenOn;

            tail = tail.mid(onMatch.capturedStart(0) + onMatch.capturedLength());
            nextType = JoinTokenCondition;
        } else {

            QRegularExpressionMatch m = goesFirst(commaMatch, joinMatch) ? commaMatch : joinMatch;

            QString token = tail.mid(0,m.capturedStart());
            tokens << token;
            types << nextType;

            tokens << m.captured(0);
            types.append(goesFirst(commaMatch, joinMatch) ? JoinTokenComma : JoinTokenJoin);

            tail = tail.mid(m.capturedStart(0) + m.capturedLength());
            nextType = JoinTokenTable;
        }

        commaMatch = comma.match(tail);
        joinMatch = join.match(tail);
        onMatch = on.match(tail);
    }

    tokens << tail;
    types << nextType;

    return zipToPairList(types, tokens);
}

QMap<QString,QString> QueryParser::aliases(const QString& query) {

    IntPairList openParenthesisPos;
    IntPairList closeParenthesisPos;
    QMap<QString,QString> result;

    QChar openParenthesis = '(';
    QChar closeParenthesis = ')';
    int level = 0;
    for(int i=0;i<query.size();i++) {
        QChar c = query[i];
        if (c == openParenthesis) {
            openParenthesisPos << IntPair(i,++level);
        } else if (c == closeParenthesis) {
            closeParenthesisPos << IntPair(i,level--);
        }
    }
    if (openParenthesisPos.size() != closeParenthesisPos.size()) {
        return result;
    }

    QString outer = QueryParser::outer(query, openParenthesisPos, closeParenthesisPos);

    QRegularExpression::PatternOptions opt = QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption;

    QRegularExpression rx("select.*from(.*)", opt);
    QRegularExpressionMatch m = rx.match(outer);
#if 0
    QRegularExpression comma(",", opt);
    QRegularExpression join("(left|right|inner|cross)\\s+join",opt);
    QRegularExpression where("(where|having)",opt);

    if (!m.hasMatch()) {
        return result;
    }

    QString tail = m.captured(1);
    QStringList sources;

    QRegularExpressionMatch m1 = comma.match(tail);
    QRegularExpressionMatch m2 = join.match(tail);
    QRegularExpressionMatch m3 = where.match(tail);

    QList<QRegularExpressionMatch> ms = matched(m1,m2,m3);
    if (ms.isEmpty()) {

    }
#endif

    return result;
}
