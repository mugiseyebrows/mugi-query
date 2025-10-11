#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>

class ColorButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ColorButton(QWidget *parent = nullptr);

    QColor color() const;
    void setColor(const QColor& color);

protected:
    QColor mColor;

signals:

    void colorChanged(QColor);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // COLORBUTTON_H
