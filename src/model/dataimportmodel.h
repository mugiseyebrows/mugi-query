#ifndef DATAIMPORTMODEL_H
#define DATAIMPORTMODEL_H

#include <QObject>
#include <QStandardItemModel>
#include <QMap>
#include <QLocale>
class QSqlRecord;

class DataImportModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit DataImportModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QVariant parsed(int row, int column);

    void setTypes(const QMap<int, QMetaType::Type>& types, const QMap<int,int>& sizes);

    void setLocale(const QLocale& locale);

    void setType(int column, QMetaType::Type type);

    QSqlRecord record(int row, bool* ok);

protected:

    QMap<int, QMetaType::Type> mTypes;
    QMap<int, int> mSizes;

    QLocale mLocale;

signals:

public slots:
};

#endif // DATAIMPORTMODEL_H
