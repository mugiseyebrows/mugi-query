#include "copyformatwidget.h"
#include "ui_copyformatwidget.h"

#include <QInputDialog>

CopyFormatWidget::CopyFormatWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CopyFormatWidget)
{
    ui->setupUi(this);

    ui->format->addItems(CopyFormat2::formats());

    ui->sep->addItems(CopyFormat2::seps());

    ui->lang->addItems(CopyFormat2::langs());

    ui->axis->addItems(CopyFormat2::axes());

    on_format_currentIndexChanged(ui->format->currentIndex());
}

CopyFormatWidget::~CopyFormatWidget()
{
    delete ui;
}

CopyFormat2 CopyFormatWidget::format() const
{
    return CopyFormat2((CopyFormat2::Format) ui->format->currentIndex(),
                       (CopyFormat2::Sep) ui->sep->currentIndex(),
                       (CopyFormat2::Lang) ui->lang->currentIndex(),
                       (CopyFormat2::Axis) ui->axis->currentIndex(),
                       mKey);
}

void CopyFormatWidget::setKey(const QStringList &key)
{
    mKey = key;
    ui->key->setText("key: " + key.join(", "));
}

void CopyFormatWidget::on_format_currentIndexChanged(int index)
{
    CopyFormat2::Format format = (CopyFormat2::Format) index;
    ui->sep->setVisible(CopyFormat2::sepRelevant(format));
    ui->lang->setVisible(CopyFormat2::langRelevant(format));
    ui->axis->setVisible(CopyFormat2::axisRelevant(format));
    ui->key->setVisible(CopyFormat2::keyRelevant(format));
}


void CopyFormatWidget::on_key_clicked()
{
    emit keyClicked(mKey);
}

