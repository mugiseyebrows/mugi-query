#include "datetimerangewidget.h"
#include "ui_datetimerangewidget.h"
#include <QMenu>

DateTimeRangeWidget::DateTimeRangeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DateTimeRangeWidget),
    mMenu(0)
{
    ui->setupUi(this);

    connect(ui->dateTime1,&QDateTimeEdit::dateTimeChanged,[=](QDateTime dateTime){
        emit dateTime1Changed(dateTime);
        emit dateTimesChanged(dateTime,ui->dateTime2->dateTime());
    });
    connect(ui->dateTime2,&QDateTimeEdit::dateTimeChanged,[=](QDateTime dateTime){
        emit dateTime2Changed(dateTime);
        emit dateTimesChanged(ui->dateTime1->dateTime(),dateTime);
    });

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

    mMenu = new QMenu(this);
    //mMenu2 = new QMenu(this);

    for(int i=0;i<options.size();i++) {
        QString option = options[i];
        QAction* action = new QAction(option, this);
        mMenu->addAction(action);
        connect(action,&QAction::triggered,[=](){
           emit actionTriggered(i);
        });
    }

    QAction* menuAction = mMenu->menuAction();
    menuAction->setIcon(QIcon(":/icons/src/icons/undo.png"));
    ui->toolBar2->addAction(menuAction);

    QAction* action = new QAction(QIcon(":/icons/src/icons/undo.png"),QString(),this);

    ui->toolBar1->addAction(action);

    connect(action,&QAction::triggered,[=](){
        ui->dateTime1->setDateTime(dateTime1);
    });

    connect(menuAction,&QAction::triggered,[=](){
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
