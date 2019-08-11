#include "tests.h"

#include <QStringList>
#include "jointokenlist.h"

#include "zipunzip.h"
#include <QDebug>
#include "queryparser.h"
#include "testdatetimesample.h"
#include "testdatesample.h"
#include "datetime.h"
#include "sqldatatypes.h"
#include <QTimeZone>
#include "timezone.h"
#include <QTextCodec>
#include "timezones.h"
#include "testdatetime2sample.h"

namespace {

QString tail(const QString& s) {
    return s + "$";
}

}

void Tests::run()
{

    testSplit();
    testJoinSplit();
    testAliases();
    testFlatQueries();
    testClosingBracket();
    testParseCreateTable();
    textParseCreateTableCreateDefinition();
    //testDateRegularExpressions();
    //testDateTimeRegularExpressions();
    testTryConvert1();
    testTryConvert2();
    //testApParse();
    //testTimeZones();
    testDateTimeParse();
    testTableNamesFromSelectQuery();
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

bool equals(int id, int a, int b) {
    if (a == b) {
        return true;
    }
    qDebug() << id << "not equal, expected: " << a << ", actual " << b;
    return false;
}

bool equals(int id, const QMap<QString,QString>& e, const QMap<QString,QString>& a) {
    if (e == a) {
        return true;
    }
    QString e_ = mapAsString(e);
    QString a_ = mapAsString(a);
    qDebug() << id << "not equal, expected: " << e_.toStdString().c_str() << ", actual " << a_.toStdString().c_str();
    return false;
}

bool equals(int id, const QList<QPair<JoinToken::JoinToken,QString> >& e, const QList<QPair<JoinToken::JoinToken,QString> >& a) {

    QStringList es = mapTrimmed(unzipSeconds(e));
    QStringList as = mapTrimmed(unzipSeconds(a));
    QList<JoinToken::JoinToken> ef = unzipFirsts(e);
    QList<JoinToken::JoinToken> af = unzipFirsts(a);
    if (as == es && ef == af) {
        return true;
    }
    QString e_ = tokenListAsString(e);
    QString a_ = tokenListAsString(a);
    qDebug() << id << "not equal, expected: " << e_.toStdString().c_str() << ", actual " << a_.toStdString().c_str();
    return false;
}

bool equals(int id, const QPair<QString,QStringList>& e, const QPair<QString,QStringList>& a) {
    if (e == a) {
        return true;
    }
    QString e_ = e.first + "{" + e.second.join("|") + "}";
    QString a_ = a.first + "{" + a.second.join("|") + "}";
    qDebug() << id << "not equal, expected: " << e_.toStdString().c_str() << ", actual " << a_.toStdString().c_str();
    return false;
}

bool equals(int id, const QString& e, const QString& a) {
    if (e == a) {
        return true;
    }
    qDebug() << id << "not equal, expected: " << e << ", actual " << a;
    return false;
}

bool equals(int id, const QStringList e, const QStringList a) {
    QString e_ = "{" + e.join("|") + "}";
    QString a_ = "{" + a.join("|") + "}";
    if (e_ == a_) {
        return true;
    }
    qDebug() << id << "not equal, expected: " << e_.toStdString().c_str() << ", actual " << a_.toStdString().c_str();
    return false;
}

bool isValid(int id, const QTime& time) {
    if (!time.isValid()) {
        qDebug() << id << "time is not valid";
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

bool equals(int id, const QDate& date, Qt::DateFormat format,
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

bool equals(int id, const QDate& date, const QString& format,
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

bool equals(int id, const QTime& time, Qt::DateFormat format,
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

bool equals(int id, const QDateTime& dateTimeExpected, Qt::DateFormat format,
                    const QString& string, const QVariant& converted, bool ok) {

    if (converted.isNull()) {
        qDebug() << "test" << id << "tryConvert return null" << dateTimeExpected << format << string << converted << ok;
        return false;
    }

    QDateTime dateTime = converted.toDateTime();

    if (!ok) {
        qDebug() << "test" << id << "tryConvert ok == false" << dateTimeExpected << format << string << converted << ok;
        return false;
    }

    if (dateTime == dateTimeExpected) {
        return true;
    }

    int part = 0;

    int offset;
    int offsetExpected;

    if (dateTimeExpected.date() != dateTime.date()) {
        part = 1;
    } else if (dateTimeExpected.time() != dateTime.time()) {
        part = 2;
    } else {
        offsetExpected = dateTimeExpected.offsetFromUtc();
        offset = dateTime.offsetFromUtc();
        if (offset != offsetExpected) {
            part = 3;
        }
    }

    if (part != 0) {
        static QStringList parts = {"", "date", "time", "offset"};
        qDebug() << id << "tryConvert converted with error";
        qDebug() << "input" <<  string;
        qDebug() << "expected" << dateTimeExpected;
        qDebug() << "UTC" << dateTimeExpected.toUTC();
        qDebug() << "got" << converted;
        qDebug() << "UTC" << dateTime.toUTC();
        qDebug() << "different" << parts[part];
        if (part == 3) {
            qDebug() << "offset expected" << offsetExpected << "got" << offset;
        }
        return false;
    }

    return true;
}


} // namespace

bool Tests::testAliases() {
    QList<bool> ok;

    ok << equals(__LINE__,
    {},
                 QueryParser::aliases("select * from foo, bar left join baz"));

    ok << equals(__LINE__,
    {{"f1","foo"},{"b1","bar"},{"b2","baz"},{"q1","qix"}},
                 QueryParser::aliases("select * from foo f1, bar b1 left join baz b2 on baz.id=bar.id join qix q1"));

    ok << equals(__LINE__,
    {{"b1","bar"},{"b2","baz"},{"q1","qix"},{"z1","zen"}},
                 QueryParser::aliases("select * from (select abc from zen z1 having 2) x, bar b1 left join baz b2 on baz.id=bar.id join qix q1 where a=b"));

    bool passed = allTrue(ok);
    qDebug() << "testAliases" << (passed ? "passed" : "failed");
    return passed;
}

bool Tests::testJoinSplit() {
    QList<bool> ok;

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
    ok << equals(__LINE__,e,a);

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
    ok << equals(__LINE__,e,a);
    bool passed = allTrue(ok);
    qDebug() << "testJoinSplit" << (passed ? "passed" : "failed");
    return passed;
}


bool Tests::testSplit() {

    QList<bool> ok;

    ok << equals(__LINE__,{"foo","bar","baz"},QueryParser::split("foo;bar;baz"));
    ok << equals(__LINE__,{"","",""},QueryParser::split(";;"));
    ok << equals(__LINE__,{"foo","bar--;baz"},QueryParser::split("foo;bar--;baz"));
    ok << equals(__LINE__,{"foo","bar","--baz"},QueryParser::split("foo;bar;--baz"));
    ok << equals(__LINE__,{"foo--\n","bar"},QueryParser::split("foo--\n;bar"));
    ok << equals(__LINE__,{"foo--;\nbar"},QueryParser::split("foo--;\nbar"));
    ok << equals(__LINE__,{"foo'bar'","baz"},QueryParser::split("foo'bar';baz"));
    ok << equals(__LINE__,{},QueryParser::split("foo'bar;baz"));
    ok << equals(__LINE__,{"foo'bar--'","baz"},QueryParser::split("foo'bar--';baz"));
    ok << equals(__LINE__,{"foo'bar'--;baz"},QueryParser::split("foo'bar'--;baz"));
    ok << equals(__LINE__,{"foo/*bar;*/baz"},QueryParser::split("foo/*bar;*/baz"));
    ok << equals(__LINE__,{"foo/*bar*/","baz"},QueryParser::split("foo/*bar*/;baz"));
    ok << equals(__LINE__,{"foo--/*bar\n","*/baz"},QueryParser::split("foo--/*bar\n;*/baz"));
    ok << equals(__LINE__,{"foo'/*bar'","*/baz"},QueryParser::split("foo'/*bar';*/baz"));
    bool passed = allTrue(ok);
    qDebug() << "testSplit" << (passed ? "passed" : "failed");
    return passed;
}

bool Tests::testFlatQueries() {
    QList<bool> ok;

    ok << equals(__LINE__,
    {"select foo from () union ()",
     "select * from bar",
     "select baz from qix"},
                 QueryParser::flatQueries("select foo from (select * from bar) union (select baz from qix)"));

    ok << equals(__LINE__,
    {"select a,b,c from () where 1",
     "select foo from () union ()",
     "select * from bar",
     "select baz from qix"},
                 QueryParser::flatQueries("select a,b,c from (select foo from (select * from bar) union (select baz from qix)) where 1"));

    ok << equals(__LINE__,
    {"select * from foo"},
                 QueryParser::flatQueries("select * from foo"));

    bool passed = allTrue(ok);
    qDebug() << "testFlatQueries" << (passed ? "passed" : "failed");
    return passed;
}

bool Tests::testClosingBracket() {
    QList<bool> ok;
    ok << equals(__LINE__,1,QueryParser::closingBracket("()",0));
    ok << equals(__LINE__,3,QueryParser::closingBracket("(())",0));
    ok << equals(__LINE__,2,QueryParser::closingBracket("(())",1));
    QString t = "create table foo(bar int(11),baz text)";
    ok << equals(__LINE__,t.lastIndexOf(")"),QueryParser::closingBracket(t,t.indexOf("(")));
    bool passed = allTrue(ok);
    qDebug() << "testClosingBracket" << (passed ? "passed" : "failed");
    return passed;
}

bool Tests::testParseCreateTable()
{
    typedef QPair<QString,QStringList> X;
    QList<bool> ok;
    ok << equals(__LINE__,X("foo",{"a","b"}),QueryParser::parseCreateTable("create table foo(a,b)"));
    ok << equals(__LINE__,X("foo",{"a","b"}),QueryParser::parseCreateTable("create temporary table if not exists foo(a,b)"));
    ok << equals(__LINE__,X("foo",{"a","b"}),QueryParser::parseCreateTable("create table foo like bar(a,b)"));
    bool passed = allTrue(ok);
    qDebug() << "testParseCreateTable" << (passed ? "passed" : "failed");
    return passed;
}

bool Tests::textParseCreateTableCreateDefinition() {
    QString err;
    QList<bool> ok;
    ok << equals(__LINE__,
    {"foo","int(11)","NOT NULL","",""},
                 QueryParser::parseCreateTableCreateDefinition("`foo` int(11) NOT NULL",err));
    ok << equals(__LINE__,
    {"bar","text","","",""},
                 QueryParser::parseCreateTableCreateDefinition("`bar` text",err));
    ok << equals(__LINE__,
    {"foo","int unsigned zerofill","null","",""},
                 QueryParser::parseCreateTableCreateDefinition("foo int unsigned zerofill null",err));
    ok << equals(__LINE__,
    {"foo","varchar(100)","","DEFAULT NULL",""},
                 QueryParser::parseCreateTableCreateDefinition("`foo` varchar(100) DEFAULT NULL",err));
    ok << equals(__LINE__,
    {"bar","int(11)","NOT NULL","DEFAULT '0'",""},
                 QueryParser::parseCreateTableCreateDefinition("`bar` int(11) NOT NULL DEFAULT '0'",err));
    bool passed = allTrue(ok);
    qDebug() << "textParseCreateTableCreateDefinition" << (passed ? "passed" : "failed");
    return passed;
}

#if 0
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
#endif

#if 0
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
#endif

bool Tests::testTryConvert1() {

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
    bool outUtc = false;

    QRegularExpression rxTimeWithMs("[0-9]+:[0-9]+:[0-9]+[.][0-9]+");
    QRegularExpression rxTimeWithS("[0-9]+:[0-9]+:[0-9]+");

    for(int i=0;i<100;i++) {

        int y = 1900 + (rand() % 200);
        int M = 1 + (rand() % 12);
        int d = 1 + (rand() % 28);

        //y = 1950 + (rand() % 50);
        //y = 1991;

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
            QVariant converted = SqlDataTypes::tryConvert(string,QVariant::Date,locale,inLocalDateTime,outUtc,&ok);
            //if ((rand() % 40) == 0) converted = converted.toDate().addDays(1);
            passed << equals(__LINE__,date,format,string,converted,ok);
        }

        foreach(const QString& format, dateFormats2) {
            QString string = date.toString(format);
            bool ok;
            QVariant converted = SqlDataTypes::tryConvert(string,QVariant::Date,locale,inLocalDateTime,outUtc,&ok);
            //if ((rand() % 40) == 0) converted = converted.toDate().addDays(1);
            passed << equals(__LINE__,date,format,string,converted,ok);
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
            QVariant converted = SqlDataTypes::tryConvert(string, QVariant::Time, locale,inLocalDateTime,outUtc,&ok);
            QTime time_ = QTime(h, m, hasSeconds ? s : 0, hasMilliseconds ? ms : 0);
            //if ((rand() % 40) == 0) converted = converted.toTime().addSecs(1);
            passed << equals(__LINE__,time_,format,string,converted,ok);
        }

        // datetime

        foreach(Qt::DateFormat format, dateFormats) {
            QString string = dateTime.toString(format);
            bool hasSeconds = rxTimeWithS.match(string).hasMatch();
            bool hasMilliseconds = rxTimeWithMs.match(string).hasMatch();

            bool hasTimeZone = QRegularExpression(tail(TimeZones::regExp())).match(string).hasMatch();
            if (hasTimeZone) {
                // TODO historical timezone abbreviations
/*
input "пятница, 12 апреля 1991 г. 6:49:28 EEST"
expected QDateTime(1991-04-12 06:49:28.000 EEST Qt::TimeSpec(LocalTime))
UTC QDateTime(1991-04-12 03:49:28.000 UTC Qt::TimeSpec(UTC))
got QVariant(QDateTime, QDateTime(1991-04-12 06:49:28.000 EET Qt::TimeSpec(TimeZone) Asia/Amman ))
UTC QDateTime(1991-04-12 04:49:28.000 UTC Qt::TimeSpec(UTC))
*/
                continue;
            }

            QDateTime dateTime_ = QDateTime(QDate(y, M, d),
                                            QTime(h, m, hasSeconds ? s : 0, hasMilliseconds ? ms : 0),
                                            Qt::LocalTime);
            bool ok;
            QVariant converted = SqlDataTypes::tryConvert(string,QVariant::DateTime,locale,inLocalDateTime,outUtc,&ok);
            //if ((rand() % 40) == 0) converted = converted.toDateTime().addDays(1);
            passed << equals(__LINE__,dateTime_,format,string,converted,ok);

        }


    } // for

    bool passed_ = allTrue(passed);
    qDebug() << "testTryConvert1" << (passed_ ? "passed" : "failed");
    return passed_;
}



bool Tests::testTryConvert2() {

    QLocale locale;

    QList<bool> ok;

    QDateTime dlocal(QDate(2000,1,1),QTime(12,0,0),Qt::LocalTime);

    QDateTime dutc(QDate(2000,1,1),QTime(12,0,0),Qt::UTC);

    QString s = dlocal.toString(Qt::ISODate); // no timezone
    //qDebug() << s;

    QDateTime d1, d2, d3, d4;

    d1 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, true, false).toDateTime();
    d2 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, false, true).toDateTime();
    d3 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, false, false).toDateTime();
    d4 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, true, true).toDateTime();

    // d1 TF 12MSK 12MSK
    // d2 FT 12GMT 12GMT
    // d3 FF 12GMT 12GMT
    // d4 TT 12MSK  9GMT

    ok << testSpec(__LINE__,d1,Qt::LocalTime);
    ok << testSpec(__LINE__,d2,Qt::UTC);
    ok << testSpec(__LINE__,d3,Qt::UTC);
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

    d1 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, true, false).toDateTime();
    d2 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, false, true).toDateTime();
    d3 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, false, false).toDateTime();
    d4 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, true, true).toDateTime();

    // d1 TF 12MSK 12MSK
    // d2 FT 12MSK  9GMT
    // d3 FF 12MSK 12MSK
    // d4 TT 12MSK  9GMT

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

    d1 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, true, false).toDateTime();
    d2 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, false, true).toDateTime();
    d3 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, false, false).toDateTime();
    d4 = SqlDataTypes::tryConvert(s, QVariant::DateTime, locale, true, true).toDateTime();

    // d1 TF 12MSK 12MSK
    // d2 FT 12MSK  9GMT
    // d3 FF 12MSK 12MSK
    // d4 TT 12MSK  9GMT

    ok << testSpec(__LINE__,d1,Qt::TimeZone);
    ok << testSpec(__LINE__,d2,Qt::UTC);
    ok << testSpec(__LINE__,d3,Qt::TimeZone);
    ok << testSpec(__LINE__,d4,Qt::UTC);

    ok << testOffset(__LINE__,dlocal,d1,0);
    ok << testOffset(__LINE__,dlocal,d2,0);
    ok << testOffset(__LINE__,dlocal,d3,0);
    ok << testOffset(__LINE__,dlocal,d4,0);

    ok << testOffset(__LINE__,dutc,d1,-offset);

    bool passed = allTrue(ok);
    qDebug() << "testTryConvert2" << (passed ? "passed" : "failed");
    return passed;
}


