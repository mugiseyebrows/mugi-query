#ifndef DELETEEVENTFILTER_H
#define DELETEEVENTFILTER_H

#include <QObject>
#include <QTableView>

class QTableView;

class DeleteEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit DeleteEventFilter(QObject *parent = nullptr);

    bool eventFilter(QObject *watched, QEvent *event) override;

    void setView(QTableView* view);

protected:
    QTableView* mView;

signals:

public slots:
};

#endif // DELETEEVENTFILTER_H
