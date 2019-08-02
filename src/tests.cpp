#include "tests.h"

#include <QStringList>
#include "jointokenlist.h"

#include "zipunzip.h"
#include <QDebug>
#include "queryparser.h"
#include "lit.h"
#include "stringstringmap.h"
#include "testdatetimesample.h"
#include "testdatesample.h"
#include "datetime.h"
#include "sqldatatypes.h"
#include <QTimeZone>

using namespace Lit;

void Tests::run()
{
    testSplit();
    testJoinSplit();
    testAliases();
    testFlatQueries();
    testClosingBracket();
    testParseCreateTable();
    textParseCreateTableCreateDefinition();
    testDateRegularExpressions();
    testDateTimeRegularExpressions();
    testTryConvert1();
    testTryConvert2();
    testApParse();
}

namespace {

QStringList mapTrimmed(const QStringList& vs) {
    QStringList res;
    foreach(const QString& v, vs) {
        res << v.trimmed();
    }
    return res;
}

QString mapAsString(const QMap<QString,QString>& m) {
    QStringList keys = m.keys();
    QStringList vs;
    foreach(const QString& key, keys) {
        vs << QString("%1:%2").arg(key).arg(m[key]);
    }
    return QString("{%1}").arg(vs.join("|"));
}

QString tokenListAsString(const QList<QPair<JoinToken::JoinToken,QString> >& vs) {
    QStringList res;
    for(int i=0;i<vs.size();i++) {
        const QPair<JoinToken::JoinToken,QString>& v = vs[i];
        res << QString("%1, %2").arg(v.first).arg(v.second);
    }
    return QString("{%1}").arg(res.join(", "));
}

bool equals(int a, int b) {
    if (a == b) {
        return true;
    }
    qDebug() << "not equal, expected: " << a << ", actual " << b;
    return false;
}

bool equals(const QMap<QString,QString>& e, const QMap<QString,QString>& a) {
    if (e == a) {
        return true;
    }
    QString e_ = mapAsString(e);
    QString a_ = mapAsString(a);
    qDebug() << "not equal, expected: " << e_.toStdString().c_str() << ", actual " << a_.toStdString().c_str();
    return false;
}

bool equals(const QList<QPair<JoinToken::JoinToken,QString> >& e, const QList<QPair<JoinToken::JoinToken,QString> >& a) {

    QStringList es = mapTrimmed(unzipSeconds(e));
    QStringList as = mapTrimmed(unzipSeconds(a));
    QList<JoinToken::JoinToken> ef = unzipFirsts(e);
    QList<JoinToken::JoinToken> af = unzipFirsts(a);
    if (as == es && ef == af) {
        return true;
    }
    QString e_ = tokenListAsString(e);
    QString a_ = tokenListAsString(a);
    qDebug() << "not equal, expected: " << e_.toStdString().c_str() << ", actual " << a_.toStdString().c_str();
    return false;
}

bool equals(const QPair<QString,QStringList>& e, const QPair<QString,QStringList>& a) {
    if (e == a) {
        return true;
    }
    QString e_ = e.first + "{" + e.second.join("|") + "}";
    QString a_ = a.first + "{" + a.second.join("|") + "}";
    qDebug() << "not equal, expected: " << e_.toStdString().c_str() << ", actual " << a_.toStdString().c_str();
    return false;
}

bool equals(const QString& e, const QString& a) {
    if (e == a) {
        return true;
    }
    qDebug() << "not equal, expected: " << e << ", actual " << a;
}

bool equals(const QStringList e, const QStringList a) {
    QString e_ = "{" + e.join("|") + "}";
    QString a_ = "{" + a.join("|") + "}";
    if (e_ == a_) {
        return true;
    }
    qDebug() << "not equal, expected: " << e_.toStdString().c_str() << ", actual " << a_.toStdString().c_str();
    return false;
}

bool isValid(const QTime& time) {
    if (!time.isValid()) {
        qDebug() << "time is not valid";
    }
    return time.isValid();
}

bool testOffset(int id, const QDateTime& d1, const QDateTime& d2, int offset) {

    QDateTime d1_ = d1.toUTC();
    QDateTime d2_ = d2.toUTC();

    int offset_ = d1_.secsTo(d2_);
    if (offset_ != offset) {
        qDebug() << "test" << id << "offset expected" << offset << "actual" << offset_ << d1 << d2;
    }
    return offset_ == offset;
}

bool testSpec(int id, const QDateTime& d, Qt::TimeSpec spec) {
    Qt::TimeSpec spec_ = d.timeSpec();
    if (spec_ != spec) {
        qDebug() << "test" << id << "spec expected" << spec << "actual" << spec_;
    }
    return spec_ == spec;
}

bool allTrue(const QList<bool>& vs) {
    foreach(bool v,vs) {
        if (!v) {
            return false;
        }
    }
    return true;
}

bool testDateEquals(int id, const QDate& date, Qt::DateFormat format,
                    const QString& string, const QVariant& converted, bool ok) {

    if (converted.isNull()) {
        qDebug() << "test" << id << "tryConvert return null" << date << format << string << converted << ok;
        return false;
    }

    QDate date_ = converted.toDate();

    if (!ok) {
        qDebug() << "test" << id << "tryConvert ok == false" << date << format << string << converted << ok;
        return false;
    }

    if (date != date_) {
        qDebug() << "test" << id << "tryConvert converted with error" << date << format << string << converted << ok;
        return false;
    }

    return true;
}

bool testDateEquals(int id, const QDate& date, const QString& format,
                    const QString& string, const QVariant& converted, bool ok) {

    if (converted.isNull()) {
        qDebug() << "test" << id << "tryConvert return null" << date << format << string << converted << ok;
        return false;
    }

    QDate date_ = converted.toDate();

    if (!ok) {
        qDebug() << "test" << id << "tryConvert ok == false" << date << format << string << converted << ok;
        return false;
    }

    if (date != date_) {
        qDebug() << "test" << id << "tryConvert converted with error" << date << format << string << converted << ok;
        return false;
    }

    return true;
}

bool testTimeEquals(int id, const QTime& time, Qt::DateFormat format,
                    const QString& string, const QVariant& converted, bool ok) {

    if (converted.isNull()) {
        qDebug() << "test" << id << "tryConvert return null" << time << format << string << converted << ok;
        return false;
    }

    QTime time_ = converted.toTime();

    if (!ok) {
        qDebug() << "test" << id << "tryConvert ok == false" << time << format << string << converted << ok;
        return false;
    }

    if (time != time_) {
        qDebug() << "test" << id << "tryConvert converted with error" << time << format << string << converted << ok;
        return false;
    }

    return true;
}

bool testDateTimeEquals(int id, const QDateTime& dateTime, Qt::DateFormat format,
                    const QString& string, const QVariant& converted, bool ok) {

    if (converted.isNull()) {
        qDebug() << "test" << id << "tryConvert return null" << dateTime << format << string << converted << ok;
        return false;
    }

    QDateTime dateTime_ = converted.toDateTime();

    if (!ok) {
        qDebug() << "test" << id << "tryConvert ok == false" << dateTime << format << string << converted << ok;
        return false;
    }

    if (dateTime != dateTime_) {
        qDebug() << "test" << id << "tryConvert converted with error" << dateTime << format << string << converted << ok;
        return false;
    }

    return true;
}



} // namespace

