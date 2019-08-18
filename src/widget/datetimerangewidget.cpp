#include "datetimerangewidget.h"
#include "ui_datetimerangewidget.h"
#include <QMenu>

DateTimeRangeWidget::DateTimeRangeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DateTimeRangeWidget),
    mMenu1(0)
{
    ui->setupUi(this);

    connect(ui->dateTime1,SIGNAL(dateTimeChanged(QDateTime)),this,SIGNAL(dateTime1Changed(QDateTime)));
    connect(ui->dateTime2,SIGNAL(dateTimeChanged(QDateTime)),this,SIGNAL(dateTime2Changed(QDateTime)));
}

DateTimeRangeWidget::~DateTimeRangeWidget()
{
    delete ui;
}

void DateTimeRangeWidget::init(const QDateTime& dateTime1,
                               const QDateTime& dateTime2,
                               const QStringList& options) {

    ui->dateTime1->setDateTime(dateTime1);
    ui->dateTime2->setDateTime(dateTime2);

    mMenu1 = new QMenu(this);
    //mMenu2 = new QMenu(this);

    for(int i=0;i<options.size();i++) {
        QString option = options[i];
        QAction* action = new QAction(option, this);
        mMenu1->addAction(action);
        connect(action,&QAction::triggered,[=](){
           emit actionTriggered(i);
        });
    }

    QAction* menuAction1 = mMenu1->menuAction();
    menuAction1->setIcon(QIcon(":/icons/src/icons/undo.png"));
    ui->toolBar1->addAction(menuAction1);

    QAction* action2 = new QAction(QIcon(":/icons/src/icons/undo.png"),QString(),this);

    ui->toolBar2->addAction(action2);

    connect(menuAction1,&QAction::triggered,[=](){
        ui->dateTime1->setDateTime(dateTime1);
    });
    connect(action2,&QAction::triggered,[=](){
        ui->dateTime2->setDateTime(dateTime2);
    });

}

QDateTime DateTimeRangeWidget::dateTime1() const
{
    return ui->dateTime1->dateTime();
}

QDateTime DateTimeRangeWidget::dateTime2() const
{
    return ui->dateTime2->dateTime();
}

void DateTimeRangeWidget::setDateTime1(const QDateTime &value)
{
    ui->dateTime1->setDateTime(value);
}

void DateTimeRangeWidget::setDateTime2(const QDateTime &value)
{
    ui->dateTime2->setDateTime(value);
}
