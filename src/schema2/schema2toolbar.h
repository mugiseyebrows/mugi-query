#ifndef SCHEMA2TOOLBAR_H
#define SCHEMA2TOOLBAR_H

#include <QWidget>

class QPushButton;

namespace Ui {
class Schema2Toolbar;
}

class Schema2Toolbar : public QWidget
{
    Q_OBJECT

public:
    enum Mode {
        ModeNone,
        ModeMove,
        ModeRelate,
        ModeUnrelate,
        ModeAlter,
        ModeInsert,
        ModeSelect,
        ModeDrop
    };
    explicit Schema2Toolbar(QWidget *parent = nullptr);
    ~Schema2Toolbar();

    Mode mode() const;


signals:
    void clearTableStack();

protected:
    Mode mMode;

    void uncheckAllExcept(QPushButton *checked);
    void syncMove();
protected slots:

    void on_move_clicked(bool checked);
    void on_relate_clicked(bool checked);
    void on_unrelate_clicked(bool checked);
    void on_drop_clicked(bool checked);
    void on_alter_clicked(bool checked);
    void on_insert_clicked(bool checked);
private:
    Ui::Schema2Toolbar *ui;
};

#endif // SCHEMA2TOOLBAR_H
