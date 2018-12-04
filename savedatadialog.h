#ifndef SAVEDATADIALOG_H
#define SAVEDATADIALOG_H

#include <QDialog>

namespace Ui {
class SaveDataDialog;
}

class QSqlQueryModel;

#include "dataformat.h"

class SaveDataDialog : public QDialog
{
    Q_OBJECT

public:

    explicit SaveDataDialog(QSqlQueryModel *model, QWidget *parent = 0);
    ~SaveDataDialog();

    DataFormat::Format format() const;

    QList<bool> keysChecked() const;
    QList<bool> dataChecked() const;

    QString table() const;

private slots:
    void on_allData_clicked();

    void on_noneData_clicked();

    void on_allKeys_clicked();

    void on_noneKeys_clicked();

private:
    Ui::SaveDataDialog *ui;
    QSqlQueryModel* mModel;
};

#endif // SAVEDATADIALOG_H