bool Tests::testApParse() {
    QList<bool> ok;
    ok << equals(__LINE__,"10:20 AM",QTime(10,20,30).toString("h:mm AP"));
    ok << isValid(__LINE__,QTime::fromString("10:20 AM","h:mm AP"));
    bool passed = allTrue(ok);
    qDebug() << "testApParse" << (passed ? "passed" : "failed");
    return passed;
}



bool Tests::testTimeZones() {

#if 0
    QStringList codes = {"A", "ACDT", "ACST", "ACWST", "AEDT", "AEST", "AFT", "AKDT", "AKST", "ALMT", "ANAST", "ANAT", "AQTT", "ART", "AWDT", "AWST", "AZOST", "AZOT", "AZST", "AZT", "AoE", "B", "BNT", "BOT", "BRST", "BRT", "BTT", "C", "CAST", "CAT", "CCT", "CEST", "CET", "CHADT", "CHAST", "CHOST", "CHOT", "CHUT", "CIDST", "CIST", "CKT", "CLST", "CLT", "COT", "CVT", "CXT", "ChST", "D", "DAVT", "DDUT", "E", "EASST", "EAST", "EAT", "ECT", "EDT", "EEST", "EET", "EGST", "EGT", "EST", "F", "FET", "FJST", "FJT", "FKST", "FKT", "FNT", "G", "GALT", "GAMT", "GET", "GFT", "GILT", "GMT", "GYT", "H", "HDT", "HKT", "HOVST", "HOVT", "HST", "I", "ICT", "IDT", "IOT", "IRDT", "IRKST", "IRKT", "IRST", "JST", "K", "KGT", "KOST", "KRAST", "KRAT", "KST", "KUYT", "L", "LHDT", "LHST", "LINT", "M", "MAGST", "MAGT", "MART", "MAWT", "MDT", "MHT", "MMT", "MSD", "MSK", "MST", "MUT", "MVT", "MYT", "N", "NCT", "NDT", "NFDT", "NFT", "NOVST", "NOVT", "NPT", "NRT", "NST", "NUT", "NZDT", "NZST", "O", "OMSST", "OMST", "ORAT", "P", "PDT", "PET", "PETST", "PETT", "PGT", "PHOT", "PHT", "PKT", "PMDT", "PMST", "PONT", "PWT", "PYST", "Q", "QYZT", "R", "RET", "ROTT", "S", "SAKT", "SAMT", "SAST", "SBT", "SCT", "SGT", "SRET", "SRT", "SST", "SYOT", "T", "TAHT", "TFT", "TJT", "TKT", "TLT", "TMT", "TOST", "TOT", "TRT", "TVT", "U", "ULAST", "ULAT", "UYST", "UYT", "UZT", "V", "VET", "VLAST", "VLAT", "VOST", "VUT", "W", "WAKT", "WARST", "WAST", "WAT", "WEST", "WET", "WFT", "WGST", "WGT", "WIB", "WIT", "WITA", "WT", "X", "Y", "YAKST", "YAKT", "YAPT", "YEKST", "YEKT", "Z"};

    bool passed = true;

    QDateTime current = QDateTime::currentDateTime();
    foreach(const QString& code, codes) {
        TimeZone timeZone = DateTime::timeZone(code);
        if (!timeZone.isValid()) {
            qDebug() << "invalid timezone" << code;
            continue;
        }
        QTimeZone timeZone_(timeZone.ianaId());

        if (!timeZone_.isValid()) {
            qDebug() << "timezone not available" << timeZone.ianaId();
            continue;
        }

        int offset = timeZone_.offsetFromUtc(current);

        if (offset != timeZone.offset()) {
            qDebug() << "different offset" << offset << timeZone.offset() << code;
            passed = false;
        }

    }

    qDebug() << "testTimeZones" << (passed ? "passed" : "failed");
    return passed;
#endif
    return true;
}