void Tests::testAliases() {

    qDebug() << "testAliases() started";

    using namespace StringStringMap;

    QString q;
    QMap<QString,QString> e,a;

    q = "select * from foo, bar left join baz";
    e = ssm();
    a = QueryParser::aliases(q);
    equals(e,a);

    q = "select * from foo f1, bar b1 left join baz b2 on baz.id=bar.id join qix q1";
    e = ssm("f1","foo","b1","bar","b2","baz","q1","qix");
    a = QueryParser::aliases(q);
    equals(e,a);

    q = "select * from (select abc from zen z1 having 2) x, bar b1 left join baz b2 on baz.id=bar.id join qix q1 where a=b";
    e = ssm("b1","bar","b2","baz","q1","qix","z1","zen");
    a = QueryParser::aliases(q);
    equals(e,a);

    qDebug() << "testAliases() finished";
}

void Tests::testJoinSplit() {

    qDebug() << "testJoinSplit started";

    using namespace JoinTokenList;
    using namespace JoinToken;

    QString q;
    QList<QPair<JoinToken::JoinToken,QString> > e, a;

    q = "foo, bar left join baz on baz.id=bar.id join qix";
    e = jtl(JoinTokenTable,"foo",
            JoinTokenComma,",",
            JoinTokenTable,"bar",
            JoinTokenJoin,"left join",
            JoinTokenTable,"baz",
            JoinTokenOn,"on",
            JoinTokenCondition,"baz.id=bar.id",
            JoinTokenJoin,"join",
            JoinTokenTable,"qix");

    a = QueryParser::joinSplit(q);
    equals(e,a);

    q = "foo f1, bar b1 left join baz b2 on baz.id=bar.id join qix q1";
    e = jtl(JoinTokenTable,"foo f1",
            JoinTokenComma,",",
            JoinTokenTable,"bar b1",
            JoinTokenJoin,"left join",
            JoinTokenTable,"baz b2",
            JoinTokenOn,"on",
            JoinTokenCondition,"baz.id=bar.id",
            JoinTokenJoin,"join",
            JoinTokenTable,"qix q1");

    a = QueryParser::joinSplit(q);
    equals(e,a);

    qDebug() << "testJoinSplit() finished";
}


