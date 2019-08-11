#include "field.h"

Field::Field()
    : mSize(-1), mPrimaryKey(false), mAutoincrement(false), mIndex(false), mUnique(false) {
}

Field::Field(const QString& name, const QString& type, int size, bool primaryKey,
             bool autoincrement, bool index, bool unique)
    : mName(name), mType(type), mSize(size), mPrimaryKey(primaryKey), mAutoincrement(autoincrement),
      mIndex(index), mUnique(unique) {
}

QString Field::name() const {
    return mName;
}

void Field::setName(const QString& name) {
    mName = name;
}

QString Field::type() const {
    return mType;
}

void Field::setType(const QString& type) {
    mType = type;
}

int Field::size() const {
    return mSize;
}

void Field::setSize(int size) {
    mSize = size;
}

bool Field::primaryKey() const {
    return mPrimaryKey;
}

void Field::setPrimaryKey(bool primaryKey) {
    mPrimaryKey = primaryKey;
}

bool Field::autoincrement() const {
    return mAutoincrement;
}

void Field::setAutoincrement(bool autoincrement) {
    mAutoincrement = autoincrement;
}

bool Field::index() const {
    return mIndex;
}

void Field::setIndex(bool index) {
    mIndex = index;
}

bool Field::unique() const {
    return mUnique;
}

void Field::setUnique(bool unique) {
    mUnique = unique;
}
