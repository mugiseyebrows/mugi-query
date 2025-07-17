#ifndef COMPLETIONMODEL_H
#define COMPLETIONMODEL_H

#include <QSortFilterProxyModel>

class CompletionModel: public QSortFilterProxyModel {
    Q_OBJECT
public:
    void filter(const QString& prefix);
    QModelIndex currentIndex(bool sourceIndex) const;
protected:
    QString prefix;
    int curRow = -1;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};


#endif // COMPLETIONMODEL_H
