#ifndef DATATRUNCATEDIALOG_H
#define DATATRUNCATEDIALOG_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class DataTruncateDialog;
}

class DataTruncateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataTruncateDialog(QSqlDatabase db, QWidget *parent = nullptr);
    ~DataTruncateDialog();

    QStringList tables() const;

private:
    Ui::DataTruncateDialog *ui;
};

#endif // DATATRUNCATEDIALOG_H