bool Tests::testDateTimeParse() {

    QList<bool> ok;

    QList<TestDateTimeSample> samples = {
        TestDateTimeSample(QDateTime::fromString("2028-12-20T08:15:26.662",Qt::ISODateWithMs),"20 декабря 2028 г. 8:15:26",Qt::DefaultLocaleLongDate),
        TestDateTimeSample(QDateTime::fromString("1992-07-19T11:39:48.073",Qt::ISODateWithMs),"19 июля 1992 г. 11:39:48",Qt::DefaultLocaleLongDate),
        TestDateTimeSample(QDateTime::fromString("2046-10-17T01:00:40.487",Qt::ISODateWithMs),"17 октября 2046 г. 1:00:40",Qt::DefaultLocaleLongDate),
        TestDateTimeSample(QDateTime::fromString("2090-05-19T03:13:56.373",Qt::ISODateWithMs),"19.05.2090 3:13",Qt::DefaultLocaleShortDate),
        TestDateTimeSample(QDateTime::fromString("1961-09-01T20:59:47.497",Qt::ISODateWithMs),"01.09.1961 20:59",Qt::DefaultLocaleShortDate),
        TestDateTimeSample(QDateTime::fromString("1972-03-26T04:53:38.999",Qt::ISODateWithMs),"26.03.1972 4:53",Qt::DefaultLocaleShortDate),
        TestDateTimeSample(QDateTime::fromString("2023-02-14T22:07:56.632",Qt::ISODateWithMs),"2023-02-14T22:07:56",Qt::ISODate),
        TestDateTimeSample(QDateTime::fromString("1970-12-03T02:23:00.553",Qt::ISODateWithMs),"1970-12-03T02:23:00",Qt::ISODate),
        TestDateTimeSample(QDateTime::fromString("2005-08-28T18:07:09.042",Qt::ISODateWithMs),"2005-08-28T18:07:09",Qt::ISODate),
        TestDateTimeSample(QDateTime::fromString("2023-09-28T04:13:07.741",Qt::ISODateWithMs),"2023-09-28T04:13:07.741",Qt::ISODateWithMs),
        TestDateTimeSample(QDateTime::fromString("2013-11-18T13:30:36.271",Qt::ISODateWithMs),"2013-11-18T13:30:36.271",Qt::ISODateWithMs),
        TestDateTimeSample(QDateTime::fromString("1914-06-03T09:33:47.179",Qt::ISODateWithMs),"1914-06-03T09:33:47.179",Qt::ISODateWithMs),
        TestDateTimeSample(QDateTime::fromString("03 Jul 1961 08:29:35 +0300",Qt::RFC2822Date),"03 Jul 1961 08:29:35 +0300",Qt::RFC2822Date),
        TestDateTimeSample(QDateTime::fromString("16 Aug 1968 22:17:41 +0300",Qt::RFC2822Date),"16 Aug 1968 22:17:41 +0300",Qt::RFC2822Date),
        TestDateTimeSample(QDateTime::fromString("16 Oct 2052 17:52:14 +0300",Qt::RFC2822Date),"16 Oct 2052 17:52:14 +0300",Qt::RFC2822Date),
        TestDateTimeSample(QDateTime::fromString("2037-03-15T15:14:27.293",Qt::ISODateWithMs),"15 марта 2037 г. 15:14:27",Qt::SystemLocaleLongDate),
        TestDateTimeSample(QDateTime::fromString("2029-08-25T13:04:34.076",Qt::ISODateWithMs),"25 августа 2029 г. 13:04:34",Qt::SystemLocaleLongDate),
        TestDateTimeSample(QDateTime::fromString("1915-11-01T07:08:27.555",Qt::ISODateWithMs),"1 ноября 1915 г. 7:08:27",Qt::SystemLocaleLongDate),
        TestDateTimeSample(QDateTime::fromString("2066-06-20T06:47:48.554",Qt::ISODateWithMs),"20.06.2066 6:47",Qt::SystemLocaleShortDate),
        TestDateTimeSample(QDateTime::fromString("1950-05-24T04:55:18.792",Qt::ISODateWithMs),"24.05.1950 4:55",Qt::SystemLocaleShortDate),
        TestDateTimeSample(QDateTime::fromString("2016-11-18T20:59:18.583",Qt::ISODateWithMs),"18.11.2016 20:59",Qt::SystemLocaleShortDate),
        TestDateTimeSample(QDateTime::fromString("1970-05-28T04:55:06.498",Qt::ISODateWithMs),"Чт май 28 04:55:06 1970",Qt::TextDate),
        TestDateTimeSample(QDateTime::fromString("2070-12-20T12:23:41.663",Qt::ISODateWithMs),"Сб дек 20 12:23:41 2070",Qt::TextDate),
        TestDateTimeSample(QDateTime::fromString("2006-12-23T19:09:00.164",Qt::ISODateWithMs),"Сб дек 23 19:09:00 2006",Qt::TextDate),
        TestDateTimeSample(QDateTime::fromString("1988-04-19T20:54:17.346",Qt::ISODateWithMs),"вт апр. 19 20:54:17 1988",Qt::TextDate),
        TestDateTimeSample(QDateTime::fromString("1935-02-04T01:23:13.328",Qt::ISODateWithMs),"пн февр. 4 01:23:13 1935",Qt::TextDate),
        TestDateTimeSample(QDateTime::fromString("2025-08-07T18:37:36.963",Qt::ISODateWithMs),"чт авг. 7 18:37:36 2025",Qt::TextDate),
    };

    foreach(const TestDateTimeSample& sample, samples) {
        QDateTime dateTime;
        bool parsed = DateTime::parseAsDateTime(sample.string(), dateTime, 1950, true, false);
        if (!parsed) {
            ok << false;
            qDebug() << "not parsed" << sample.string();
        } else if (!dateTime.isValid()) {
            qDebug() << "!dateTime.isValid()" << sample.string();
        } else {
            bool hasMs = sample.format() == Qt::ISODateWithMs;
            bool hasSeconds = !(sample.format() == Qt::SystemLocaleShortDate || sample.format() == Qt::DefaultLocaleShortDate);
            bool hasTimeZone = sample.format() == Qt::RFC2822Date;
            Qt::TimeSpec timeSpecExpected = (hasTimeZone ? Qt::OffsetFromUTC : Qt::LocalTime);
            if (dateTime.timeSpec() != timeSpecExpected) {
                qDebug() << "timeSpec expected" << timeSpecExpected << "actual" << dateTime.timeSpec();
                ok << false;
            }
            QDateTime dateTimeExpected = sample.dateTime();
            if (!hasMs) {
                dateTimeExpected = dateTimeExpected.addMSecs(-dateTimeExpected.time().msec());
            }
            if (!hasSeconds) {
                dateTimeExpected = dateTimeExpected.addSecs(-dateTimeExpected.time().second());
            }

            if (dateTimeExpected != dateTime) {
                qDebug() << "not equals";
                qDebug() << dateTimeExpected;
                qDebug() << dateTime;
                qDebug() << sample.string();
                qDebug() << sample.format();
                ok << false;
            }
        }
    }

    QList<TestDateTime2Sample> samples2 = {
        TestDateTime2Sample("06.08.2019 8:15 PM"),
        TestDateTime2Sample("06.08.2019 8:15:32 PM",true),
        TestDateTime2Sample("06.08.2019 8:15:32.123 PM",true,true),
        TestDateTime2Sample("06.08.2019 20:15"),
        TestDateTime2Sample("06.08.2019 20:15:32",true),
        TestDateTime2Sample("06.08.2019 20:15:32.123",true,true),
        TestDateTime2Sample("06.08.19 8:15 PM"),
        TestDateTime2Sample("06.08.19 20:15"),
        TestDateTime2Sample("06 августа 2019 г. 8:15 PM"),
        TestDateTime2Sample("06 августа 2019 г. 20:15"),
        TestDateTime2Sample("06 августа 2019 8:15 PM"),
        TestDateTime2Sample("06 августа 2019 20:15"),
        TestDateTime2Sample("6-авг-2019 20:15"),
        TestDateTime2Sample("6/Aug/2019 20:15"),
        TestDateTime2Sample("06-авг-2019 20:15"),
        TestDateTime2Sample("6-авг-2019 8:15 PM"),
    };

    foreach(const TestDateTime2Sample& sample, samples2) {
        QDateTime dateTime;
        if (!DateTime::parseAsDateTime(sample.string(), dateTime, 1950, true, false)) {
            qDebug() << "failed to parse" << sample.string();
            ok << false;
        } else {
            QDateTime dateTimeExpected(QDate(2019,8,6),QTime(20,15));
            if (sample.hasMilliseconds())  {
                dateTimeExpected.setTime(QTime(20,15,32,123));
            } else if (sample.hasSeconds()) {
                dateTimeExpected.setTime(QTime(20,15,32));
            }
            if (dateTime != dateTimeExpected) {
                qDebug() << "not equals";
                qDebug() << dateTime;
                qDebug() << dateTimeExpected;
                qDebug() << sample.string();
                ok << false;
            }
        }
    }



    bool passed = allTrue(ok);
    qDebug() << "testDateTimeParse" << (passed ? "passed" : "failed");
    return passed;
}

