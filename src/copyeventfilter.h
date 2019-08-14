#ifndef COPYEVENTFILTER_H
#define COPYEVENTFILTER_H

#include <QObject>

class QTableView;
class QTextStream;
class QItemSelectionRange;
class QAbstractItemModel;
class QItemSelection;

#include "dataformat.h"

class CopyEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit CopyEventFilter(QObject *parent = 0);

    /**
     * @brief Installs event filter for view
     * @param view
     */
    void setView(QTableView* view);
    
protected:

    bool eventFilter(QObject* object, QEvent *event);
    QTableView* mView;

signals:
    void copy();
    void paste();
    void delete_();
    void cut();
public slots:
    void onDeleteSelected();

};

#endif // COPYEVENTFILTER_H
