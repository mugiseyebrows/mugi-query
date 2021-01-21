#ifndef DATACOMPAREMODEL_H
#define DATACOMPAREMODEL_H

#include <QObject>
#include <QAbstractTableModel>

class DataCompareModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Mode {
        Short,
        Full,
        Before,
        After
    };

    explicit DataCompareModel(QObject *parent = nullptr);

    void setModels(QAbstractItemModel* model1, QAbstractItemModel* model2);

    void setKeyColumns(const QList<int>& columns);

    void update();

    void setMode(Mode mode);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

protected:
    QList<int> mKeyColumns;
    QList<QAbstractItemModel*> mModels;

    QList<QVariantList> mKeys;
    QList<QMap<int,int>> mRows;

    int mColumnCount;

    Mode mMode;


signals:

public slots:

public:

    QVariantList rowKey(QAbstractItemModel *model, int row) const;
    int findRow(QAbstractItemModel *model, const QVariantList &key) const;
    QVariant valueAt(int row, int column) const;
    bool isRemovedRow(int row) const;
    bool isInsertedRow(int row) const;
    QVariantList valuesAt(int row_, int column) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void setKeys(const QList<QVariantList> &keys, const QList<QMap<int, int> > &rows);
    bool testKey(QAbstractItemModel *model, const QVariantList &key) const;
    bool modelContainsRow(int model, int row) const;
};

#endif // DATACOMPAREMODEL_H
