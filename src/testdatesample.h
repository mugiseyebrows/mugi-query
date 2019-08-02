#ifndef TESTDATESAMPLE_H
#define TESTDATESAMPLE_H
#include <QDate>
#include <QString>

class TestDateSample {
public:
    TestDateSample();
    TestDateSample(const QDate& date, const QString& string, Qt::DateFormat format);
    QDate date() const;
    void setDate(const QDate& date);
    QString string() const;
    void setString(const QString& string);
    Qt::DateFormat format() const;
    void setFormat(Qt::DateFormat format);

protected:
    QDate mDate;
    QString mString;
    Qt::DateFormat mFormat;
};
#endif // TESTDATESAMPLE_H
