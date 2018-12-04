#ifndef SAVEDATADIALOG_H
#define SAVEDATADIALOG_H

#include <QDialog>

namespace Ui {
class SaveDataDialog;
}

class SaveDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveDataDialog(const QStringList& columns, QWidget *parent = 0);
    ~SaveDataDialog();

private slots:
    void on_allData_clicked();

    void on_noneData_clicked();

    void on_allKeys_clicked();

    void on_noneKeys_clicked();

private:
    Ui::SaveDataDialog *ui;
};

#endif // SAVEDATADIALOG_H
