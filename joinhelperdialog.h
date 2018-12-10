#ifndef JOINHELPERDIALOG_H
#define JOINHELPERDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class JoinHelperDialog;
}
class Tokens;

class JoinHelperDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JoinHelperDialog(const QString& connectionName, const Tokens& tokens, QWidget *parent = 0);
    ~JoinHelperDialog();

    QString filePath() const;

    void findPath();

private slots:

    void on_add_clicked();

    void on_remove_clicked();

public slots:
    void accept();

private:
    Ui::JoinHelperDialog *ui;
    QString mConnectionName;

};

#endif // JOINHELPERDIALOG_H
