#include "schema2changeset.h"
#include "history.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

Schema2ChangeSet::Schema2ChangeSet(QObject *parent)
    : QAbstractTableModel{parent}
{

}

void Schema2ChangeSet::append(const QStringList &queries, const Schema2ChangeSetHandler &onSuccess) {

    if (queries.isEmpty()) {
        return;
    }

    auto item = Schema2ChangeSetItem(queries, onSuccess);
    for(int i=0;i<item.size();i++) {
        mIndexes.append({mItems.size(), i});
    }
    mItems.append(item);
}

bool Schema2ChangeSet::execute(const QString& connectionName)
{
    History* history = History::instance();
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    bool ok = true;
    for(int row=0;row<mItems.size();row++) {
        QSqlQuery q(db);
        QStringList errors;
        bool hasErrors = false;
        for(const QString& query: std::as_const(mItems[row].queries)) {
            QString error;
            if (!q.exec(query)) {
                error = q.lastError().text();
                hasErrors = true;
            } else {
                history->addQuery(connectionName, query);
            }
            errors.append(error);
        }
        if (!hasErrors) {
            mItems[row].handler();
        }
        mItems[row].errors = errors;
    }
    return ok;
}

bool Schema2ChangeSet::isEmpty() const
{
    return mItems.isEmpty();
}

int Schema2ChangeSet::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mIndexes.size();
}

int Schema2ChangeSet::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return 2;
}

QVariant Schema2ChangeSet::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QList<int> ix = mIndexes[index.row()];
        switch(index.column()) {
        case 0: return mItems[ix[0]].queries[ix[1]];
        case 1: return mItems[ix[0]].errors[ix[1]];
        }
    }
    return QVariant();
}

QVariant Schema2ChangeSet::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QStringList header = {"Query", "Error"};
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return header.value(section);
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
