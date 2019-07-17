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
    explicit DataImportModel(const QVariant& headerSizeHint, QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void setTypes(const QMap<int, QVariant::Type>& types);

    void setLocale(const QLocale& locale);

protected:

    QVariant mHeaderSizeHint;

    QMap<int, QVariant::Type> mTypes;

    QLocale mLocale;

signals:

public slots:
};

#endif // DATAIMPORTMODEL_H
