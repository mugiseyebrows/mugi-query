#ifndef ADDDATABASEDIALOG_H
#define ADDDATABASEDIALOG_H

#include <QDialog>

namespace Ui {
class DatabaseConnectDialog;
}

class DatabaseConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseConnectDialog(bool showHistory, QWidget *parent = 0);
    ~DatabaseConnectDialog();

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

    void on_savePassword_clicked(bool checked);

private:
    Ui::DatabaseConnectDialog *ui;
};

#endif // ADDDATABASEDIALOG_H
