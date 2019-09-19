// DO NOT EDIT ** This file was generated by mugicpp ** DO NOT EDIT
#include "richheaderdata.h"
#include "richheaderdataimpl.h"

RichHeaderData::RichHeaderData(RichHeaderDataImpl* impl) : mImpl(impl) {
}
bool RichHeaderData::multiline() const {
    return mImpl->multiline();
}
RichHeaderData& RichHeaderData::multiline(bool value) {
    mImpl->multiline(value);
    return *this;
}
Qt::TextElideMode RichHeaderData::elide() const {
    return mImpl->elide();
}
RichHeaderData& RichHeaderData::elide(Qt::TextElideMode value) {
    mImpl->elide(value);
    return *this;
}
Qt::Alignment RichHeaderData::align() const {
    return mImpl->align();
}
RichHeaderData& RichHeaderData::align(Qt::Alignment value) {
    mImpl->align(value);
    return *this;
}
double RichHeaderData::rotation() const {
    return mImpl->rotation();
}
RichHeaderData& RichHeaderData::rotation(double value) {
    mImpl->rotation(value);
    return *this;
}
Qt::Orientation RichHeaderData::tabDirection() const {
    return mImpl->tabDirection();
}
RichHeaderData& RichHeaderData::tabDirection(Qt::Orientation value) {
    mImpl->tabDirection(value);
    return *this;
}
bool RichHeaderData::stretchFixed() const {
    return mImpl->stretchFixed();
}
RichHeaderData& RichHeaderData::stretchFixed(bool value) {
    mImpl->stretchFixed(value);
    return *this;
}
QList<int> RichHeaderData::subsectionSizes() const {
    return mImpl->subsectionSizes();
}
RichHeaderData& RichHeaderData::subsectionSizes(const QList<int>& value) {
    mImpl->subsectionSizes(value);
    return *this;
}
QColor RichHeaderData::highlightColor() const {
    return mImpl->highlightColor();
}
RichHeaderData& RichHeaderData::highlightColor(const QColor& value) {
    mImpl->highlightColor(value);
    return *this;
}
bool RichHeaderData::flatStyle() const {
    return mImpl->flatStyle();
}
RichHeaderData& RichHeaderData::flatStyle(bool value) {
    mImpl->flatStyle(value);
    return *this;
}
RichHeaderCell RichHeaderData::cell(int row, int column) {
    return mImpl->cell(row, column);
}
bool RichHeaderData::hasCell(int row, int column) {
    return mImpl->hasCell(row, column);
}
RichHeaderData& RichHeaderData::pull(RichHeaderDirection::DirectionType direction,
                                     int sectionCount) {
    mImpl->pull(direction, sectionCount);
    return *this;
}
RichHeaderData& RichHeaderData::clear() {
    mImpl->clear();
    return *this;
}
RichHeaderData& RichHeaderData::pullUp(int sectionCount) {
    mImpl->pullUp(sectionCount);
    return *this;
}
RichHeaderData& RichHeaderData::pullDown(int sectionCount) {
    mImpl->pullDown(sectionCount);
    return *this;
}
RichHeaderData& RichHeaderData::pullLeft(int sectionCount) {
    mImpl->pullLeft(sectionCount);
    return *this;
}
RichHeaderData& RichHeaderData::pullRight(int sectionCount) {
    mImpl->pullRight(sectionCount);
    return *this;
}