bool equals(int id, bool e, bool a) {
    if (e != a) {
        qDebug() << id << "not equal, expected" << e << "actual" << a;
        return false;
    }
    return true;
}

bool Tests::testTableNamesFromSelectQuery() {
    bool many;
    QList<bool> ok;

    ok << equals(__LINE__, QString(), QueryParser::tableNameFromSelectQuery("select 1", &many));
    ok << equals(__LINE__, false, many);
    ok << equals(__LINE__, "bar", QueryParser::tableNameFromSelectQuery("select foo from bar", &many));
    ok << equals(__LINE__, false, many);
    ok << equals(__LINE__, QString(), QueryParser::tableNameFromSelectQuery("select foo from bar, baz", &many));
    ok << equals(__LINE__, true, many);
    ok << equals(__LINE__, "baz", QueryParser::tableNameFromSelectQuery("select foo from (select bar from baz)", &many));
    ok << equals(__LINE__, false, many);
    ok << equals(__LINE__, QString(), QueryParser::tableNameFromSelectQuery("select foo from (select bar from baz, qix)", &many));
    ok << equals(__LINE__, true, many);
    ok << equals(__LINE__, QString(), QueryParser::tableNameFromSelectQuery("select foo from bar left join baz", &many));
    ok << equals(__LINE__, true, many);
    ok << equals(__LINE__, "baz", QueryParser::tableNameFromSelectQuery("select foo from (select bar from baz)", &many));
    ok << equals(__LINE__, false, many);
    ok << equals(__LINE__, QString(), QueryParser::tableNameFromSelectQuery("select foo from (select bar from baz, qix)", &many));
    ok << equals(__LINE__, true, many);
    ok << equals(__LINE__, "bar", QueryParser::tableNameFromSelectQuery("select\nfoo\tfrom\r\nbar", &many));
    ok << equals(__LINE__, false, many);
    ok << equals(__LINE__, QString(), QueryParser::tableNameFromSelectQuery("select\nfoo\nfrom bar\t, \nbaz", &many));
    ok << equals(__LINE__, true, many);

    bool passed = allTrue(ok);
    qDebug() << "testTableNamesFromSelectQuery" << (passed ? "passed" : "failed");
    return passed;
}

