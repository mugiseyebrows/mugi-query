#include <QTest>

#include "sdata.h"

STable mockTable(const QString& schema, TableType type, const QString &name, const QStringList &columnNames)
{
    QList<SColumn> columns;
    for(const QString& name: columnNames) {
        columns.append(SColumn(name, "int"));
    }
    return STable(SName(schema, name), columns);
}

STable mockTable(const QString &name, const QStringList &columnNames)
{
    QList<SColumn> columns;
    for(const QString& name: columnNames) {
        columns.append(SColumn(name, "int"));
    }
    return STable(SName(name), columns);
}

STable mockTable(const QString& schema, const QString &name, const QStringList &columnNames)
{
    QList<SColumn> columns;
    for(const QString& name: columnNames) {
        columns.append(SColumn(name, "int"));
    }
    return STable(SName(schema, name), columns);
}

class tst_SData : public QObject {
    Q_OBJECT
public:

private slots:
    void testDiffBasic();
    void testDiffEmpty();
    void testDiffSchema();
    void testHash();
};


void tst_SData::testDiffBasic()
{
    QList<STable> tables1 = {
                             mockTable("foo", {"a", "b", "c"}),
                             mockTable("bar", {"d", "e", "f"}),
                             mockTable("baz", {"g", "h", "i"}),
                             mockTable("moo", {"a", "b", "c"}),
                             };
    QList<STable> tables2 = {
                             mockTable("foo", {"a", "b", "c"}),
                             mockTable("bar1", {"d", "e", "f"}),
                             mockTable("qix", {"j", "k", "l"}),
                             mockTable("moo", {"a", "b", "d"}),
                             };

    STablesDiff diff = getDiff(tables1, tables2);

    auto created = diff.created;
    auto dropped = diff.dropped;
    auto renamed = diff.renamed;
    auto altered = diff.altered;


    QCOMPARE(created.size(), 1);
    QCOMPARE(created[0].name.name, "qix");
    QCOMPARE(dropped.size(), 1);
    QCOMPARE(dropped[0].fullname(), "baz");
    QCOMPARE(renamed.size(), 1);
    QCOMPARE(renamed[0].oldName.name, "bar");
    QCOMPARE(renamed[0].newName.name, "bar1");
    QCOMPARE(altered.size(), 1);
    QCOMPARE(altered[0].name.name, "moo");

}

void tst_SData::testDiffEmpty()
{
    QList<STable> tables1 = {};
    QList<STable> tables2 = {
                             mockTable("foo", {"a", "b", "c"}),
                             mockTable("bar1", {"d", "e", "f"}),
                             mockTable("qix", {"j", "k", "l"}),
                             mockTable("moo", {"a", "b", "d"}),
                             };

    STablesDiff diff = getDiff(tables1, tables2);

    auto created = diff.created;
    auto dropped = diff.dropped;
    auto renamed = diff.renamed;
    auto altered = diff.altered;

    QCOMPARE(created.size(), 4);
    QCOMPARE(dropped.size(), 0);
    QCOMPARE(renamed.size(), 0);
    QCOMPARE(altered.size(), 0);


}

void tst_SData::testDiffSchema() {

    QList<STable> tables1 = {
                             mockTable("a", "foo", {"a", "b", "c"}),
                             mockTable("a", "bar", {"d", "e", "f"}),
                             };

    QList<STable> tables2 = {
                             mockTable("a", "foo", {"a", "b", "c"}),
                             mockTable("b", "bar", {"d", "e", "f"}),
                             };

    STablesDiff diff = getDiff(tables1, tables2);

    auto created = diff.created;
    auto dropped = diff.dropped;
    auto renamed = diff.renamed;
    auto altered = diff.altered;

    QCOMPARE(created.size(), 0);
    QCOMPARE(dropped.size(), 0);
    QCOMPARE(renamed.size(), 1);
    QCOMPARE(altered.size(), 0);


}

void tst_SData::testHash() {

    QHash<SName, int> data;
    data[SName("foo", "bar")] = 1;
    data[SName("foo", "BAR")] = 2;

    QCOMPARE(data.size(), 1);
    QCOMPARE(data[SName("foo", "bar")], 2);
    QCOMPARE(SName("foo", "bar"), SName("foo", "BAR"));
}



QTEST_MAIN(tst_SData)
#include "tst_sdata.moc"
