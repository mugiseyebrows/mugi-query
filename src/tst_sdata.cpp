#include <QTest>

#include "sdata.h"

STable mockTable(const QString &name, const QStringList &columnNames)
{
    QList<SColumn> columns;
    for(const QString& name: columnNames) {
        columns.append(SColumn(name, "int"));
    }
    return STable(name, columns);
}

class tst_SData : public QObject {
    Q_OBJECT
public:

private slots:
    void testDiffBasic();
    void testDiffEmpty();
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
    QCOMPARE(created[0].name, "qix");
    QCOMPARE(dropped.size(), 1);
    QCOMPARE(dropped[0], "baz");
    QCOMPARE(renamed.size(), 1);
    QCOMPARE(renamed[0].oldName, "bar");
    QCOMPARE(renamed[0].newName, "bar1");
    QCOMPARE(altered.size(), 1);
    QCOMPARE(altered[0].name, "moo");

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

QTEST_MAIN(tst_SData)
#include "tst_sdata.moc"
