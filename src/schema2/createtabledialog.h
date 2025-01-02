#ifndef CREATETABLEDIALOG_H
#define CREATETABLEDIALOG_H

#include <QDialog>
class Schema2Data;
class SName;

namespace Ui {
class CreateTableDialog;
}

class CreateTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateTableDialog(Schema2Data *data, QWidget *parent = nullptr);
    ~CreateTableDialog();

    SName name() const;

private:
    Ui::CreateTableDialog *ui;
};

#endif // CREATETABLEDIALOG_H
