#ifndef COMPLETER_H
#define COMPLETER_H

#include <QObject>
#include <QCompleter>
#include <QPointer>
#include <QItemSelection>
#include <QItemDelegate>
#include <QSortFilterProxyModel>

class Completer;
class QAbstractItemView;
class QCompletionModel;
class CompleterPrivate;

class Completer : public QObject
{
    Q_OBJECT
public:
    explicit Completer(QObject *parent = nullptr);

    void setCompletionPrefix(const QString& prefix);

    void setModel(QAbstractItemModel* model);

    QAbstractItemView *popup() const;

    void setPopup(QAbstractItemView *popup);
    QCompleter::CompletionMode completionMode() const;
    QString pathFromIndex(const QModelIndex &index) const;

    QString completionPrefix() const;

    QAbstractItemModel* completionModel() const;

    void complete(const QRect &rect);
    void setWidget(QWidget *widget);

    QWidget *widget() const;

protected:
    CompleterPrivate *d;

    bool eventFilter(QObject *o, QEvent *e);
protected slots:
    void _q_completionSelected(const QItemSelection &selection);
    void _q_complete(QModelIndex, bool highlighted_ = false);
signals:
    void activatedString(const QString &text);
    void activated(const QModelIndex &index);
    void highlightedString(const QString &text);
    void highlighted(const QModelIndex &index);

};

#endif // COMPLETER_H
