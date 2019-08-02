#include "testdatesample.h"

TestDateSample::TestDateSample() {
}

TestDateSample::TestDateSample(const QDate& date, const QString& string, Qt::DateFormat format)
    : mDate(date), mString(string), mFormat(format) {
}

QDate TestDateSample::date() const {
    return mDate;
}

void TestDateSample::setDate(const QDate& date) {
    mDate = date;
}

QString TestDateSample::string() const {
    return mString;
}

void TestDateSample::setString(const QString& string) {
    mString = string;
}

Qt::DateFormat TestDateSample::format() const {
    return mFormat;
}

void TestDateSample::setFormat(Qt::DateFormat format) {
    mFormat = format;
}
