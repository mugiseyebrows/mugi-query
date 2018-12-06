#ifndef COPYEVENTFILTER_H
#define COPYEVENTFILTER_H

#include <QObject>

class QTableView;
class QTextStream;
class QItemSelectionRange;
class QAbstractItemModel;
class QItemSelection;

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
    
    static void streamRange(QTextStream &stream, const QItemSelectionRange& rng);

    static void copyAll(QAbstractItemModel *model);

    static void copySelected(QAbstractItemModel *model, const QItemSelection& selection);
protected:

    bool eventFilter(QObject* object, QEvent *event);
    QTableView* mView;

signals:
    
public slots:


};

#endif // COPYEVENTFILTER_H
