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

bool ExportDialog::overwrite() const
{
    return ui->output->overwrite();
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

void ExportDialog::on_format_clicked(int index)
{
    static QStringList exts = {"svg", "png", "dot", "txt"};
    ui->output->setExt(exts[index]);
}