void Tests::testSplit() {

    QList<bool> ok;

    ok << equals({"foo","bar","baz"},QueryParser::split("foo;bar;baz"));
    ok << equals({"","",""},QueryParser::split(";;"));
    ok << equals({"foo","bar--;baz"},QueryParser::split("foo;bar--;baz"));
    ok << equals({"foo","bar","--baz"},QueryParser::split("foo;bar;--baz"));
    ok << equals({"foo--\n","bar"},QueryParser::split("foo--\n;bar"));
    ok << equals({"foo--;\nbar"},QueryParser::split("foo--;\nbar"));
    ok << equals({"foo'bar'","baz"},QueryParser::split("foo'bar';baz"));
    ok << equals({},QueryParser::split("foo'bar;baz"));
    ok << equals({"foo'bar--'","baz"},QueryParser::split("foo'bar--';baz"));
    ok << equals({"foo'bar'--;baz"},QueryParser::split("foo'bar'--;baz"));
    ok << equals({"foo/*bar;*/baz"},QueryParser::split("foo/*bar;*/baz"));
    ok << equals({"foo/*bar*/","baz"},QueryParser::split("foo/*bar*/;baz"));
    ok << equals({"foo--/*bar\n","*/baz"},QueryParser::split("foo--/*bar\n;*/baz"));
    ok << equals({"foo'/*bar'","*/baz"},QueryParser::split("foo'/*bar';*/baz"));

    qDebug() << "testSplit" << (allTrue(ok) ? "passed" : "failed");
}

void Tests::testFlatQueries() {

    qDebug() << "testFlatQueries() started";

    QString q;
    QStringList e,a;

    q = "select foo from (select * from bar) union (select baz from qix)";
    e = sl("select foo from () union ()","select * from bar","select baz from qix");
    a = QueryParser::flatQueries(q);
    equals(e,a);

    q = "select a,b,c from (select foo from (select * from bar) union (select baz from qix)) where 1";
    e = sl("select a,b,c from () where 1","select foo from () union ()","select * from bar","select baz from qix");
    a = QueryParser::flatQueries(q);
    equals(e,a);

    q = "select * from foo";
    e = sl("select * from foo");
    a = QueryParser::flatQueries(q);
    equals(e,a);

    qDebug() << "testFlatQueries() finished";
}

void Tests::testClosingBracket() {

    qDebug() << "testClosingBracket() started";

    QString t;
    int a,e;

    t = "()";
    a = QueryParser::closingBracket(t,0);
    e = 1;
    equals(e,a);

    t = "(())";
    a = QueryParser::closingBracket(t,0);
    e = 3;
    equals(e,a);

    t = "(())";
    a = QueryParser::closingBracket(t,1);
    e = 2;
    equals(e,a);

    t = "create table foo(bar int(11),baz text)";
    a = QueryParser::closingBracket(t,t.indexOf("("));
    e = t.lastIndexOf(")");
    equals(e,a);
    qDebug() << "testClosingBracket() finished";
}

