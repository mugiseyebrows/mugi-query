#include <QTest>

#include <sqlparse.h>

class tst_SqlParse : public QObject {
    Q_OBJECT
private slots:
    void colorQueries1();
    void colorQueries2();
    void colorQueries3();
    void colorQueries4();
    void colorQueries5();

    void splitQueries();
    void splitQueries_data();
};

void tst_SqlParse::colorQueries1() {
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

static int mid(int p1, int p2) {
    return (p1 + p2) / 2;
}

void tst_SqlParse::colorQueries2() {

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

void tst_SqlParse::colorQueries3() {
    QString q = "select --co;mm\nfrom;where";
    QList<int> cs = SqlParse::colorQueries(q);

    int s1 = q.indexOf(';');
    int s2 = q.indexOf(';',s1 + 1);
    int c = q.indexOf("--");
    int n = q.indexOf("\n");

    QCOMPARE(cs[s1], SqlParse::InlineComment);
    QCOMPARE(cs[s2], SqlParse::Separator);
    QCOMPARE(cs[c], SqlParse::InlineComment);
    QCOMPARE(cs[mid(c, n)], SqlParse::InlineComment);
    QCOMPARE(cs[n-1], SqlParse::InlineComment);
    QCOMPARE(cs[n], SqlParse::Query);

}

QList<int> indexOf(const QString& text, QChar c) {
    QList<int> res;
    int p = 0;
    p = text.indexOf(c);
    while (p > -1) {
        res.append(p);
        if (p > -1) {
            p = text.indexOf(c, p + 1);
        }
    }
    return res;
}

void tst_SqlParse::colorQueries4() {
    QString q = "'test' foo 'te\\'st' bar 'te\\\\' baz ";
    QList<int> cs = SqlParse::colorQueries(q);
    int p1 = q.indexOf("foo");
    int p2 = q.indexOf("bar");
    int p3 = q.indexOf("baz");
    QList<int> qs = indexOf(q, '\'');
    QCOMPARE(cs[p1], SqlParse::Query);
    QCOMPARE(cs[p2], SqlParse::Query);
    QCOMPARE(cs[p3], SqlParse::Query);
    for(int i=0;i<qs.size();i++) {
        QCOMPARE(cs[qs[i]], SqlParse::String);
    }
    QCOMPARE(cs[qs[3]+1], SqlParse::String);
    QCOMPARE(cs[qs[4]+1], SqlParse::Query);
    QCOMPARE(cs[qs[6]+1], SqlParse::Query);

}

void tst_SqlParse::colorQueries5() {
    QString q = ";;";
    QList<int> cs = SqlParse::colorQueries(q);
    QCOMPARE(cs[0], SqlParse::Separator);
    QCOMPARE(cs[1], SqlParse::Separator);
}

// src\tests.cpp
// .*\{(.*)\}.*split\((.*)\)\);
// QTest::newRow("") << $2 << QStringList{$1};
void tst_SqlParse::splitQueries_data()
{
    QTest::addColumn<QString>("queries");
    QTest::addColumn<QStringList>("expected");

    QTest::newRow("1") << "foo;bar;baz" << QStringList{"foo","bar","baz"};
    QTest::newRow("2") << ";;" << QStringList{"","",""};
    QTest::newRow("3") << "foo;bar--;baz" << QStringList{"foo","bar--;baz"};
    QTest::newRow("4") << "foo;bar;--baz" << QStringList{"foo","bar","--baz"};
    QTest::newRow("5") << "foo--\n;bar" << QStringList{"foo--\n","bar"};
    QTest::newRow("6") << "foo--;\nbar" << QStringList{"foo--;\nbar"};
    QTest::newRow("7") << "foo'bar';baz" << QStringList{"foo'bar'","baz"};
    QTest::newRow("8") << "foo'bar;baz" << QStringList{"foo'bar;baz"};
    QTest::newRow("9") << "foo'bar--';baz" << QStringList{"foo'bar--'","baz"};
    QTest::newRow("10") << "foo'bar'--;baz" << QStringList{"foo'bar'--;baz"};
    QTest::newRow("11") << "foo/*bar;*/baz" << QStringList{"foo/*bar;*/baz"};
    QTest::newRow("12") << "foo/*bar*/;baz" << QStringList{"foo/*bar*/","baz"};
    QTest::newRow("13") << "foo--/*bar\n;*/baz" << QStringList{"foo--/*bar\n","*/baz"};
    QTest::newRow("14") << "foo'/*bar';*/baz" << QStringList{"foo'/*bar'","*/baz"};
}

void tst_SqlParse::splitQueries()
{
    QFETCH(QString, queries);
    QFETCH(QStringList, expected);
    QStringList actual = SqlParse::splitQueries(queries);
    QCOMPARE(actual, expected);
}



QTEST_MAIN(tst_SqlParse)
#include "tst_sqlparse.moc"
