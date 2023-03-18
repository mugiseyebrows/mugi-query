#include "schema2pushview.h"
#include "ui_schema2pushview.h"

#include "tablestretcher.h"
#include "schema2changeset.h"

Schema2PushView::Schema2PushView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Schema2PushView)
{
    ui->setupUi(this);

    TableStretcher* stretcher = new TableStretcher();
    stretcher->setView(ui->table);
    stretcher->setRatio({1,1});
}

Schema2PushView::~Schema2PushView()
{
    delete ui;
}

void Schema2PushView::setModel(Schema2ChangeSet *model)
{
    ui->table->setModel(model);
}
