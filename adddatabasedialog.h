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
    explicit AddDatabaseDialog(QWidget *parent = 0);
    ~AddDatabaseDialog();

    QString connectionName();
public slots:
    void accept();

private:
    Ui::AddDatabaseDialog *ui;
};

#endif // ADDDATABASEDIALOG_H
