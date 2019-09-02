#include "richheadertabfilter.h"
#include <QKeyEvent>
#include <QWidget>
#include <QDebug>

RichHeaderTabFilter::RichHeaderTabFilter(QObject *parent) : QObject(parent)
{

}

bool RichHeaderTabFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab) {
            mWidget = qobject_cast<QWidget*>(watched);
            emit tabPressed(keyEvent->key() == Qt::Key_Backtab);
            return true;
        }
    }
    return false;
}

void RichHeaderTabFilter::watch(QWidget *widget)
{
    if (mWatched.contains(widget)) {
        return;
    }
    mWatched << widget;
    widget->installEventFilter(this);
}

QWidget *RichHeaderTabFilter::widget() const
{
    return mWidget;
}
