#include "sessiontab.h"
#include "ui_sessiontab.h"

SessionTab::SessionTab(const QString &database, const QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SessionTab), mDatabase(database), mName(name)
{
    ui->setupUi(this);
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
