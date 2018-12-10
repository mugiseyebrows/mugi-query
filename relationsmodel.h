#ifndef RELATIONSMODEL_H
#define RELATIONSMODEL_H

#include <QStandardItemModel>

class RelationsModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit RelationsModel(QObject *parent = nullptr);
    ~RelationsModel();

    void load(const QString& path);
    void save(const QString& path);
};

#endif // RELATIONSMODEL_H
