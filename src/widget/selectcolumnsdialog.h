#ifndef SELECTCOLUMNSDIALOG_H
#define SELECTCOLUMNSDIALOG_H

#include <QDialog>
#include <QVariant>

namespace Ui {
class SelectColumnsDialog;
}

class SelectColumnsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectColumnsDialog(QWidget *parent = nullptr);
    ~SelectColumnsDialog();

    void init(const QString& table, const QStringList& fields, const QList<QVariant::Type>& types);

    QStringList dataChecked() const;
    QStringList keysChecked() const;

private:
    Ui::SelectColumnsDialog *ui;
};

#endif // SELECTCOLUMNSDIALOG_H
