#include "schema2synctoolbar.h"
#include "ui_schema2synctoolbar.h"

Schema2SyncToolbar::Schema2SyncToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Schema2SyncToolbar)
{
    ui->setupUi(this);

    connect(ui->pull,SIGNAL(clicked(bool)),this,SIGNAL(pull()));
    connect(ui->push,SIGNAL(clicked(bool)),this,SIGNAL(push()));
}

Schema2SyncToolbar::~Schema2SyncToolbar()
{
    delete ui;
}
