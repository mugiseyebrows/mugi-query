#include <QTest>

#include "emmet.h"
#include <QRegularExpression>
#include <QDebug>

class tst_Emmet : public QObject {
    Q_OBJECT
private slots:
    void testCw_data();
    void testCw();
};

void tst_Emmet::testCw_data()
{
    QTest::addColumn<QString>("in");
    QTest::addColumn<QString>("expected");
    QTest::addRow("1") << "cwt" << "case when # then # else # end";
    QTest::addRow("2") << "cwtee" << "case when # then # else # end";
    QTest::addRow("3") << "scwtw" << "select case when # then # else # end where #";
}

static QStringList wssplit(const QString& text) {
    return text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
}

void tst_Emmet::testCw()
{
    QFETCH(QString, in);
    QFETCH(QString, expected);
    QString error;
    QString parsed = Emmet::parse(in, error);
    if (!error.isEmpty()) {
        qDebug() << error;
    }
    //qDebug() << parsed;
    QCOMPARE(wssplit(parsed), wssplit(expected));
}

QTEST_MAIN(tst_Emmet)
#include "tst_emmet.moc"
