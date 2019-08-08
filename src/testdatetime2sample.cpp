#include "testdatetime2sample.h"

TestDateTime2Sample::TestDateTime2Sample() : mHasSeconds(false), mHasMilliseconds(false) {
}

TestDateTime2Sample::TestDateTime2Sample(const QString& string, bool hasSeconds,
                                         bool hasMilliseconds)
    : mString(string), mHasSeconds(hasSeconds), mHasMilliseconds(hasMilliseconds) {
}

QString TestDateTime2Sample::string() const {
    return mString;
}

void TestDateTime2Sample::setString(const QString& string) {
    mString = string;
}

bool TestDateTime2Sample::hasSeconds() const {
    return mHasSeconds;
}

void TestDateTime2Sample::setHasSeconds(bool hasSeconds) {
    mHasSeconds = hasSeconds;
}

bool TestDateTime2Sample::hasMilliseconds() const {
    return mHasMilliseconds;
}

void TestDateTime2Sample::setHasMilliseconds(bool hasMilliseconds) {
    mHasMilliseconds = hasMilliseconds;
}
