#include "completionmodel.h"


QModelIndex CompletionModel::currentIndex(bool sourceIndex) const
{
    int row = curRow;
    QModelIndex idx = index(row, 0);
    if (!sourceIndex)
        return idx;
    return mapToSource(idx);
}

void CompletionModel::filter(const QString &prefix)
{
    this->prefix = prefix;
    invalidateFilter();
    curRow = 0;
}

static bool contains(const QString& s, const QString& ss, bool starts = false) {
    int p = -1;
    if (ss.isEmpty()) {
        return true;
    }
    if (s.isEmpty()) {
        return false;
    }
    if (starts) {
        if (s[0] != ss[0]) {
            return false;
        }
    }
    for(int i=0;i<ss.size();i++) {
        p = s.indexOf(ss[i], p + 1, Qt::CaseInsensitive);
        if (p < 0) {
            return false;
        }
    }
    return true;
}

bool CompletionModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (prefix.isEmpty()) {
        return true;
    }
    QAbstractItemModel* m = sourceModel();
    QString text = m->data(m->index(source_row, 0, source_parent)).toString();
    return contains(text, prefix, true);
}

