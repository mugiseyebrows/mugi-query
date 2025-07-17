#include "completer.h"

#include <QListView>
#include <QAbstractItemView>
#include <QApplication>
#include <QScreen>
#include <QScrollBar>
#include <QKeyEvent>

#include "completerprivate.h"
#include "completionmodel.h"
#include "completeritemdelegate.h"

Completer::Completer(QObject *parent)
    : QObject{parent}, d(new CompleterPrivate(this))
{

}

QCompleter::CompletionMode Completer::completionMode() const
{
    return d->mode;
}

QAbstractItemView *Completer::popup() const
{
    if (!d->popup && completionMode() != QCompleter::InlineCompletion) {
        QListView *listView = new QListView;
        listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listView->setSelectionBehavior(QAbstractItemView::SelectRows);
        listView->setSelectionMode(QAbstractItemView::SingleSelection);
        listView->setModelColumn(d->column);
        Completer *that = const_cast<Completer*>(this);
        that->setPopup(listView);
    }
    return d->popup;
}

void Completer::setPopup(QAbstractItemView *popup)
{
    Q_ASSERT(popup != nullptr);
    if (d->popup) {
        QObject::disconnect(d->popup->selectionModel(), nullptr, this, nullptr);
        QObject::disconnect(d->popup, nullptr, this, nullptr);
    }
    if (d->popup != popup)
        delete d->popup;
    if (popup->model() != d->proxy)
        popup->setModel(d->proxy);
     popup->hide();

    Qt::FocusPolicy origPolicy = Qt::NoFocus;
    if (d->widget)
        origPolicy = d->widget->focusPolicy();

    popup->setParent(nullptr);
    popup->setWindowFlag(Qt::Popup);
    popup->setFocusPolicy(Qt::NoFocus);
    if (d->widget)
        d->widget->setFocusPolicy(origPolicy);

    popup->setFocusProxy(d->widget);
    popup->installEventFilter(this);

    auto* delegate = new CompleterItemDelegate(popup);
    d->delegate = delegate;
    popup->setItemDelegate(delegate);

    if (QListView *listView = qobject_cast<QListView *>(popup)) {
        listView->setModelColumn(d->column);
    }

    QObject::connect(popup, SIGNAL(clicked(QModelIndex)),
                     this, SLOT(_q_complete(QModelIndex)));
    QObject::connect(this, SIGNAL(activated(QModelIndex)),
                     popup, SLOT(hide()));

    QObject::connect(popup->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                     this, SLOT(_q_completionSelected(QItemSelection)));
    d->popup = popup;
}

QString Completer::pathFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return QString();
    QAbstractItemModel *sourceModel = d->proxy->sourceModel();
    if (!sourceModel)
        return QString();
    return sourceModel->data(index, d->role).toString();
}

QString Completer::completionPrefix() const
{
    return d->prefix;
}


void Completer::setCompletionPrefix(const QString &prefix)
{
    d->prefix = prefix;
    d->proxy->filter(prefix);
    if (d->delegate) {
        d->delegate->setPrefix(prefix);
    } else {
        qDebug() << "delegate is null" << d->delegate;
    }
}

void Completer::setModel(QAbstractItemModel *model)
{
    QAbstractItemModel *oldModel = d->proxy->sourceModel();
    if (oldModel == model)
        return;
    d->proxy->setSourceModel(model);
    if (d->popup)
        setPopup(d->popup); // set the model and make new connections
    if (oldModel && oldModel->QObject::parent() == this)
        delete oldModel;
}

QAbstractItemModel *Completer::completionModel() const
{
    return d->proxy;
}

void Completer::_q_complete(QModelIndex index, bool highlighted_)
{
    QString completion;

    auto* proxy = d->proxy;
    auto prefix = d->prefix;
    int column = d->column;

    if (!index.isValid()) {
        completion = prefix;
        index = QModelIndex();
    } else {
        if (!(index.flags() & Qt::ItemIsEnabled))
            return;
        QModelIndex si = proxy->mapToSource(index);
        si = si.sibling(si.row(), column); // for clicked()
        completion = pathFromIndex(si);
    }

    if (highlighted_) {
        emit highlighted(index);
        emit highlightedString(completion);
    } else {
        emit activated(index);
        emit activatedString(completion);
    }
}

void Completer::complete(const QRect& rect)
{
    QModelIndex idx = d->proxy->currentIndex(false);
    d->hiddenBecauseNoMatch = false;
    if (d->mode == QCompleter::InlineCompletion) {
        if (idx.isValid())
            _q_complete(idx, true);
        return;
    }

    Q_ASSERT(d->widget);
    if ((d->mode == QCompleter::PopupCompletion && !idx.isValid())
        || (d->mode == QCompleter::UnfilteredPopupCompletion && d->proxy->rowCount() == 0)) {
        if (d->popup)
            d->popup->hide(); // no suggestion, hide
        d->hiddenBecauseNoMatch = true;
        return;
    }

    popup();
#if 0
    if (d->mode == QCompleter::UnfilteredPopupCompletion)
        d->setCurrentIndex(idx, false);
#endif

    d->showPopup(rect);
    d->popupRect = rect;
}

void Completer::_q_completionSelected(const QItemSelection& selection)
{
    QModelIndex index;
    if (!selection.indexes().isEmpty())
        index = selection.indexes().first();
    _q_complete(index, true);
}

