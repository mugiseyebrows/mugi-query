#ifndef DATAIMPORTMODEL_H
#define DATAIMPORTMODEL_H

#include <QObject>
#include <QStandardItemModel>

class DataImportModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit DataImportModel(const QVariant& headerSizeHint, QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

protected:

    QVariant mHeaderSizeHint;

signals:

public slots:
};

#endif // DATAIMPORTMODEL_H
