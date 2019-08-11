#include "fieldattributeswidget.h"
#include "ui_fieldattributeswidget.h"

FieldAttributesWidget::FieldAttributesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FieldAttributesWidget)
{
    ui->setupUi(this);

    connect(ui->primaryKey,SIGNAL(clicked(bool)),this,SIGNAL(primaryKeyClicked(bool)));
    connect(ui->autoincrement,SIGNAL(clicked(bool)),this,SIGNAL(autoincrementClicked(bool)));
    connect(ui->index,SIGNAL(clicked(bool)),this,SIGNAL(indexClicked(bool)));
    connect(ui->unique,SIGNAL(clicked(bool)),this,SIGNAL(uniqueClicked(bool)));

    connect(ui->primaryKey,SIGNAL(clicked(bool)),this,SIGNAL(attributeClicked(bool)));
    connect(ui->autoincrement,SIGNAL(clicked(bool)),this,SIGNAL(attributeClicked(bool)));
    connect(ui->index,SIGNAL(clicked(bool)),this,SIGNAL(attributeClicked(bool)));
    connect(ui->unique,SIGNAL(clicked(bool)),this,SIGNAL(attributeClicked(bool)));
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

bool FieldAttributesWidget::index() const
{
    return ui->index->isChecked();
}

bool FieldAttributesWidget::unique() const
{
    return ui->unique->isChecked();
}

void FieldAttributesWidget::setPrimaryKey(bool value)
{
    ui->primaryKey->setChecked(value);
}

void FieldAttributesWidget::setAutoincrement(bool value)
{
    ui->autoincrement->setChecked(value);
}
