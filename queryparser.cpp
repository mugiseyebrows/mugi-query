#include "queryparser.h"

#include "sl.h"
#include <QDebug>

QueryParser::QueryParser()
{

}

void QueryParser::testSplitCompare(const QStringList e, const QStringList a) {

    QString e_ = "{" + e.join("|") + "}";
    QString a_ = "{" + a.join("|") + "}";

    if (e_ != a_) {
        qDebug() << "not equal" << e_.toStdString().c_str() << a_.toStdString().c_str();
    }
}

void QueryParser::testSplit() {

    qDebug() << "QueryParser::testSplit() started";

    QString q;
    QStringList e;
    QStringList a;

    q = "foo;bar;baz";
    e = sl("foo","bar","baz");
    a = split(q);
    testSplitCompare(e,a);

    q = ";;";
    e = sl("","","");
    a = split(q);
    testSplitCompare(e,a);

    q = "foo;bar--;baz";
    e = sl("foo","bar--;baz");
    a = split(q);
    testSplitCompare(e,a);

    q = "foo;bar;--baz";
    e = sl("foo","bar","--baz");
    a = split(q);
    testSplitCompare(e,a);

    q = "foo--\n;bar";
    e = sl("foo--\n","bar");
    a = split(q);
    testSplitCompare(e,a);

    q = "foo--;\nbar";
    e = sl("foo--;\nbar");
    a = split(q);
    testSplitCompare(e,a);

    q = "foo'bar';baz";
    e = sl("foo'bar'","baz");
    a = split(q);
    testSplitCompare(e,a);

    q = "foo'bar;baz";
    e = QStringList();
    a = split(q);
    testSplitCompare(e,a);

    q = "foo'bar--';baz";
    e = sl("foo'bar--'","baz");
    a = split(q);
    testSplitCompare(e,a);

    q = "foo'bar'--;baz";
    e = sl("foo'bar'--;baz");
    a = split(q);
    testSplitCompare(e,a);

    qDebug() << "QueryParser::testSplit() complete";
}

QStringList QueryParser::split(const QString &queries)
{
    QList<int> bounds;
    bounds << -1;

    QChar quote = '\'';
    QChar semicolon = ';';
    QChar minus = '-';
    QChar newline = '\n';
    bool comment = false;
    bool literal = false;
    for(int i=0;i<queries.size();i++) {
        QChar c = queries[i];
        bool hasNext = i+1<queries.size();
        if (c == quote) {
            if (!comment) {
                literal = !literal;
            }
        } else if (c == semicolon) {
            if (!literal && !comment) {
                bounds << i;
            }
        } else if (c == minus) {
            if (hasNext && queries[i+1] == '-' && !literal) {
                comment = true;
            }
        } else if (c == newline) {
            comment = false;
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

