#include "timezone.h"

TimeZone::TimeZone() {
}

TimeZone::TimeZone(const QString& code, const QString& name, const QByteArray& ianaId,
                   const QString& utc, int offset)
    : mCode(code), mName(name), mIanaId(ianaId), mUtc(utc), mOffset(offset) {
}

QString TimeZone::code() const {
    return mCode;
}

void TimeZone::setCode(const QString& code) {
    mCode = code;
}

QString TimeZone::name() const {
    return mName;
}

void TimeZone::setName(const QString& name) {
    mName = name;
}

QByteArray TimeZone::ianaId() const {
    return mIanaId;
}

void TimeZone::setIanaId(const QByteArray& ianaId) {
    mIanaId = ianaId;
}

QString TimeZone::utc() const {
    return mUtc;
}

void TimeZone::setUtc(const QString& utc) {
    mUtc = utc;
}

int TimeZone::offset() const {
    return mOffset;
}

void TimeZone::setOffset(int offset) {
    mOffset = offset;
}

bool TimeZone::isValid() const {
    return !mCode.isEmpty() && !mIanaId.isEmpty();
}
