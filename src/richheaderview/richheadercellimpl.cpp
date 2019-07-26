#include "richheadercellimpl.h"

#include <QWidget>

RichHeaderCellImpl::RichHeaderCellImpl()
    : mRow(-1), mColumn(-1), mRowSpan(1), mColumnSpan(1), mText(QString()), mMultiline(false),
      mElide(Qt::ElideNone), mAlign(Qt::AlignCenter), mRotation(0.0), mWidget(0), mVisible(true) {
}

RichHeaderCellImpl::RichHeaderCellImpl(int row, int column, int rowSpan, int columnSpan,
                                       const QString& text, bool multiline, Qt::TextElideMode elide,
                                       Qt::Alignment align, double rotation, QWidget* widget,
                                       bool visible)
    : mRow(row), mColumn(column), mRowSpan(rowSpan), mColumnSpan(columnSpan), mText(text),
      mMultiline(multiline), mElide(elide), mAlign(align), mRotation(rotation), mWidget(widget),
      mVisible(visible), mPaddingTop(0), mPaddingRight(0), mPaddingBottom(0), mPaddingLeft(0) {
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

void RichHeaderCellImpl::padding(int all) {
    mPaddingTop = all;
    mPaddingRight = all;
    mPaddingBottom = all;
    mPaddingLeft = all;
}

void RichHeaderCellImpl::padding(int vertical, int horizontal)
{
    mPaddingTop = vertical;
    mPaddingRight = horizontal;
    mPaddingBottom = vertical;
    mPaddingLeft = horizontal;
}

void RichHeaderCellImpl::padding(int top, int right, int bottom, int left)
{
    mPaddingTop = top;
    mPaddingRight = right;
    mPaddingBottom = bottom;
    mPaddingLeft = left;
}

int RichHeaderCellImpl::paddingTop() const
{
    return mPaddingTop;
}

int RichHeaderCellImpl::paddingRight() const
{
    return mPaddingRight;
}

int RichHeaderCellImpl::paddingBottom() const
{
    return mPaddingBottom;
}

int RichHeaderCellImpl::paddingLeft() const
{
    return mPaddingLeft;
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

QDebug operator<<(QDebug debug, const RichHeaderCellImpl& cell) {
    QString s = QString("Cell(row: %1, column: %2, rowSpan: %3, colSpan: %4")
                    .arg(cell.row())
                    .arg(cell.column())
                    .arg(cell.rowSpan())
                    .arg(cell.columnSpan());
    debug.nospace() << s.toStdString().c_str();
    return debug.space();
}
