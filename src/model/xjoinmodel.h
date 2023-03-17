#ifndef XJOINMODEL_H
#define XJOINMODEL_H

#include <QObject>
#include <QAbstractTableModel>

class QSqlQueryModel;

class XJoinModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit XJoinModel(QSqlQueryModel* model1, const QStringList& columns1,
                        QSqlQueryModel* model2, const QStringList& columns2,
                        QObject *parent = nullptr);

protected:
    QSqlQueryModel *mModel1;
    QSqlQueryModel *mModel2;
    QList<QList<int>> mMatched;

signals:


    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // XJOINMODEL_H
