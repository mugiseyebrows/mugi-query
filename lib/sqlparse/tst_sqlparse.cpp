#include <QTest>

#include <sqlparse.h>

class tst_SqlParse : public QObject {
    Q_OBJECT
private slots:
    void colors1();
    void colors2();
};


void tst_SqlParse::colors1() {
    //SqlParse parse;

    QString q = "sel;from;where";

    QList<int> colors = SqlParse::colorQueries(q);
    int p1 = q.indexOf(";");
    int p2 = q.indexOf(";", p1 + 1);

    QCOMPARE(colors[p1], SqlParse::Separator);
    QCOMPARE(colors[p2], SqlParse::Separator);
    QCOMPARE(colors[0], SqlParse::Query);
    QCOMPARE(colors[colors.size()-1], SqlParse::Query);
    QCOMPARE(colors.size(), q.size());

}

int mid(int p1, int p2) {
    return (p1 + p2) / 2;
}

void tst_SqlParse::colors2() {

    QString q = "sel 'test' ;from/*;where*/join";

    QList<int> cs = SqlParse::colorQueries(q);

    int s1 = q.indexOf(';');
    int s2 = q.indexOf(';',s1 + 1);
    int q1 = q.indexOf('\'');
    int q2 = q.indexOf('\'', q1 + 1);
    int c1 = q.indexOf("/*");
    int c2 = q.indexOf("*/");

    QCOMPARE(cs[q1], SqlParse::String);
    QCOMPARE(cs[q2], SqlParse::String);
    QCOMPARE(cs[mid(q1,q2)], SqlParse::String);
    QCOMPARE(cs[q2+1], SqlParse::Query);

    QCOMPARE(cs[c1], SqlParse::MultilineComment);
    QCOMPARE(cs[c2], SqlParse::MultilineComment);
    QCOMPARE(cs[mid(c1,c2)], SqlParse::MultilineComment);
    QCOMPARE(cs[c2+2], SqlParse::Query);

    QCOMPARE(cs[s1], SqlParse::Separator);
    QCOMPARE(cs[s2], SqlParse::MultilineComment);
}

QTEST_MAIN(tst_SqlParse)
#include "tst_sqlparse.moc"
