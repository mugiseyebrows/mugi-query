#include "sessiontab.h"
#include "ui_sessiontab.h"

SessionTab::SessionTab(const QString &database, const QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SessionTab), mDatabase(database), mName(name)
{
    ui->setupUi(this);

    while(ui->resultTabs->count() > 0) {
        ui->resultTabs->removeTab(0);
    }

}

SessionTab::~SessionTab()
{
    delete ui;
}

QString SessionTab::database() const
{
    return mDatabase;
}

QString SessionTab::name() const
{
    return mName;
}

void SessionTab::on_execute_clicked()
{
    emit query(ui->query->toPlainText());
}
