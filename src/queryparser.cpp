#include "queryparser.h"

#include <QDebug>
#include <QRegularExpression>
#include <QQueue>
#include "zipunzip.h"

namespace  {

QString replace(const QString& s, const QString& subj, const QString& repl) {
    QString s_ = s;
    s_.replace(subj,repl);
    return s_;
}

QStringList trimmed(const QStringList& vs) {
    QStringList res;
    foreach(const QString& s, vs) {
        res << s.trimmed();
    }
    return res;
}

}

bool QueryParser::isAlterSchemaQuery(const QString& query) {
    QRegularExpression rx("(create|drop|alter)\\s+table", QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption);
    return rx.match(query).hasMatch();
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


typedef QPair<int,int> IntPair;
typedef QList<IntPair> IntPairList;

QList<int> filterLevel(const IntPairList& items, int level) {
    IntPairList result;
    std::copy_if(items.begin(),items.end(),std::back_inserter(result),
                 [level](const IntPair& p){return p.second == level;});
    return unzipFirsts(result);
}

QStringList innerQueries(const QString& query, QList<int>& open0, QList<int>& close0) {
    QStringList result;
    if (open0.size() != close0.size()) {
        return result;
    }
    for(int i=0;i<open0.size();i++) {
        int p = open0[i] + 1;
        int t = close0[i];
        result << query.mid(p,t - p);
    }
    return result;
}

QString outerQuery(const QString& query, QList<int>& open0, QList<int>& close0) {
    QStringList parts;

    if (open0.size() != close0.size()) {
        return QString {};
    }

    if (open0.isEmpty()) {
        parts << query;
    } else {
        for(int i=0;i<open0.size();i++) {
            int p = 0;
            if (i > 0) {
                p = close0[i-1];
            }
            int t = open0[i] + 1;
            if (t - p > 0) {
                parts << query.mid(p,t - p);
            }
        }
        int p = close0.last();
        parts << query.mid(p);
    }
    return parts.join("");
}

QPair<IntPairList,IntPairList> parenthesis(const QString& query) {
    IntPairList open;
    IntPairList close;
    int level = 0;
    for(int i=0;i<query.size();i++) {
        QChar c = query[i];
        if (c == '(') {
            open << IntPair(i,level++);
        } else if (c == ')') {
            close << IntPair(i,--level);
        }
    }
    return QPair<IntPairList,IntPairList>(open,close);
}



QStringList QueryParser::flatQueries(const QString& query_) {

    QStringList result;

    QQueue<QString> queue;
    queue.enqueue(query_);

    while(!queue.isEmpty()) {
        QString query = queue.dequeue();
        QPair<IntPairList,IntPairList> parenthesis_ = parenthesis(query);
        QList<int> open0 = filterLevel(parenthesis_.first,0);
        QList<int> close0 = filterLevel(parenthesis_.second,0);
        result << outerQuery(query,open0,close0);
        QStringList inner = innerQueries(query,open0,close0);
        foreach(const QString& q, inner) {
            queue.enqueue(q);
        }
    }

    return result;
}



int QueryParser::closingBracket(const QString& s, int openBracketPos) {

    QMap<QChar,QChar> closing;
    closing['('] = ')';
    closing['{'] = '}';
    closing['['] = ']';

    int level = 0;

    int bracketLevel = -1;
    QChar openBracket = s[openBracketPos];
    if (!closing.contains(openBracket)) {
        return -1;
    }
    QChar closingBracket = closing[openBracket];

    for(int i=0;i<s.size();i++) {
        if (s[i] == openBracket) {
            if (i == openBracketPos) {
                bracketLevel = level;
            }
            level++;
        } else if (s[i] == closingBracket) {
            level--;
            if (i > openBracketPos && level == bracketLevel) {
                return i;
            }
        }
    }

    return -1;
}

QPair<QString,QStringList> splitDefinitions(const QString &query, const QString& name, int openBracketPos) {
    int closingBracketPos = QueryParser::closingBracket(query,openBracketPos);
    QStringList createDefinitions = trimmed(query.mid(openBracketPos+1,closingBracketPos-openBracketPos-1).split(","));
    return QPair<QString,QStringList>(name.trimmed(),createDefinitions);
}

QPair<QString,QStringList> QueryParser::parseCreateTable(const QString &query)
{
    // https://dev.mysql.com/doc/refman/8.0/en/create-table.html

    QRegularExpression rx1(replace("create (temporary)? table (if not exists)? (?<name>[^(]*) [(]"," ","\\s*"), QRegularExpression::CaseInsensitiveOption);
    QRegularExpression rx2(replace("create (temporary)? table (if not exists)? (?<name>[^(]*) like (?<oldname>[^(]*) [(]"," ","\\s*"), QRegularExpression::CaseInsensitiveOption);


    QRegularExpressionMatch rx1m = rx1.match(query);
    QRegularExpressionMatch rx2m = rx2.match(query);

    if (rx2m.hasMatch()) {
        int openBracketPos = query.indexOf("(",rx2m.capturedEnd("name"));
        return splitDefinitions(query, rx2m.captured("name"), openBracketPos);
    }

    if (rx1m.hasMatch()) {
        int openBracketPos = query.indexOf("(",rx1m.capturedEnd("name"));
        return splitDefinitions(query, rx1m.captured("name"), openBracketPos);
    }

    return QPair<QString,QStringList>();
}

QStringList QueryParser::parseCreateTableCreateDefinition(const QString& definition, QString& error) {

    QString definition_ = definition.toLower();
    if (definition_.startsWith("key ") || definition_.startsWith("primary key ")) {
        return QStringList{};
    }

/*
https://dev.mysql.com/doc/refman/8.0/en/numeric-type-overview.html

let types = [...document.querySelector('div.section ul').children].map( c => c.querySelector('code.literal') ).map( e => e.innerText )
types = types.map( e => e.split(' ')[0].split('[')[0].split('(')[0]
types = Array.from(new Set(types))
types = types.map(e => e.toLowerCase())
document.body.innerText = types.map( e => '"' + e + '"').join(' << ')
*/

    QStringList numeric;
    numeric << "bit" << "tinyint" << "bool" << "smallint" << "mediumint" << "int" << "integer"
            << "bigint" << "decimal" << "dec" << "float" << "double" << "serial";

    QStringList date;
    date << "date" << "datetime" << "timestamp" << "time" << "year";

    QStringList string;
    string << "char" << "varchar" << "enum" << "set" << "blob" << "text" << "binary" << "varbinary"
           << "mediumblob" << "mediumtext" << "longblob" << "longtext";

    QStringList spatial;
    spatial << "geometry" << "point" << "linestring" << "polygon" << "multipoint" << "multilinestring"
            << "multipolygon" << "geometrycollection";

    QStringList types;
    types << numeric << date << string << spatial;

    QRegularExpression rx1(QString("[`]?(?<name>[^` ]*)[`]?\\s+"
                                   "(?<type>(%1)([(][0-9,]*[)])?\\s*(unsigned|zerofill|\\s)*)\\s*"
                                   "(?<null>(null|not null))?\\s*"
                                   "(?<default>default\\s+[^\\s]+)?\\s*"
                                   "(?<attributes>.*)").arg(types.join("|")), QRegularExpression::CaseInsensitiveOption);
    //QRegularExpression rx2(QString("[`]?(?<name>[^` ]*)[`]?\\s+(?<type>%1)(?<attributes>.*)").arg(types.join("|")), QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch rxm = rx1.match(definition);
    //QRegularExpressionMatch rxm2 = rx2.match(definition);
    //QRegularExpressionMatch rxm = rxm1.hasMatch() ? rxm1 : rxm2;

    if (rxm.hasMatch()) {
        QStringList result;
        result << rxm.captured("name")
               << rxm.captured("type").trimmed()
               << rxm.captured("null").trimmed()
               << rxm.captured("default").trimmed()
               << rxm.captured("attributes").trimmed();
        return result;
    } else {
        error = QString("parse error");
    }

    return QStringList{};
}

namespace {

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

QStringList filter(const QList<QPair<JoinToken::JoinToken,QString> >& tokens, JoinToken::JoinToken type) {
    QStringList result;
    for(int i=0;i<tokens.size();i++) {
        const QPair<JoinToken::JoinToken,QString>& token = tokens[i];
        if (token.first == type) {
            result << token.second;
        }
    }
    return result;
}

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

namespace {


QStringList mapTrimmed(const QStringList& vs) {
    QStringList res;
    foreach(const QString& v, vs) {
        res << v.trimmed();
    }
    return res;
}

}

QMap<QString,QString> QueryParser::filterAliases(const QMap<QString,QString>& aliases, const QStringList& tables) {
    QMap<QString,QString> result;
    QStringList keys = aliases.keys();
    foreach(const QString& key, keys) {
        QString table = aliases[key];
        if (tables.contains(table)) {
            result[key] = table;
        }
    }
    return result;
}

void collectAliases(const QString& flatQuery, QMap<QString,QString>& result) {

    QRegularExpression::PatternOptions opt = QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption;

    QRegularExpression rx1("select.*from(.*)", opt);
    QRegularExpression rx2("select.*from(.*)(where|having|group\\s+by|limit)", opt);
    QRegularExpressionMatch m1 = rx1.match(flatQuery);
    QRegularExpressionMatch m2 = rx2.match(flatQuery);
    QRegularExpressionMatch m = m2.hasMatch() ? m2 : m1;

    if (!m.hasMatch()) {
        return;
    }

    QString joinExp = m.captured(1);

    QList<QPair<JoinToken::JoinToken,QString> > tokens = QueryParser::joinSplit(joinExp);

    QStringList filtered = mapTrimmed(filter(tokens, JoinToken::JoinTokenTable));

    foreach(const QString& item, filtered) {
        QStringList words = item.split(QRegularExpression("\\s+"));
        if (words.size() == 2) {
            QString table = words[0];
            QString alias = words[1];
            if (table != "()") {
                result[alias] = table;
            }
        }
    }

}

QMap<QString,QString> QueryParser::aliases(const QString& query) {
    QMap<QString,QString> result;
    QStringList queries = flatQueries(query);
    foreach(const QString& flatQuery, queries) {
        collectAliases(flatQuery, result);
    }
    return result;
}
