#ifndef CHECKABLEMODEL_H
#define CHECKABLEMODEL_H

#include <QStandardItemModel>

class CheckableModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit CheckableModel(QObject *parent = nullptr, const QStringList& items = {});

    void append(const QStringList& items);

    void setChecked(Qt::CheckState state);
    QStringList checked() const;
public slots:
    void checkAll();
    void uncheckAll();
signals:

};

#endif // CHECKABLEMODEL_H
