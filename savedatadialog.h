#ifndef SAVEDATADIALOG_H
#define SAVEDATADIALOG_H

#include <QDialog>

namespace Ui {
class SaveDataDialog;
}

class QSqlQueryModel;
class CheckableStringListModel;

#include "dataformat.h"
#include "outputtype.h"

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

    OutputType::Type output() const;

    CheckableStringListModel *dataModel() const;
    CheckableStringListModel *keysModel() const;
protected:
    void updateLabels();

public slots:

    void accept();

private slots:
    void on_allData_clicked();

    void on_noneData_clicked();

    void on_allKeys_clicked();

    void on_noneKeys_clicked();

    void on_statement_currentIndexChanged(int index);

    void on_table_textChanged(const QString &arg1);

private:
    Ui::SaveDataDialog *ui;
    QSqlQueryModel* mModel;
};

#endif // SAVEDATADIALOG_H
