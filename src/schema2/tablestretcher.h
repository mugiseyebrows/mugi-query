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

    void setRatio(const QList<double>& ratio);

    void setPadding(int value);

    static TableStretcher* stretch(QTableView* view, const QList<double>& ratio, int padding = 0);

protected:
    QTableView* mView;
    QList<double> mRatio;
    QHeaderView* mHeaderView;
    int mPadding;

signals:


    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // TABLESTRETCHER_H
