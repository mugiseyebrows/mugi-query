#include "fieldattributeswidget.h"
#include "ui_fieldattributeswidget.h"

FieldAttributesWidget::FieldAttributesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FieldAttributesWidget)
{
    ui->setupUi(this);

    connect(ui->primaryKey,SIGNAL(clicked(bool)),this,SIGNAL(primaryKeyClicked(bool)));
    connect(ui->autoincrement,SIGNAL(clicked(bool)),this,SIGNAL(autoincrementClicked(bool)));
}

FieldAttributesWidget::~FieldAttributesWidget()
{
    delete ui;
}

bool FieldAttributesWidget::primaryKey() const
{
    return ui->primaryKey->isChecked();
}

bool FieldAttributesWidget::autoincrement() const
{
    return ui->autoincrement->isChecked();
}

void FieldAttributesWidget::on_primaryKey_stateChanged(int state)
{
    //ui->autoincrement->setVisible(state == Qt::Checked);
}
