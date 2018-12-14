#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "settings.h"

namespace  {

void setValueOrDefault(QLineEdit* edit, const QString& v, const QString d) {
    if (v.trimmed().isEmpty()) {
        edit->setText(d);
    } else {
        edit->setText(v);
    }
}

}

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    Settings* s = Settings::instance();

    DisplayFormat::init(s->dateTimeLocale(), ui->dateCLocale, ui->dateCurrentLocale, ui->dateSpecial);
    DisplayFormat::init(s->realLocale(),ui->realCLocale,ui->realCurrentLocale,0);

    setValueOrDefault(ui->dateTime,s->dateTimeFormat(),"yyyy-MM-dd hh:mm:ss");
    setValueOrDefault(ui->date,s->dateFormat(),"yyyy-MM-dd");
    setValueOrDefault(ui->time,s->timeFormat(),"hh:mm:ss");
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

DisplayFormat::Locale SettingsDialog::dateTimeLocale() const
{
    return DisplayFormat::value(ui->dateCLocale, ui->dateCurrentLocale, ui->dateSpecial);
}

DisplayFormat::Locale SettingsDialog::realLocale() const
{
    return DisplayFormat::value(ui->realCLocale, ui->realCurrentLocale, 0);
}

QString SettingsDialog::dateTime() const
{
    return ui->dateTime->text();
}

QString SettingsDialog::date() const
{
    return ui->date->text();
}

QString SettingsDialog::time() const
{
    return ui->time->text();
}

void SettingsDialog::on_dateSpecial_clicked()
{

}

void SettingsDialog::on_dateCurrentLocale_clicked()
{

}

void SettingsDialog::on_dateCLocale_clicked()
{

}

void SettingsDialog::accept()
{
    Settings* s = Settings::instance();

    s->setDateTimeLocale(dateTimeLocale());
    s->setRealLocale(realLocale());
    s->setDateFormat(date());
    s->setTimeFormat(time());
    s->setDateTimeFormat(dateTime());

    QDialog::accept();
}
