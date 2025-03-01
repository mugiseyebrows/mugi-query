// DO NOT EDIT ** This file was generated by mugicpp ** DO NOT EDIT
#include "richheadercell.h"
#include "richheadercellimpl.h"
#include <QWidget>

RichHeaderCell::RichHeaderCell(RichHeaderCellImpl* impl) : mImpl(impl) {
}
int RichHeaderCell::row() const {
    return mImpl->row();
}
RichHeaderCell& RichHeaderCell::row(int value) {
    mImpl->row(value);
    return *this;
}
int RichHeaderCell::column() const {
    return mImpl->column();
}
RichHeaderCell& RichHeaderCell::column(int value) {
    mImpl->column(value);
    return *this;
}
int RichHeaderCell::rowSpan() const {
    return mImpl->rowSpan();
}
RichHeaderCell& RichHeaderCell::rowSpan(int value) {
    mImpl->rowSpan(value);
    return *this;
}
int RichHeaderCell::columnSpan() const {
    return mImpl->columnSpan();
}
RichHeaderCell& RichHeaderCell::columnSpan(int value) {
    mImpl->columnSpan(value);
    return *this;
}
QString RichHeaderCell::text() const {
    return mImpl->text();
}
RichHeaderCell& RichHeaderCell::text(const QString& value) {
    mImpl->text(value);
    return *this;
}
bool RichHeaderCell::multiline() const {
    return mImpl->multiline();
}
RichHeaderCell& RichHeaderCell::multiline(bool value) {
    mImpl->multiline(value);
    return *this;
}
Qt::TextElideMode RichHeaderCell::elide() const {
    return mImpl->elide();
}
RichHeaderCell& RichHeaderCell::elide(Qt::TextElideMode value) {
    mImpl->elide(value);
    return *this;
}
Qt::Alignment RichHeaderCell::align() const {
    return mImpl->align();
}
RichHeaderCell& RichHeaderCell::align(Qt::Alignment value) {
    mImpl->align(value);
    return *this;
}
double RichHeaderCell::rotation() const {
    return mImpl->rotation();
}
RichHeaderCell& RichHeaderCell::rotation(double value) {
    mImpl->rotation(value);
    return *this;
}
QWidget* RichHeaderCell::widget() const {
    return mImpl->widget();
}
RichHeaderCell& RichHeaderCell::widget(QWidget* value) {
    mImpl->widget(value);
    return *this;
}
bool RichHeaderCell::visible() const {
    return mImpl->visible();
}
RichHeaderCell& RichHeaderCell::visible(bool value) {
    mImpl->visible(value);
    return *this;
}
QColor RichHeaderCell::textColor() const
{
    return mImpl->textColor();
}
RichHeaderCell &RichHeaderCell::textColor(const QColor &color)
{
    mImpl->textColor(color);
    return *this;
}
int RichHeaderCell::paddingTop() const {
    return mImpl->paddingTop();
}
RichHeaderCell& RichHeaderCell::paddingTop(int value) {
    mImpl->paddingTop(value);
    return *this;
}
int RichHeaderCell::paddingRight() const {
    return mImpl->paddingRight();
}
RichHeaderCell& RichHeaderCell::paddingRight(int value) {
    mImpl->paddingRight(value);
    return *this;
}
int RichHeaderCell::paddingBottom() const {
    return mImpl->paddingBottom();
}
RichHeaderCell& RichHeaderCell::paddingBottom(int value) {
    mImpl->paddingBottom(value);
    return *this;
}
int RichHeaderCell::paddingLeft() const {
    return mImpl->paddingLeft();
}
RichHeaderCell& RichHeaderCell::paddingLeft(int value) {
    mImpl->paddingLeft(value);
    return *this;
}
RichHeaderCell& RichHeaderCell::cell(int row, int column) {
    mImpl->cell(row, column);
    return *this;
}
RichHeaderCell& RichHeaderCell::span(int rows, int columns) {
    mImpl->span(rows, columns);
    return *this;
}
RichHeaderCell& RichHeaderCell::padding(int all) {
    mImpl->padding(all);
    return *this;
}
RichHeaderCell& RichHeaderCell::padding(int vertical, int horizontal) {
    mImpl->padding(vertical, horizontal);
    return *this;
}
RichHeaderCell& RichHeaderCell::padding(int top, int right, int bottom, int left) {
    mImpl->padding(top, right, bottom, left);
    return *this;
}
RichHeaderCell& RichHeaderCell::show() {
    mImpl->show();
    return *this;
}
RichHeaderCell& RichHeaderCell::hide() {
    mImpl->hide();
    return *this;
}
RichHeaderCell& RichHeaderCell::incColumnSpan() {
    mImpl->incColumnSpan();
    return *this;
}
RichHeaderCell& RichHeaderCell::incRowSpan() {
    mImpl->incRowSpan();
    return *this;
}
RichHeaderCell& RichHeaderCell::moveUp() {
    mImpl->moveUp();
    return *this;
}
RichHeaderCell& RichHeaderCell::moveDown() {
    mImpl->moveDown();
    return *this;
}
RichHeaderCell& RichHeaderCell::moveLeft() {
    mImpl->moveLeft();
    return *this;
}
RichHeaderCell& RichHeaderCell::moveRight() {
    mImpl->moveRight();
    return *this;
}
