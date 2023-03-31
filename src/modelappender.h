#ifndef MODELAPPENDER_H
#define MODELAPPENDER_H

#include <QObject>
#include <QAbstractItemModel>

class ModelAppender : public QObject
{
    Q_OBJECT
public:
    explicit ModelAppender(QObject *parent = nullptr);

    void setModel(QAbstractItemModel* model);

    void setActive(bool active);

    static ModelAppender* attach(QAbstractItemModel* model);

protected:
    bool mActive;

signals:
    void rowInserted(int);
private slots:
    void onDataChanged(QModelIndex, QModelIndex, QVector<int>);
};

#endif // MODELAPPENDER_H
