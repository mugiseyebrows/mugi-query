#ifndef TABLESTRETCHER_H
#define TABLESTRETCHER_H

#include <QObject>

class QTableView;
class QHeaderView;

class TableStretcher : public QObject
{
    Q_OBJECT
public:
    explicit TableStretcher(QObject *parent = nullptr);

    void setView(QTableView* view);

    void setRatio(const QList<double> ratio);

protected:
    QTableView* mView;
    QList<double> mRatio;
    QHeaderView* mHeaderView;

signals:


    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // TABLESTRETCHER_H