void Tests::testParseCreateTable()
{
    qDebug() << "testParseCreateTable() started";

    typedef QPair<QString,QStringList> X;

    QString t;
    X a,e;

    t = "create table foo(a,b)";
    a = QueryParser::parseCreateTable(t);
    e = X("foo",sl("a","b"));
    equals(e,a);

    t = "create temporary table if not exists foo(a,b)";
    a = QueryParser::parseCreateTable(t);
    e = X("foo",sl("a","b"));
    equals(e,a);

    t = "create table foo like bar(a,b)";
    a = QueryParser::parseCreateTable(t);
    e = X("foo",sl("a","b"));
    equals(e,a);

    qDebug() << "testParseCreateTable() finished";
}

void Tests::textParseCreateTableCreateDefinition() {

    qDebug() << "textParseCreateTableCreateDefinition() started";

    QString t,err;
    QStringList e,a;

    t = "`foo` int(11) NOT NULL";
    e = sl("foo","int(11)","NOT NULL","","");
    a = QueryParser::parseCreateTableCreateDefinition(t,err);
    equals(e,a);

    t = "`bar` text";
    e = sl("bar","text","","","");
    a = QueryParser::parseCreateTableCreateDefinition(t,err);
    equals(e,a);

    t = "foo int unsigned zerofill null";
    e = sl("foo","int unsigned zerofill","null","","");
    a = QueryParser::parseCreateTableCreateDefinition(t,err);
    equals(e,a);

    t = "`foo` varchar(100) DEFAULT NULL";
    e = sl("foo","varchar(100)","","DEFAULT NULL","");
    a = QueryParser::parseCreateTableCreateDefinition(t,err);
    equals(e,a);

    t = "`bar` int(11) NOT NULL DEFAULT '0'";
    e = sl("bar","int(11)","NOT NULL","DEFAULT '0'","");
    a = QueryParser::parseCreateTableCreateDefinition(t,err);
    equals(e,a);

    qDebug() << "textParseCreateTableCreateDefinition() finished";
}




void Tests::testDateRegularExpressions() {
    static QMap<Qt::DateFormat,int> dateFormats = {
        {Qt::DefaultLocaleLongDate,0},{Qt::SystemLocaleLongDate,0},
        {Qt::DefaultLocaleShortDate,1},{Qt::SystemLocaleShortDate,1},
        {Qt::ISODate,2},{Qt::ISODateWithMs,2},
        {Qt::RFC2822Date,3},
        {Qt::TextDate,4}
    };
    QList<TestDateSample> samples = {
        TestDateSample(QDate::fromString("1974-09-05",Qt::ISODate),"5 сентября 1974 г.",Qt::DefaultLocaleLongDate),
        TestDateSample(QDate::fromString("2049-08-14",Qt::ISODate),"14 августа 2049 г.",Qt::DefaultLocaleLongDate),
        TestDateSample(QDate::fromString("1977-01-13",Qt::ISODate),"13 января 1977 г.",Qt::DefaultLocaleLongDate),
        TestDateSample(QDate::fromString("2067-01-28",Qt::ISODate),"28.01.2067",Qt::DefaultLocaleShortDate),
        TestDateSample(QDate::fromString("2058-11-19",Qt::ISODate),"19.11.2058",Qt::DefaultLocaleShortDate),
        TestDateSample(QDate::fromString("1928-02-07",Qt::ISODate),"07.02.1928",Qt::DefaultLocaleShortDate),
        TestDateSample(QDate::fromString("2057-11-08",Qt::ISODate),"2057-11-08",Qt::ISODate),
        TestDateSample(QDate::fromString("2056-08-10",Qt::ISODate),"2056-08-10",Qt::ISODate),
        TestDateSample(QDate::fromString("2016-07-12",Qt::ISODate),"2016-07-12",Qt::ISODate),
        TestDateSample(QDate::fromString("1978-04-10",Qt::ISODate),"1978-04-10",Qt::ISODateWithMs),
        TestDateSample(QDate::fromString("1982-10-02",Qt::ISODate),"1982-10-02",Qt::ISODateWithMs),
        TestDateSample(QDate::fromString("2045-12-16",Qt::ISODate),"2045-12-16",Qt::ISODateWithMs),
        TestDateSample(QDate::fromString("1998-01-05",Qt::ISODate),"05 Jan 1998",Qt::RFC2822Date),
        TestDateSample(QDate::fromString("1921-06-18",Qt::ISODate),"18 Jun 1921",Qt::RFC2822Date),
        TestDateSample(QDate::fromString("1956-12-13",Qt::ISODate),"13 Dec 1956",Qt::RFC2822Date),
        TestDateSample(QDate::fromString("1927-02-05",Qt::ISODate),"5 февраля 1927 г.",Qt::SystemLocaleLongDate),
        TestDateSample(QDate::fromString("1924-03-21",Qt::ISODate),"21 марта 1924 г.",Qt::SystemLocaleLongDate),
        TestDateSample(QDate::fromString("1974-08-23",Qt::ISODate),"23 августа 1974 г.",Qt::SystemLocaleLongDate),
        TestDateSample(QDate::fromString("1995-09-12",Qt::ISODate),"12.09.1995",Qt::SystemLocaleShortDate),
        TestDateSample(QDate::fromString("1995-03-21",Qt::ISODate),"21.03.1995",Qt::SystemLocaleShortDate),
        TestDateSample(QDate::fromString("2087-03-21",Qt::ISODate),"21.03.2087",Qt::SystemLocaleShortDate),
        TestDateSample(QDate::fromString("1970-05-28",Qt::ISODate),"Чт май 28 1970",Qt::TextDate),
        TestDateSample(QDate::fromString("2070-12-20",Qt::ISODate),"Сб дек 20 2070",Qt::TextDate),
        TestDateSample(QDate::fromString("2006-12-23",Qt::ISODate),"Сб дек 23 2006",Qt::TextDate)
    };

    QList<QRegularExpression> exps = DateTime::dateRegularExpressions();

    bool passed = true;

    foreach (const TestDateSample& sample, samples) {
        int i=0;
        foreach (QRegularExpression exp, exps) {
            if (exp.match(sample.string()).hasMatch()) {
                if (dateFormats[sample.format()] == i) {

                } else {
                    passed = false;
                    qDebug() << "false positive" << sample.string() << exp.pattern() << i;
                }
            } else {
                if (dateFormats[sample.format()] == i) {
                    passed = false;
                    qDebug() << "not matched" << sample.string() << exp.pattern() << i;
                }
            }
            i++;
        }
    }

    qDebug() << "testDateRegularExpressions" << (passed ? "passed" : "failed");

}

