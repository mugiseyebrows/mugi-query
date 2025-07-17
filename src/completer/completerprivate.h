#ifndef COMPLETERPRIVATE_H
#define COMPLETERPRIVATE_H

class Completer;
class QAbstractItemView;
class QWidget;
class CompletionModel;
#include <QCompleter>
#include <QPointer>
class CompleterItemDelegate;

class CompleterPrivate {
public:
    CompleterPrivate(Completer* completer);
    void showPopup(const QRect &rect);

    QAbstractItemView *popup = nullptr;
    QPointer<QWidget> widget;
    QCompleter::CompletionMode mode = QCompleter::PopupCompletion;
    int column = 0;
    CompletionModel *proxy;
    int role = Qt::EditRole;
    QString prefix;
    int maxVisibleItems = 7;
    bool hiddenBecauseNoMatch = false;
    Completer* q = nullptr;
    QRect popupRect;
    bool eatFocusOut = true;
    bool wrap = true;
    CompleterItemDelegate* delegate = nullptr;

    void setCurrentIndex(QModelIndex index, bool select = true);
};


#endif // COMPLETERPRIVATE_H
