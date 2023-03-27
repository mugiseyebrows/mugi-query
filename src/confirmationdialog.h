#ifndef CONFIRMATIONDIALOG_H
#define CONFIRMATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ConfirmationDialog;
}

class ConfirmationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfirmationDialog(QWidget *parent = nullptr);
    ~ConfirmationDialog();

    void setMessage(const QString& message);

    static bool question(QWidget* parent, const QString& message, bool* dontAsk);

    bool dontAskAgain() const;
private:
    Ui::ConfirmationDialog *ui;
};

#endif // CONFIRMATIONDIALOG_H
