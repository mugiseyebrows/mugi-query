#include "schema2actiontoolbar.h"
#include "ui_schema2actiontoolbar.h"

Schema2ActionToolbar::Schema2ActionToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Schema2ActionToolbar)
{
    ui->setupUi(this);
}

Schema2ActionToolbar::~Schema2ActionToolbar()
{
    delete ui;
}

void Schema2ActionToolbar::on_create_clicked() {
    emit action(Create);
}

void Schema2ActionToolbar::on_arrange_clicked() {
    emit action(Arrange);
}

void Schema2ActionToolbar::on_script_clicked() {
    emit action(Script);
}

void Schema2ActionToolbar::on_save_clicked() {
    emit action(Save);
}
