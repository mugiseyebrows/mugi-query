#ifndef REMOVEDATABASEDIALOG_H
#define REMOVEDATABASEDIALOG_H

#include <QDialog>

namespace Ui {
class RemoveDatabaseDialog;
}

class RemoveDatabaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveDatabaseDialog(const QString &name, QWidget *parent = 0);
    ~RemoveDatabaseDialog();

public slots:
    void accept();

private:
    Ui::RemoveDatabaseDialog *ui;
    QString mName;
};

#endif // REMOVEDATABASEDIALOG_H
