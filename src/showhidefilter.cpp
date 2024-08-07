#include "showhidefilter.h"
#include <QEvent>

ShowHideFilter::ShowHideFilter(QObject *parent)
    : QObject{parent}
{}

bool ShowHideFilter::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
    case QEvent::Show: emit shown(); break;
    case QEvent::Hide: emit hidden(); break;
    default:
        break;
    }
    return false;
}
