#ifndef SCHEMA2CHANGESET_H
#define SCHEMA2CHANGESET_H

#include <QObject>
#include <QAbstractTableModel>
#include <functional>

typedef std::function<void()> Schema2ChangeSetHandler;

class Schema2ChangeSetItem {
public:
    Schema2ChangeSetItem() {

    }
    Schema2ChangeSetItem(const QStringList& queries, const Schema2ChangeSetHandler& handler)
        : queries(queries), handler(handler){

    }

    QStringList queries;
    QStringList errors;
    Schema2ChangeSetHandler handler;

};

class Schema2ChangeSet : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit Schema2ChangeSet(QObject *parent = nullptr);

    void append(const QStringList& queries,
                const Schema2ChangeSetHandler& onSuccess) {
        mItems.append(Schema2ChangeSetItem(queries, onSuccess));
    }

    QList<Schema2ChangeSetItem> mItems;

    bool execute(const QString &connectionName);

    bool isEmpty() const;

signals:


    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    // QAbstractItemModel interface
public:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // SCHEMA2CHANGESET_H
