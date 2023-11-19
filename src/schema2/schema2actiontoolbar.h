#ifndef SCHEMA2ACTIONTOOLBAR_H
#define SCHEMA2ACTIONTOOLBAR_H

#include <QWidget>

namespace Ui {
class Schema2ActionToolbar;
}

class Schema2ActionToolbar : public QWidget
{
    Q_OBJECT

public:
    enum Action {
        Create,
        Arrange,
        Script,
        Save
    };

    explicit Schema2ActionToolbar(QWidget *parent = nullptr);
    ~Schema2ActionToolbar();

signals:
    void action(int);

protected slots:
    void on_create_clicked();
    void on_arrange_clicked();
    void on_script_clicked();
    void on_save_clicked();
private:
    Ui::Schema2ActionToolbar *ui;
};

#endif // SCHEMA2ACTIONTOOLBAR_H