void Tests::testDateTimeRegularExpressions() {

    static QMap<Qt::DateFormat,int> dateFormats = {
        {Qt::DefaultLocaleLongDate,0},{Qt::SystemLocaleLongDate,0},
        {Qt::DefaultLocaleShortDate,1},{Qt::SystemLocaleShortDate,1},
        {Qt::ISODate,2},
        {Qt::ISODateWithMs,3},
        {Qt::RFC2822Date,4},
        {Qt::TextDate,5}
    };

    QList<TestDateTimeSample> samples = {
        TestDateTimeSample(QDateTime::fromString("1974-09-05T01:18:07.196",Qt::ISODateWithMs),"5 сентября 1974 г. 1:18:07",Qt::DefaultLocaleLongDate),
        TestDateTimeSample(QDateTime::fromString("2049-08-14T11:50:04.355",Qt::ISODateWithMs),"14 августа 2049 г. 11:50:04",Qt::DefaultLocaleLongDate),
        TestDateTimeSample(QDateTime::fromString("1977-01-13T06:08:13.080",Qt::ISODateWithMs),"13 января 1977 г. 6:08:13",Qt::DefaultLocaleLongDate),
        TestDateTimeSample(QDateTime::fromString("2067-01-28T21:11:34.176",Qt::ISODateWithMs),"28.01.2067 21:11",Qt::DefaultLocaleShortDate),
        TestDateTimeSample(QDateTime::fromString("2058-11-19T09:35:45.405",Qt::ISODateWithMs),"19.11.2058 9:35",Qt::DefaultLocaleShortDate),
        TestDateTimeSample(QDateTime::fromString("1928-02-07T01:10:58.725",Qt::ISODateWithMs),"07.02.1928 1:10",Qt::DefaultLocaleShortDate),
        TestDateTimeSample(QDateTime::fromString("2057-11-08T17:21:09.216",Qt::ISODateWithMs),"2057-11-08T17:21:09",Qt::ISODate),
        TestDateTimeSample(QDateTime::fromString("2056-08-10T13:32:38.421",Qt::ISODateWithMs),"2056-08-10T13:32:38",Qt::ISODate),
        TestDateTimeSample(QDateTime::fromString("2016-07-12T08:58:30.605",Qt::ISODateWithMs),"2016-07-12T08:58:30",Qt::ISODate),
        TestDateTimeSample(QDateTime::fromString("1978-04-10T04:57:05.185",Qt::ISODateWithMs),"1978-04-10T04:57:05.185",Qt::ISODateWithMs),
        TestDateTimeSample(QDateTime::fromString("1982-10-02T23:27:48.890",Qt::ISODateWithMs),"1982-10-02T23:27:48.890",Qt::ISODateWithMs),
        TestDateTimeSample(QDateTime::fromString("2045-12-16T14:06:10.940",Qt::ISODateWithMs),"2045-12-16T14:06:10.940",Qt::ISODateWithMs),
        TestDateTimeSample(QDateTime::fromString("1998-01-05T05:44:32.946",Qt::ISODateWithMs),"05 Jan 1998 05:44:32 +0300",Qt::RFC2822Date),
        TestDateTimeSample(QDateTime::fromString("1921-06-18T00:22:37.991",Qt::ISODateWithMs),"18 Jun 1921 00:22:37 +0300",Qt::RFC2822Date),
        TestDateTimeSample(QDateTime::fromString("1956-12-13T08:59:41.470",Qt::ISODateWithMs),"13 Dec 1956 08:59:41 +0300",Qt::RFC2822Date),
        TestDateTimeSample(QDateTime::fromString("1927-02-05T21:07:48.672",Qt::ISODateWithMs),"5 февраля 1927 г. 21:07:48",Qt::SystemLocaleLongDate),
        TestDateTimeSample(QDateTime::fromString("1924-03-21T07:46:16.033",Qt::ISODateWithMs),"21 марта 1924 г. 7:46:16",Qt::SystemLocaleLongDate),
        TestDateTimeSample(QDateTime::fromString("1974-08-23T22:49:31.625",Qt::ISODateWithMs),"23 августа 1974 г. 22:49:31",Qt::SystemLocaleLongDate),
        TestDateTimeSample(QDateTime::fromString("1995-09-12T20:45:08.354",Qt::ISODateWithMs),"12.09.1995 20:45",Qt::SystemLocaleShortDate),
        TestDateTimeSample(QDateTime::fromString("1995-03-21T12:46:49.845",Qt::ISODateWithMs),"21.03.1995 12:46",Qt::SystemLocaleShortDate),
        TestDateTimeSample(QDateTime::fromString("2087-03-21T07:22:50.184",Qt::ISODateWithMs),"21.03.2087 7:22",Qt::SystemLocaleShortDate),
        TestDateTimeSample(QDateTime::fromString("1970-05-28T04:55:06.498",Qt::ISODateWithMs),"Чт май 28 04:55:06 1970",Qt::TextDate),
        TestDateTimeSample(QDateTime::fromString("2070-12-20T12:23:41.663",Qt::ISODateWithMs),"Сб дек 20 12:23:41 2070",Qt::TextDate),
        TestDateTimeSample(QDateTime::fromString("2006-12-23T19:09:00.164",Qt::ISODateWithMs),"Сб дек 23 19:09:00 2006",Qt::TextDate),
    };

    // samples.last().setFormat(Qt::SystemLocaleShortDate); // not matched and false positive
    // samples.last().setString("Сб нул 23 19:09:00 2006"); // not matched

    QList<QRegularExpression> exps = DateTime::dateTimeRegularExpressions();

    bool passed = true;

    foreach (const TestDateTimeSample& sample, samples) {
        int i=0;
        foreach (QRegularExpression exp, exps) {
            if (exp.match(sample.string()).hasMatch()) {
                if (dateFormats[sample.format()] == i) {

                } else {
                    passed = false;
                    qDebug() << "false positive" << sample.string() << exp.pattern() << i;
                }
            } else {
                if (dateFormats[sample.format()] == i) {
                    passed = false;
                    qDebug() << "not matched" << sample.string() << exp.pattern() << i;
                }
            }
            i++;
        }
    }

    qDebug() << "testDateTimeRegularExpressions" << (passed ? "passed" : "failed");
}

