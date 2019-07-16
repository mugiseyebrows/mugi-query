#include "richheadercell.h"

RichHeaderCell::RichHeaderCell(RichHeaderCellImpl* cell) : mCell(cell) {
}

RichHeaderCell& RichHeaderCell::cell(int row, int column) {
    mCell->row(row);
    mCell->column(column);
    return *this;
}

RichHeaderCell& RichHeaderCell::span(int rows, int columns) {
    mCell->rowSpan(rows);
    mCell->columnSpan(columns);
    return *this;
}

RichHeaderCell& RichHeaderCell::row(int value) {
    mCell->row(value);
    return *this;
}

int RichHeaderCell::row() const {
    return mCell->row();
}

RichHeaderCell& RichHeaderCell::column(int value) {
    mCell->column(value);
    return *this;
}

int RichHeaderCell::column() const {
    return mCell->column();
}

RichHeaderCell& RichHeaderCell::rowSpan(int value) {
    mCell->rowSpan(value);
    return *this;
}

int RichHeaderCell::rowSpan() const {
    return mCell->rowSpan();
}

RichHeaderCell& RichHeaderCell::columnSpan(int value) {
    mCell->columnSpan(value);
    return *this;
}

int RichHeaderCell::columnSpan() const {
    return mCell->columnSpan();
}

RichHeaderCell& RichHeaderCell::text(const QString& value) {
    mCell->text(value);
    return *this;
}

QString RichHeaderCell::text() const {
    return mCell->text();
}

RichHeaderCell& RichHeaderCell::multiline(bool value) {
    mCell->multiline(value);
    return *this;
}

bool RichHeaderCell::multiline() const {
    return mCell->multiline();
}

RichHeaderCell& RichHeaderCell::elide(Qt::TextElideMode value) {
    mCell->elide(value);
    return *this;
}

Qt::TextElideMode RichHeaderCell::elide() const {
    return mCell->elide();
}

RichHeaderCell& RichHeaderCell::align(Qt::Alignment value) {
    mCell->align(value);
    return *this;
}

Qt::Alignment RichHeaderCell::align() const {
    return mCell->align();
}

RichHeaderCell& RichHeaderCell::rotation(double value) {
    mCell->rotation(value);
    return *this;
}

double RichHeaderCell::rotation() const {
    return mCell->rotation();
}

RichHeaderCell& RichHeaderCell::widget(QWidget* value) {
    mCell->widget(value);
    return *this;
}

QWidget* RichHeaderCell::widget() const {
    return mCell->widget();
}

RichHeaderCell& RichHeaderCell::incColumnSpan() {
    mCell->incColumnSpan();
    return *this;
}

RichHeaderCell& RichHeaderCell::incRowSpan() {
    mCell->incRowSpan();
    return *this;
}

RichHeaderCell& RichHeaderCell::moveUp() {
    mCell->moveUp();
    return *this;
}

RichHeaderCell& RichHeaderCell::moveDown() {
    mCell->moveDown();
    return *this;
}

RichHeaderCell& RichHeaderCell::moveLeft() {
    mCell->moveLeft();
    return *this;
}

RichHeaderCell& RichHeaderCell::moveRight() {
    mCell->moveRight();
    return *this;
}
