#ifndef Schema2RelationsListDialog_H
#define Schema2RelationsListDialog_H

#include <QDialog>

namespace Ui {
class Schema2RelationsListDialog;
}

#include <QHash>
class Schema2RelationModel;
class Schema2RelationsFilterModel;
class Schema2RelationsListModel;
class Schema2TableModel;
#include "hash.h"

class Schema2RelationsListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit Schema2RelationsListDialog(QWidget *parent = nullptr);
    ~Schema2RelationsListDialog();

    QString childTable() const;

    QString parentTable() const;

    bool remove() const;


    void init(const StringHash<Schema2TableModel *> &tableModels,
              const StringListHash<Schema2RelationModel *> &relationModels);

private slots:
    void on_table_doubleClicked(const QModelIndex &index);

    void on_childTable_textChanged(QString);

    void on_parentTable_textChanged(QString);

private:
    Ui::Schema2RelationsListDialog *ui;
    Schema2RelationsFilterModel* mProxyModel;
    Schema2RelationsListModel* mModel;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // Schema2RelationsListDialog_H