void Tests::testTryConvert1() {

    QLocale locale;

    QList<Qt::DateFormat> dateFormats = {Qt::TextDate,
                                         Qt::ISODate,
                                         Qt::SystemLocaleShortDate,
                                         Qt::SystemLocaleLongDate,
                                         Qt::DefaultLocaleShortDate,
                                         Qt::DefaultLocaleLongDate,
                                         Qt::RFC2822Date,
                                         Qt::ISODateWithMs};

    QStringList dateFormats2 = {"yyyy-MM-dd","dd.MM.yyyy"};

    QList<bool> passed;

    bool inLocalDateTime = true;
    bool outLocalDateTime = true;

    QRegularExpression rxTimeWithMs("[0-9]+:[0-9]+:[0-9]+[.][0-9]+");
    QRegularExpression rxTimeWithS("[0-9]+:[0-9]+:[0-9]+");

    for(int i=0;i<100;i++) {

        int y = 1900 + (rand() % 200);
        int M = 1 + (rand() % 12);
        int d = 1 + (rand() % 28);

        int h = rand() % 24;
        int m = rand() % 60;
        int s = rand() % 60;
        int ms = rand() % 1000;

        QDate date(y,M,d);
        QTime time(h,m,s,ms);

        /*QDateTime t1(QDate(2000,1,1),QTime(12,0,0),Qt::LocalTime);
        QDateTime t2(QDate(2000,1,1),QTime(12,0,0),Qt::UTC);*/

        QDateTime dateTime(date,time,Qt::LocalTime);

        // date

        foreach(Qt::DateFormat format, dateFormats) {
            QString string = date.toString(format);
            bool ok;
            QVariant converted = SqlDataTypes::tryConvert(string,QVariant::Date,locale,inLocalDateTime,outLocalDateTime,&ok);
            //if ((rand() % 40) == 0) converted = converted.toDate().addDays(1);
            passed << testDateEquals(__LINE__,date,format,string,converted,ok);
        }



        foreach(const QString& format, dateFormats2) {
            QString string = date.toString(format);
            bool ok;
            QVariant converted = SqlDataTypes::tryConvert(string,QVariant::Date,locale,inLocalDateTime,outLocalDateTime,&ok);
            //if ((rand() % 40) == 0) converted = converted.toDate().addDays(1);
            passed << testDateEquals(__LINE__,date,format,string,converted,ok);
        }

        // time

        foreach(Qt::DateFormat format, dateFormats) {
            if (format == Qt::DefaultLocaleLongDate || format == Qt::SystemLocaleLongDate) {
                //TODO: is "7:06:08 MSK" valid time?
                continue;
            }
            QString string = time.toString(format);
            bool hasSeconds = rxTimeWithS.match(string).hasMatch();
            bool hasMilliseconds = rxTimeWithMs.match(string).hasMatch();
            bool ok;
            QVariant converted = SqlDataTypes::tryConvert(string, QVariant::Time, locale,inLocalDateTime,outLocalDateTime,&ok);
            QTime time_ = QTime(h, m, hasSeconds ? s : 0, hasMilliseconds ? ms : 0);
            //if ((rand() % 40) == 0) converted = converted.toTime().addSecs(1);
            passed << testTimeEquals(__LINE__,time_,format,string,converted,ok);
        }

        // datetime

        foreach(Qt::DateFormat format, dateFormats) {
            QString string = dateTime.toString(format);
            bool hasSeconds = rxTimeWithS.match(string).hasMatch();
            bool hasMilliseconds = rxTimeWithMs.match(string).hasMatch();
            QDateTime dateTime_ = QDateTime(QDate(y, M, d),
                                            QTime(h, m, hasSeconds ? s : 0, hasMilliseconds ? ms : 0),
                                            Qt::LocalTime);
            bool ok;
            QVariant converted = SqlDataTypes::tryConvert(string,QVariant::DateTime,locale,inLocalDateTime,outLocalDateTime,&ok);
            //if ((rand() % 40) == 0) converted = converted.toDateTime().addDays(1);
            passed << testDateTimeEquals(__LINE__,dateTime_,format,string,converted,ok);

            /*if (format == Qt::TextDate) {
                qDebug() << "Qt::TextDate" << dateTime << string << converted.toDateTime();
            }*/

        }


    } // for

    qDebug() << "testTryConvert1" << (allTrue(passed) ? "passed" : "failed");
}



