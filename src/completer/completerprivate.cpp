#include "completerprivate.h"

#include "completer.h"
#include "completionmodel.h"
#include <QScrollBar>
#include <QAbstractItemView>
#include <QApplication>
#include <QScreen>

CompleterPrivate::CompleterPrivate(Completer *completer) : proxy(new CompletionModel{}), q(completer) {

}


void CompleterPrivate::showPopup(const QRect& rect)
{
    // todo widget to screen
    const QRect screen = qApp->primaryScreen()->availableGeometry();
    Qt::LayoutDirection dir = widget->layoutDirection();
    QPoint pos;
    int rh, w;
    int h = (popup->sizeHintForRow(0) * qMin(maxVisibleItems, popup->model()->rowCount()) + 3) + 3;
    QScrollBar *hsb = popup->horizontalScrollBar();
    if (hsb && hsb->isVisible())
        h += popup->horizontalScrollBar()->sizeHint().height();

    if (rect.isValid()) {
        rh = rect.height();
        w = rect.width();
        pos = widget->mapToGlobal(dir == Qt::RightToLeft ? rect.bottomRight() : rect.bottomLeft());
    } else {
        rh = widget->height();
        pos = widget->mapToGlobal(QPoint(0, widget->height() - 2));
        w = widget->width();
    }

    if (w > screen.width())
        w = screen.width();
    if ((pos.x() + w) > (screen.x() + screen.width()))
        pos.setX(screen.x() + screen.width() - w);
    if (pos.x() < screen.x())
        pos.setX(screen.x());

    int top = pos.y() - rh - screen.top() + 2;
    int bottom = screen.bottom() - pos.y();
    h = qMax(h, popup->minimumHeight());
    if (h > bottom) {
        h = qMin(qMax(top, bottom), h);

        if (top > bottom)
            pos.setY(pos.y() - h - rh + 2);
    }

    popup->setGeometry(pos.x(), pos.y(), w, h);

    if (!popup->isVisible())
        popup->show();
}

void CompleterPrivate::setCurrentIndex(QModelIndex index, bool select)
{
    if (!q->popup())
        return;
    if (!select) {
        popup->selectionModel()->setCurrentIndex(index, QItemSelectionModel::NoUpdate);
    } else {
        if (!index.isValid())
            popup->selectionModel()->clear();
        else
            popup->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select
                                                            | QItemSelectionModel::Rows);
    }
    index = popup->selectionModel()->currentIndex();
    if (!index.isValid())
        popup->scrollToTop();
    else
        popup->scrollTo(index, QAbstractItemView::PositionAtTop);
}
