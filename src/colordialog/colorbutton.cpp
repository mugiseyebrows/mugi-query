#include "colorbutton.h"

#include "contrasting.h"
#include <QPainter>

#include "colordialog.h"

ColorButton::ColorButton(QWidget *parent)
    : QPushButton{parent}
{
    setFlat(true);
    setText("");

    connect(this, &ColorButton::clicked, [=](){
        ColorDialog dialog(this);
        dialog.setColor(color(), true, true, true);
        if (dialog.exec() != QDialog::Accepted) {
            return;
        }
        setColor(dialog.color());
    });

}

QColor ColorButton::color() const
{
    return mColor;
}

void ColorButton::setColor(const QColor &color)
{
    mColor = color;

    QPalette palette = this->palette();

#if 0
    for(int role = 0;role< QPalette::NColorRoles; role++) {
        QColor roleColor = palette.color((QPalette::ColorRole) role);
        qDebug() << "role" << role << "color" << roleColor.name();
    }
#endif

    QColor text = contrasting(color, Qt::white, Qt::black);

    palette.setColor(QPalette::Button, color);


    palette.setColor(QPalette::ButtonText, text);
    setPalette(palette);
    setText(color.name());

    emit colorChanged(color);
}


void ColorButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QPalette palette = this->palette();

    QColor buttonColor = palette.color(QPalette::Button);
    QColor buttonTextColor = palette.color(QPalette::ButtonText);

    QRect rect(QPoint(0,0), this->size());

    painter.setPen(Qt::NoPen);
    painter.setBrush(buttonColor);
    painter.drawRect(rect);

    painter.setPen(buttonTextColor);
    painter.drawText(rect, text(), QTextOption(Qt::AlignCenter));

}
