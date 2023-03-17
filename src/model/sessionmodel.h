
#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class SessionItem;

class SessionModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    SessionModel(QObject *parent = 0);
    ~SessionModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    void addDatabase(const QString& name);
    void addSession(const QModelIndex& parent);
    void removeDatabase(const QModelIndex& index);
    void removeSession(const QModelIndex& index);

    bool isSession(const QModelIndex& index);
    bool isDatabase(const QModelIndex& index);

    QString connectionName(const QModelIndex& index);
    QString sessionName(const QModelIndex& index);

    QModelIndex indexOf(const QString& connectionName, const QString& name) const;

    QModelIndex indexOf(const QString &connectionName) const;

    QStringList connectionNames() const;
signals:

    //void databaseAdded(QString,QString);
    void sessionAdded(QString,QString,QString);
    //void databaseRemoved(QString);
    void sessionRemoved(QString,QString);

protected:

    SessionItem *rootItem;

    QString mName;

};

#endif
