#ifndef RICHHEADERCELLIMPL_H
#define RICHHEADERCELLIMPL_H
#include <QDebug>
#include <QString>

class RichHeaderCellImpl {
public:
    RichHeaderCellImpl();
    RichHeaderCellImpl(int row = -1, int column = -1, int rowSpan = 1, int columnSpan = 1,
                       const QString& text = QString(), bool multiline = false,
                       Qt::TextElideMode elide = Qt::ElideNone,
                       Qt::Alignment align = Qt::AlignCenter, double rotation = 0.0,
                       QWidget* widget = 0, bool visible = true, int paddingTop = 0,
                       int paddingRight = 0, int paddingBottom = 0, int paddingLeft = 0);
    void padding(int all);
    void padding(int vertical, int horizontal);
    void padding(int top, int right, int bottom, int left);
    int row() const;
    void row(int value);
    int column() const;
    void column(int value);
    int rowSpan() const;
    void rowSpan(int value);
    int columnSpan() const;
    void columnSpan(int value);
    QString text() const;
    void text(const QString& value);
    bool multiline() const;
    void multiline(bool value);
    Qt::TextElideMode elide() const;
    void elide(Qt::TextElideMode value);
    Qt::Alignment align() const;
    void align(Qt::Alignment value);
    double rotation() const;
    void rotation(double value);
    QWidget* widget() const;
    void widget(QWidget* value);
    bool visible() const;
    void visible(bool value);
    int paddingTop() const;
    void paddingTop(int value);
    int paddingRight() const;
    void paddingRight(int value);
    int paddingBottom() const;
    void paddingBottom(int value);
    int paddingLeft() const;
    void paddingLeft(int value);
    void show();
    void hide();
    void incColumnSpan();
    void incRowSpan();
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

protected:
    int mRow;
    int mColumn;
    int mRowSpan;
    int mColumnSpan;
    QString mText;
    bool mMultiline;
    Qt::TextElideMode mElide;
    Qt::Alignment mAlign;
    double mRotation;
    QWidget* mWidget;
    bool mVisible;
    int mPaddingTop;
    int mPaddingRight;
    int mPaddingBottom;
    int mPaddingLeft;
};
QDebug operator<<(QDebug debug, const RichHeaderCellImpl& cell);
#endif // RICHHEADERCELLIMPL_H
