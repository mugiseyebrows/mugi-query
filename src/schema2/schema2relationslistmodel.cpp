#include "schema2relationslistmodel.h"


Schema2RelationsListModel::Schema2RelationsListModel(const StringListHash<Schema2RelationModel *> &relationModels,
                                                     QObject *parent)
{

    QList<QStringList> items = relationModels.keys();
    setRowCount(items.size());
    setColumnCount(2);
    for(int row=0;row<items.size();row++) {
        setData(index(row, 0), items[row][0]);
        setData(index(row, 1), items[row][1]);
    }
}
