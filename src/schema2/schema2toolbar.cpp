#include "schema2toolbar.h"
#include "ui_schema2toolbar.h"

Schema2Toolbar::Schema2Toolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Schema2Toolbar), mMode(ModeMove)
{
    ui->setupUi(this);
    syncMove();
}

Schema2Toolbar::~Schema2Toolbar()
{
    delete ui;
}

Schema2Toolbar::Mode Schema2Toolbar::mode() const
{
    return mMode;
}

void Schema2Toolbar::uncheckAllExcept(QPushButton* checked) {
    QList<QPushButton*> buttons = {ui->move, ui->relate, ui->alter, ui->insert, ui->unrelate, ui->drop};
    for(QPushButton* button: buttons) {
        if (button == checked) {
            continue;
        }
        button->setChecked(false);
    }
}

void Schema2Toolbar::on_move_clicked(bool checked)
{
    uncheckAllExcept(ui->move);
    mMode = checked ? ModeMove : ModeMove;
    syncMove();
}

void Schema2Toolbar::on_relate_clicked(bool checked)
{
    uncheckAllExcept(ui->relate);
    mMode = checked ? ModeRelate : ModeMove;
    //emit clearTableStack();
    syncMove();
}

void Schema2Toolbar::on_unrelate_clicked(bool checked)
{
    uncheckAllExcept(ui->unrelate);
    mMode = checked ? ModeUnrelate : ModeMove;
    //emit clearTableStack();
    syncMove();
}

void Schema2Toolbar::on_drop_clicked(bool checked)
{
    uncheckAllExcept(ui->drop);
    mMode = checked ? ModeDrop : ModeMove;
    syncMove();
}

void Schema2Toolbar::on_alter_clicked(bool checked)
{
    uncheckAllExcept(ui->alter);
    mMode = checked ? ModeAlter : ModeMove;
    syncMove();
}

void Schema2Toolbar::syncMove() {
    if (mMode == ModeMove) {
        ui->move->setChecked(true);
    }
    emit modeChanged(mMode);
}

void Schema2Toolbar::on_insert_clicked(bool checked)
{
    uncheckAllExcept(ui->insert);
    mMode = checked ? ModeInsert : ModeMove;
    syncMove();
}
