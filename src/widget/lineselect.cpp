#include "lineselect.h"
#include "ui_lineselect.h"
#include <QFileDialog>

LineSelect::LineSelect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LineSelect)
{
    ui->setupUi(this);
    connect(ui->edit,SIGNAL(textChanged(QString)),this,SIGNAL(textChanged(QString)));
}

LineSelect::~LineSelect()
{
    delete ui;
}

void LineSelect::setMode(Mode mode) {
    mMode = mode;
}

void LineSelect::setCaption(const QString &caption) {
    mCaption = caption;
}

void LineSelect::setFilter(const QString &filter) {
    mFilter = filter;
}

QString LineSelect::text() const {
    return ui->edit->text();
}

void LineSelect::setText(const QString& text) {
    ui->edit->setText(text);
}

void LineSelect::on_select_clicked()
{
    QString path = ui->edit->text();

    if (mMode == ModeFileSave) {
        path = QFileDialog::getSaveFileName(this, mCaption, path, mFilter);
    } else if (mMode == ModeFileOpen) {
        path = QFileDialog::getOpenFileName(this, mCaption, path, mFilter);
    } else if (mMode == ModeDir) {
        path = QFileDialog::getExistingDirectory(this, mCaption, path);
    }
    if (path != "") {
        ui->edit->setText(QDir::toNativeSeparators(path));
    }
}
