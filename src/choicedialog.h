#ifndef CHOICEDIALOG_H
#define CHOICEDIALOG_H

#include <QDialog>

namespace Ui {
class ChoiceDialog;
}
class QRadioButton;

class ChoiceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChoiceDialog(QWidget *parent = nullptr);
    ~ChoiceDialog();

    void init(const QStringList& items, int checkedIndex = 0);

    int checkedIndex() const;

protected:
    QList<QRadioButton*> mButtons;
    Ui::ChoiceDialog *ui;
};

#endif // CHOICEDIALOG_H
