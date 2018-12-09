#ifndef USERHELPERDIALOG_H
#define USERHELPERDIALOG_H

#include <QDialog>
class Tokens;

namespace Ui {
class UserHelperDialog;
}

class UserHelperDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserHelperDialog(const QString& database, const Tokens& tokens, QWidget *parent = 0);
    ~UserHelperDialog();

private:
    Ui::UserHelperDialog *ui;
};

#endif // USERHELPERDIALOG_H