bool Completer::eventFilter(QObject *o, QEvent *e)
{
    if (d->eatFocusOut && o == d->widget && e->type() == QEvent::FocusOut) {
        d->hiddenBecauseNoMatch = false;
        if (d->popup && d->popup->isVisible())
            return true;
    }

    if (o != d->popup)
        return QObject::eventFilter(o, e);

    switch (e->type()) {
    case QEvent::KeyPress: {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);

        QModelIndex curIndex = d->popup->currentIndex();
        QModelIndexList selList = d->popup->selectionModel()->selectedIndexes();

        const int key = ke->key();
        // In UnFilteredPopup mode, select the current item
        if ((key == Qt::Key_Up || key == Qt::Key_Down) && selList.isEmpty() && curIndex.isValid()
            && d->mode == QCompleter::UnfilteredPopupCompletion) {
              d->setCurrentIndex(curIndex);
              return true;
        }

        // Handle popup navigation keys. These are hardcoded because up/down might make the
        // widget do something else (lineedit cursor moves to home/end on mac, for instance)
        switch (key) {
        case Qt::Key_End:
        case Qt::Key_Home:
            if (ke->modifiers() & Qt::ControlModifier)
                return false;
            break;

        case Qt::Key_Up:
            if (!curIndex.isValid()) {
                int rowCount = d->proxy->rowCount();
                QModelIndex lastIndex = d->proxy->index(rowCount - 1, d->column);
                d->setCurrentIndex(lastIndex);
                return true;
            } else if (curIndex.row() == 0) {
                if (d->wrap)
                    d->setCurrentIndex(QModelIndex());
                return true;
            }
            return false;

        case Qt::Key_Down:
            if (!curIndex.isValid()) {
                QModelIndex firstIndex = d->proxy->index(0, d->column);
                d->setCurrentIndex(firstIndex);
                return true;
            } else if (curIndex.row() == d->proxy->rowCount() - 1) {
                if (d->wrap)
                    d->setCurrentIndex(QModelIndex());
                return true;
            }
            return false;

        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            return false;
        }

        // Send the event to the widget. If the widget accepted the event, do nothing
        // If the widget did not accept the event, provide a default implementation
        d->eatFocusOut = false;
        (static_cast<QObject *>(d->widget))->event(ke);
        d->eatFocusOut = true;
        if (!d->widget || e->isAccepted() || !d->popup->isVisible()) {
            // widget lost focus, hide the popup
            if (d->widget && (!d->widget->hasFocus()
#ifdef QT_KEYPAD_NAVIGATION
                || (QApplicationPrivate::keypadNavigationEnabled() && !d->widget->hasEditFocus())
#endif
                ))
                d->popup->hide();
            if (e->isAccepted())
                return true;
        }

        // default implementation for keys not handled by the widget when popup is open
#if QT_CONFIG(shortcut)
        if (ke->matches(QKeySequence::Cancel)) {
            d->popup->hide();
            return true;
        }
#endif
        switch (key) {
#ifdef QT_KEYPAD_NAVIGATION
        case Qt::Key_Select:
            if (!QApplicationPrivate::keypadNavigationEnabled())
                break;
#endif
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Tab:
            d->popup->hide();
            if (curIndex.isValid())
                _q_complete(curIndex);
            break;

        case Qt::Key_F4:
            if (ke->modifiers() & Qt::AltModifier)
                d->popup->hide();
            break;

        case Qt::Key_Backtab:
            d->popup->hide();
            break;

        default:
            break;
        }

        return true;
    }

#ifdef QT_KEYPAD_NAVIGATION
    case QEvent::KeyRelease: {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        if (QApplicationPrivate::keypadNavigationEnabled() && ke->key() == Qt::Key_Back) {
            // Send the event to the 'widget'. This is what we did for KeyPress, so we need
            // to do the same for KeyRelease, in case the widget's KeyPress event set
            // up something (such as a timer) that is relying on also receiving the
            // key release. I see this as a bug in Qt, and should really set it up for all
            // the affected keys. However, it is difficult to tell how this will affect
            // existing code, and I can't test for every combination!
            d->eatFocusOut = false;
            static_cast<QObject *>(d->widget)->event(ke);
            d->eatFocusOut = true;
        }
        break;
    }
#endif

    case QEvent::MouseButtonPress: {
#ifdef QT_KEYPAD_NAVIGATION
        if (QApplicationPrivate::keypadNavigationEnabled()) {
            // if we've clicked in the widget (or its descendant), let it handle the click
            QWidget *source = qobject_cast<QWidget *>(o);
            if (source) {
                QPoint pos = source->mapToGlobal((static_cast<QMouseEvent *>(e))->pos());
                QWidget *target = QApplication::widgetAt(pos);
                if (target && (d->widget->isAncestorOf(target) ||
                    target == d->widget)) {
                    d->eatFocusOut = false;
                    static_cast<QObject *>(target)->event(e);
                    d->eatFocusOut = true;
                    return true;
                }
            }
        }
#endif
        if (!d->popup->underMouse()) {
            d->popup->hide();
            return true;
        }
        }
        return false;

    case QEvent::InputMethod:
    case QEvent::ShortcutOverride:
        QCoreApplication::sendEvent(d->widget, e);
        break;

    default:
        return false;
    }
    return false;
}

void Completer::setWidget(QWidget *widget)
{
    if (widget == d->widget)
        return;

    if (d->widget)
        d->widget->removeEventFilter(this);
    d->widget = widget;
    if (d->widget)
        d->widget->installEventFilter(this);

    if (d->popup) {
        d->popup->hide();
        d->popup->setFocusProxy(d->widget);
    }
}

QWidget *Completer::widget() const
{
    return d->widget;
}
