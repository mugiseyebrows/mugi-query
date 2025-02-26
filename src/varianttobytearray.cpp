#include "varianttobytearray.h"

#include <QVariant>
#include <QByteArray>

template<typename T>
QByteArray toByteArray(T t) {
    QByteArray b = QByteArray::fromRawData((char*)&t, sizeof(T));
    b.detach();
    return b;
}

QByteArray variantToByteArray(const QVariant& value) {

    QByteArray data;
    switch(value.typeId()) {
    case QMetaType::Int:
        data = toByteArray(value.toInt());
        break;
    case QMetaType::UInt:
        data = toByteArray(value.toUInt());
        break;
    case QMetaType::Long:
        data = toByteArray(value.toInt());
        break;
    case QMetaType::ULong:
        data = toByteArray(value.toUInt());
        break;
    case QMetaType::Short:
        data = toByteArray((short) value.toInt());
        break;
    case QMetaType::UShort:
        data = toByteArray((unsigned short) value.toUInt());
        break;
    case QMetaType::Char:
        data = toByteArray((char) value.toInt());
        break;
    case QMetaType::UChar:
        data = toByteArray((unsigned char) value.toInt());
        break;
    case QMetaType::LongLong:
        data = toByteArray(value.toLongLong());
        break;
    case QMetaType::ULongLong:
        data = toByteArray(value.toULongLong());
        break;
    default:
        data = value.toByteArray();
    }
    return data;
}
