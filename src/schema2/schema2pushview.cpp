#include "schema2pushview.h"
#include "ui_schema2pushview.h"

Schema2PushView::Schema2PushView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Schema2PushView)
{
    ui->setupUi(this);
}

Schema2PushView::~Schema2PushView()
{
    delete ui;
}
