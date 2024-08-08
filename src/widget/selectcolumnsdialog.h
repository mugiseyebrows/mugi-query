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

    void initUpdate(const QString& table, const QStringList& fields, const QList<QMetaType::Type>& types);

    QStringList dataChecked() const;
    QStringList keysChecked() const;
    QStringList checked() const;

    void initInsert(const QString &table, const QStringList &fields, const QList<QMetaType::Type> &types);
private:
    Ui::SelectColumnsDialog *ui;
};

#endif // SELECTCOLUMNSDIALOG_H
