// DO NOT EDIT ** This file was generated by mugicpp ** DO NOT EDIT
#include "richheadercellimpl.h"
#include <QWidget>

RichHeaderCellImpl::RichHeaderCellImpl(int row, int column, int rowSpan, int columnSpan,
                                       const QString& text, bool multiline, Qt::TextElideMode elide,
                                       Qt::Alignment align, double rotation, QWidget* widget,
                                       bool visible, int paddingTop, int paddingRight,
                                       int paddingBottom, int paddingLeft)
    : mRow(row), mColumn(column), mRowSpan(rowSpan), mColumnSpan(columnSpan), mText(text),
      mMultiline(multiline), mElide(elide), mAlign(align), mRotation(rotation), mWidget(widget),
      mVisible(visible), mPaddingTop(paddingTop), mPaddingRight(paddingRight),
      mPaddingBottom(paddingBottom), mPaddingLeft(paddingLeft) {
}
int RichHeaderCellImpl::row() const {
    return mRow;
}
void RichHeaderCellImpl::row(int value) {
    mRow = value;
}
int RichHeaderCellImpl::column() const {
    return mColumn;
}
void RichHeaderCellImpl::column(int value) {
    mColumn = value;
}
int RichHeaderCellImpl::rowSpan() const {
    return mRowSpan;
}
void RichHeaderCellImpl::rowSpan(int value) {
    mRowSpan = value;
}
int RichHeaderCellImpl::columnSpan() const {
    return mColumnSpan;
}
void RichHeaderCellImpl::columnSpan(int value) {
    mColumnSpan = value;
}
QString RichHeaderCellImpl::text() const {
    return mText;
}
void RichHeaderCellImpl::text(const QString& value) {
    mText = value;
}
bool RichHeaderCellImpl::multiline() const {
    return mMultiline;
}
void RichHeaderCellImpl::multiline(bool value) {
    mMultiline = value;
}
Qt::TextElideMode RichHeaderCellImpl::elide() const {
    return mElide;
}
void RichHeaderCellImpl::elide(Qt::TextElideMode value) {
    mElide = value;
}
Qt::Alignment RichHeaderCellImpl::align() const {
    return mAlign;
}
void RichHeaderCellImpl::align(Qt::Alignment value) {
    mAlign = value;
}
double RichHeaderCellImpl::rotation() const {
    return mRotation;
}
void RichHeaderCellImpl::rotation(double value) {
    mRotation = value;
}
QWidget* RichHeaderCellImpl::widget() const {
    return mWidget;
}
void RichHeaderCellImpl::widget(QWidget* value) {
    mWidget = value;
}
bool RichHeaderCellImpl::visible() const {
    return mVisible;
}
void RichHeaderCellImpl::visible(bool value) {
    mVisible = value;
}

QColor RichHeaderCellImpl::textColor()
{
    return mTextColor;
}

void RichHeaderCellImpl::textColor(const QColor &color)
{
    mTextColor = color;
}
int RichHeaderCellImpl::paddingTop() const {
    return mPaddingTop;
}
void RichHeaderCellImpl::paddingTop(int value) {
    mPaddingTop = value;
}
int RichHeaderCellImpl::paddingRight() const {
    return mPaddingRight;
}
void RichHeaderCellImpl::paddingRight(int value) {
    mPaddingRight = value;
}
int RichHeaderCellImpl::paddingBottom() const {
    return mPaddingBottom;
}
void RichHeaderCellImpl::paddingBottom(int value) {
    mPaddingBottom = value;
}
int RichHeaderCellImpl::paddingLeft() const {
    return mPaddingLeft;
}
void RichHeaderCellImpl::paddingLeft(int value) {
    mPaddingLeft = value;
}
void RichHeaderCellImpl::cell(int row, int column) {
    this->row(row);
    this->column(column);
}
void RichHeaderCellImpl::span(int rows, int columns) {
    rowSpan(rows);
    columnSpan(columns);
}
void RichHeaderCellImpl::padding(int all) {
    mPaddingTop = all;
    mPaddingRight = all;
    mPaddingBottom = all;
    mPaddingLeft = all;
}
void RichHeaderCellImpl::padding(int vertical, int horizontal) {
    mPaddingTop = vertical;
    mPaddingRight = horizontal;
    mPaddingBottom = vertical;
    mPaddingLeft = horizontal;
}
void RichHeaderCellImpl::padding(int top, int right, int bottom, int left) {
    mPaddingTop = top;
    mPaddingRight = right;
    mPaddingBottom = bottom;
    mPaddingLeft = left;
}
void RichHeaderCellImpl::show() {
    mVisible = true;
}
void RichHeaderCellImpl::hide() {
    mVisible = false;
}
void RichHeaderCellImpl::incColumnSpan() {
    mColumnSpan++;
}
void RichHeaderCellImpl::incRowSpan() {
    mRowSpan++;
}
void RichHeaderCellImpl::moveUp() {
    mRow--;
}
void RichHeaderCellImpl::moveDown() {
    mRow++;
}
void RichHeaderCellImpl::moveLeft() {
    mColumn--;
}
void RichHeaderCellImpl::moveRight() {
    mColumn++;
}
