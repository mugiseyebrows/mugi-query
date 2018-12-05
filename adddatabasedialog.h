#ifndef ADDDATABASEDIALOG_H
#define ADDDATABASEDIALOG_H

#include <QDialog>

namespace Ui {
class AddDatabaseDialog;
}

class AddDatabaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDatabaseDialog(bool showHistory, QWidget *parent = 0);
    ~AddDatabaseDialog();

    QString connectionName();
    QString driver();
    QString host();
    QString user();
    QString password();
    QString database();
    int port();


public slots:
    void accept();

private slots:
    void on_history_clicked();

private:
    Ui::AddDatabaseDialog *ui;
};

#endif // ADDDATABASEDIALOG_H
