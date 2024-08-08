#include "daterangewidget.h"
#include "ui_daterangewidget.h"

#define INDEX_ALL 3
#define INDEX_RANGE 4

DateRangeWidget::DateRangeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DateRangeWidget)
{
    ui->setupUi(this);

    ui->combo->addItems({
        "7 days",
        "30 days",
        "365 days",
        "all",
        "range"
    });

    ui->combo->setCurrentIndex(0);

    ui->date1->setDate(QDate::currentDate().addDays(-7));
    ui->date2->setDate(QDate::currentDate().addDays(1));
}

DateRangeWidget::~DateRangeWidget()
{
    delete ui;
}

DateRangeWidget::Mode DateRangeWidget::mode() const
{
    switch(ui->combo->currentIndex()) {
    case 3: return All;
    case 4: return Range;
    default:
        return NDays;
    }
    return NDays;
}

int DateRangeWidget::days() const
{
    QList<int> ns = {7,30,365};
    return ns.value(ui->combo->currentIndex(), -1);
}

QDate DateRangeWidget::date1() const
{
    return ui->date1->date();
}

QDate DateRangeWidget::date2() const
{
    return ui->date2->date();
}

void DateRangeWidget::on_combo_currentIndexChanged(int index)
{
    ui->date1->setVisible(index == INDEX_RANGE);
    ui->date2->setVisible(index == INDEX_RANGE);
    if (index == INDEX_RANGE) {
        emit expanded();
    } else {
        emit contracted();
    }
    emit changed(mode(), days(), ui->date1->date(), ui->date2->date());
    //qDebug() << "emit changed days" << days();
}

void DateRangeWidget::on_date1_dateChanged(const QDate &date)
{
    emit changed(mode(), days(), ui->date1->date(), ui->date2->date());
    //qDebug() << "emit changed days" << days();
}

void DateRangeWidget::on_date2_dateChanged(const QDate &date)
{
    emit changed(mode(), days(), ui->date1->date(), ui->date2->date());
    //qDebug() << "emit changed days" << days();
}

