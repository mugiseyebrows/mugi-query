#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);
    ui->format->init({"Svg", "Png", "Dot", "dbdiagram.io"}, 0);

    mFormats = {
        Schema2Export::SvgFormat,
        Schema2Export::PngFormat,
        Schema2Export::DotFormat,
        Schema2Export::DbioFormat,
    };

}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::setName(const QString &name)
{
    ui->output->setName(name);
    on_format_clicked(ui->format->checkedIndex());
}

bool ExportDialog::clipboard() const
{
    return ui->output->clipboard();
}

QString ExportDialog::path() const
{
    return ui->output->path();
}

Schema2Export::ExportFormat ExportDialog::format() const
{
    int checked = ui->format->checkedIndex();
    return mFormats[checked];
}

bool ExportDialog::cropViewport() const
{
    return ui->cropViewport->isChecked();
}

bool ExportDialog::cropAll() const
{
    return ui->cropAll->isChecked();
}

void ExportDialog::setFormat(int index)
{
    ui->format->setChecked(index);
}

void ExportDialog::setItemsSelected(bool value)
{
    if (!value) {
        ui->itemsAll->setChecked(true);
    } else {
        ui->itemsSelected->setChecked(true);
    }
}

void ExportDialog::setItemsAll(bool value)
{
    if (value) {
        ui->itemsAll->setChecked(true);
    } else {
        ui->itemsSelected->setChecked(true);
    }
}

void ExportDialog::setCropViewport(bool value)
{
    if (!value) {
        ui->cropAll->setChecked(true);
    } else {
        ui->cropViewport->setChecked(true);
    }
}

void ExportDialog::setCropAll(bool value)
{
    if (value) {
        ui->cropAll->setChecked(true);
    } else {
        ui->cropViewport->setChecked(true);
    }
}

void ExportDialog::setClipboard(bool value)
{
    ui->output->setClipboard(value);
}

void ExportDialog::setPath(const QString &path)
{
    ui->output->setPath(path);
}

bool ExportDialog::itemsSelected() const
{
    return ui->itemsSelected->isChecked();
}

bool ExportDialog::itemsAll() const
{
    return ui->itemsAll->isChecked();
}

void ExportDialog::on_format_clicked(int index)
{
    static QStringList exts = {"svg", "png", "dot", "txt"};
    ui->output->setExt(exts[index]);
}

void ExportDialog::updateExt() {
    static QStringList exts = {"svg", "png", "dot", "txt"};
    int index = ui->format->checkedIndex();
    ui->output->setExt(exts[index]);
}
