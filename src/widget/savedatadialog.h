#ifndef SAVEDATADIALOG_H
#define SAVEDATADIALOG_H

#include <QDialog>

namespace Ui {
class SaveDataDialog;
}

class QSqlQueryModel;
class DataImportColumnModel;

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

    DataImportColumnModel *dataModel() const;
    DataImportColumnModel *keysModel() const;


public slots:

    void accept();

private slots:

    void on_table_textChanged(const QString &arg1);

    void on_format_currentIndexChanged(int index);

private:
    Ui::SaveDataDialog *ui;
    QSqlQueryModel* mModel;
};

#endif // SAVEDATADIALOG_H
