#ifndef DATAIMPORTMODEL_H
#define DATAIMPORTMODEL_H

#include <QObject>
#include <QStandardItemModel>
#include <QMap>
#include <QLocale>

class DataImportModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit DataImportModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void setTypes(const QMap<int, QMetaType::Type>& types, const QMap<int,int>& sizes);

    void setLocale(const QLocale& locale);

protected:

    QMap<int, QMetaType::Type> mTypes;
    QMap<int, int> mSizes;

    QLocale mLocale;

signals:

public slots:
};

#endif // DATAIMPORTMODEL_H
