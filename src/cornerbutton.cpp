#include "cornerbutton.h"

#include <QPushButton>
#include <QResizeEvent>

CornerButton::CornerButton(const QString &text, QObject *parent)
    : QObject{parent}, mButton(new QPushButton(text))
{}

QPushButton *CornerButton::button() const
{
    return mButton;
}

void CornerButton::setWidget(QWidget *widget)
{
    mParent = widget;
    mButton->setParent(widget);
    widget->installEventFilter(this);
    mButton->show();
}

bool CornerButton::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == mParent && event->type() == QEvent::Resize) {
        QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
        auto size = resizeEvent->size();
        auto buttonSize = mButton->size();
        mButton->setGeometry(QRect(QPoint(size.width() - buttonSize.width(), 0), mButton->size()));
        //mButton->move(size.width() - mButton->width(), 0);
        //qDebug() << "event->type" << event->type();
    }
    return false;
}
