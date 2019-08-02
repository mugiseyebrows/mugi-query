#ifndef TESTDATETIMESAMPLE_H
#define TESTDATETIMESAMPLE_H
#include <QDate>
#include <QDateTime>
#include <QString>

class TestDateTimeSample {
public:
    TestDateTimeSample();
    TestDateTimeSample(const QDateTime& dateTime, const QString& string, Qt::DateFormat format);
    QDateTime dateTime() const;
    void setDateTime(const QDateTime& dateTime);
    QString string() const;
    void setString(const QString& string);
    Qt::DateFormat format() const;
    void setFormat(Qt::DateFormat format);

protected:
    QDateTime mDateTime;
    QString mString;
    Qt::DateFormat mFormat;
};
#endif // TESTDATETIMESAMPLE_H