void Tests::testTryConvert2() {

    QLocale locale;

    QList<bool> ok;

    QDateTime dlocal(QDate(2000,1,1),QTime(12,0,0),Qt::LocalTime);

    QDateTime dutc(QDate(2000,1,1),QTime(12,0,0),Qt::UTC);

    QString s = dlocal.toString(Qt::ISODate); // no timezone
    //qDebug() << s;

    QDateTime d1, d2, d3, d4;

    d1 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, true, true).toDateTime();
    d2 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, false, false).toDateTime();
    d3 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, false, true).toDateTime();
    d4 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, true, false).toDateTime();

    // d1 TT 12MSK 12MSK
    // d2 FF 12GMT 12GMT
    // d3 FT 12GMT 15MSK
    // d4 TF 12MSK  9GMT

    ok << testSpec(__LINE__,d1,Qt::LocalTime);
    ok << testSpec(__LINE__,d2,Qt::UTC);
    ok << testSpec(__LINE__,d3,Qt::LocalTime);
    ok << testSpec(__LINE__,d4,Qt::UTC);

    ok << testOffset(__LINE__,dlocal,d1,0);
    ok << testOffset(__LINE__,dutc,d2,0);
    ok << testOffset(__LINE__,dutc,d3,0);
    ok << testOffset(__LINE__,dlocal,d4,0);

    int offset = dlocal.timeZone().offsetFromUtc(dlocal);

    ok << testOffset(__LINE__,dlocal,d2,offset);
    ok << testOffset(__LINE__,dlocal,d3,offset);

    s = dlocal.toString(Qt::RFC2822Date); // with numeric timezone
    //qDebug() << s;

    d1 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, true, true).toDateTime();
    d2 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, false, false).toDateTime();
    d3 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, false, true).toDateTime();
    d4 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, true, false).toDateTime();

    // d1 TT 12MSK 12MSK
    // d2 FF 12MSK  9GMT
    // d3 FT 12MSK 12MSK
    // d4 TF 12MSK  9GMT

    ok << testSpec(__LINE__,d1,Qt::OffsetFromUTC);
    ok << testSpec(__LINE__,d2,Qt::UTC);
    ok << testSpec(__LINE__,d3,Qt::OffsetFromUTC);
    ok << testSpec(__LINE__,d4,Qt::UTC);

    ok << testOffset(__LINE__,dlocal,d1,0);
    ok << testOffset(__LINE__,dlocal,d2,0);
    ok << testOffset(__LINE__,dlocal,d3,0);
    ok << testOffset(__LINE__,dlocal,d4,0);

    ok << testOffset(__LINE__,dutc,d1,-offset);

    s = dlocal.toString("yyyy-MM-dd hh:mm:ss.zzz t"); // with abbreviated timezone
    //qDebug() << s;

    d1 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, true, true).toDateTime();
    d2 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, false, false).toDateTime();
    d3 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, false, true).toDateTime();
    d4 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, true, false).toDateTime();

    // d1 TT 12MSK 12MSK
    // d2 FF 12MSK  9GMT
    // d3 FT 12MSK 12MSK
    // d4 TF 12MSK  9GMT

    ok << testSpec(__LINE__,d1,Qt::LocalTime);
    ok << testSpec(__LINE__,d2,Qt::UTC);
    ok << testSpec(__LINE__,d3,Qt::LocalTime);
    ok << testSpec(__LINE__,d4,Qt::UTC);

    ok << testOffset(__LINE__,dlocal,d1,0);
    ok << testOffset(__LINE__,dlocal,d2,0);
    ok << testOffset(__LINE__,dlocal,d3,0);
    ok << testOffset(__LINE__,dlocal,d4,0);

    ok << testOffset(__LINE__,dutc,d1,-offset);

    qDebug() << "testTryConvert2" << (allTrue(ok) ? "passed" : "failed");
}


bool Tests::testApParse() {
    QList<bool> ok;
    ok << equals("10:20 AM",QTime(10,20,30).toString("h:mm AP"));
    ok << isValid(QTime::fromString("10:20 AM","h:mm AP"));
    bool passed = allTrue(ok);
    qDebug() << "testApParse" << (passed ? "passed" : "failed");
    return passed;
}
