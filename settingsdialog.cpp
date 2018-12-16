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

/*
    // (.*) m(.*);$
    // initOption(ui->\2,s->\2());
    // Real
    // real
    // DateTime
    // dateTime

    bool mDateTimeOverrideForCsv;
    bool mDateTimeOverrideForCopy;
    bool mRealOverrideForCopy;
    bool mRealOverrideForCsv;
    bool mRealUseLocale;
    bool mDateTimeUseLocale;

*/

namespace {


void initOption(QCheckBox* checkbox, bool value) {
    checkbox->setChecked(value);
}

void initOption(QRadioButton* yes, QRadioButton* no, bool value) {
    if (value) {
        yes->setChecked(true);
    } else {
        no->setChecked(true);
    }
}



}

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    Settings* s = Settings::instance();

    setValueOrDefault(ui->dateFormat,s->dateFormat(),"yyyy-MM-dd");
    setValueOrDefault(ui->timeFormat,s->timeFormat(),"hh:mm:ss");

    initOption(ui->dateTimeOverrideForCsv, s->dateTimeOverrideForCsv());
    initOption(ui->dateTimeOverrideForCopy, s->dateTimeOverrideForCopy());
    initOption(ui->realOverrideForCopy, s->realOverrideForCopy());
    initOption(ui->realOverrideForCsv, s->realOverrideForCsv());
    initOption(ui->realUseLocale, s->realUseLocale());
    initOption(ui->dateTimeUseLocale, ui->dateTimeUseSpecial, s->dateTimeUseLocale());

    setDateTimeEnabled(!ui->dateTimeUseLocale->isChecked());
    setRealEnabled(ui->realUseLocale->isChecked());

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

#include <functional>

namespace {

void saveOption(Settings*s, QCheckBox* cb, const std::function<void(Settings*,bool)>& fn) {
    fn(s,cb->isChecked());
}

void saveOption(Settings *s, QRadioButton* yes, const std::function<void(Settings*,bool)> &fn) {
    fn(s,yes->isChecked());
}

void saveOption(Settings *s, QLineEdit* edit, const std::function<void(Settings *, const QString&)> &fn) {
    fn(s,edit->text());
}

// (.*) m(.*);$
// saveOption(s, ui->\2, &Settings::set\2);

/*
bool mDateTimeOverrideForCsv;
bool mDateTimeOverrideForCopy;
bool mRealOverrideForCopy;
bool mRealOverrideForCsv;
bool mRealUseLocale;
QString mDateFormat;
QString mTimeFormat;
bool mDateTimeUseLocale;

*/

}


void SettingsDialog::accept()
{
    Settings* s = Settings::instance();

    saveOption(s, ui->dateTimeOverrideForCsv, &Settings::setDateTimeOverrideForCsv);
    saveOption(s, ui->dateTimeOverrideForCopy, &Settings::setDateTimeOverrideForCopy);
    saveOption(s, ui->realOverrideForCopy, &Settings::setRealOverrideForCopy);
    saveOption(s, ui->realOverrideForCsv, &Settings::setRealOverrideForCsv);
    saveOption(s, ui->realUseLocale, &Settings::setRealUseLocale);
    saveOption(s, ui->dateFormat, &Settings::setDateFormat);
    saveOption(s, ui->timeFormat, &Settings::setTimeFormat);
    saveOption(s, ui->dateTimeUseLocale, &Settings::setDateTimeUseLocale);

    QDialog::accept();
}

void SettingsDialog::setDateTimeEnabled(bool state) {
    ui->dateFormat->setEnabled(state);
    ui->timeFormat->setEnabled(state);
    ui->dateTimeOverrideForCopy->setEnabled(state);
    ui->dateTimeOverrideForCsv->setEnabled(state);
}

void SettingsDialog::setRealEnabled(bool state) {
    ui->realOverrideForCopy->setEnabled(state);
    ui->realOverrideForCsv->setEnabled(state);
}

void SettingsDialog::on_dateTimeUseLocale_clicked(bool checked)
{
    setDateTimeEnabled(!checked);
}

void SettingsDialog::on_dateTimeUseSpecial_clicked(bool checked)
{
    setDateTimeEnabled(checked);
}

void SettingsDialog::on_realUseLocale_clicked(bool checked)
{
    setRealEnabled(checked);
}
