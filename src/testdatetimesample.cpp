#include "testdatetimesample.h"

TestDateTimeSample::TestDateTimeSample() {
}

TestDateTimeSample::TestDateTimeSample(const QDateTime& dateTime, const QString& string,
                                       Qt::DateFormat format)
    : mDateTime(dateTime), mString(string), mFormat(format) {
}

QDateTime TestDateTimeSample::dateTime() const {
    return mDateTime;
}

void TestDateTimeSample::setDateTime(const QDateTime& dateTime) {
    mDateTime = dateTime;
}

QString TestDateTimeSample::string() const {
    return mString;
}

void TestDateTimeSample::setString(const QString& string) {
    mString = string;
}

Qt::DateFormat TestDateTimeSample::format() const {
    return mFormat;
}

void TestDateTimeSample::setFormat(Qt::DateFormat format) {
    mFormat = format;
}
