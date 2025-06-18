#ifndef DICTKEYDIALOG_H
#define DICTKEYDIALOG_H

#include <QDialog>

namespace Ui {
class DictKeyDialog;
}

class QAbstractItemModel;

class DictKeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DictKeyDialog(QAbstractItemModel* model, QWidget *parent = nullptr);
    ~DictKeyDialog();

    QStringList key() const;

private:
    Ui::DictKeyDialog *ui;
};

#endif // DICTKEYDIALOG_H
