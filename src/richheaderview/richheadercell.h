#ifndef RICHHEADERCELL_H
#define RICHHEADERCELL_H
#include "richheadercellimpl.h"
#include <QString>

class RichHeaderCell {
public:
    RichHeaderCell(RichHeaderCellImpl* cell);
    RichHeaderCell& cell(int row, int column);
    RichHeaderCell& span(int rows, int columns);
    RichHeaderCell& padding(int all);
    RichHeaderCell& padding(int vertical, int horizontal);
    RichHeaderCell& padding(int top, int right, int bottom, int left);
    RichHeaderCell& row(int value);
    int row() const;
    RichHeaderCell& column(int value);
    int column() const;
    RichHeaderCell& rowSpan(int value);
    int rowSpan() const;
    RichHeaderCell& columnSpan(int value);
    int columnSpan() const;
    RichHeaderCell& text(const QString& value);
    QString text() const;
    RichHeaderCell& multiline(bool value);
    bool multiline() const;
    RichHeaderCell& elide(Qt::TextElideMode value);
    Qt::TextElideMode elide() const;
    RichHeaderCell& align(Qt::Alignment value);
    Qt::Alignment align() const;
    RichHeaderCell& rotation(double value);
    double rotation() const;
    RichHeaderCell& widget(QWidget* value);
    QWidget* widget() const;
    RichHeaderCell& visible(bool value);
    bool visible() const;
    RichHeaderCell& paddingTop(int value);
    int paddingTop() const;
    RichHeaderCell& paddingRight(int value);
    int paddingRight() const;
    RichHeaderCell& paddingBottom(int value);
    int paddingBottom() const;
    RichHeaderCell& paddingLeft(int value);
    int paddingLeft() const;
    RichHeaderCell& show();
    RichHeaderCell& hide();
    RichHeaderCell& incColumnSpan();
    RichHeaderCell& incRowSpan();
    RichHeaderCell& moveUp();
    RichHeaderCell& moveDown();
    RichHeaderCell& moveLeft();
    RichHeaderCell& moveRight();

protected:
    RichHeaderCellImpl* mCell;
};
#endif // RICHHEADERCELL_H
