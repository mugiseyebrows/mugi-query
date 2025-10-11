#include "colordialog.h"
#include "ui_colordialog.h"

#include <QDebug>
#include "contrasting.h"
#include <QRegularExpression>

ColorDialog::ColorDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ColorDialog)
{
    ui->setupUi(this);

    QList<QColor> colors = {
        "#ff0003", "#ff2800", "#ff6600", "#ffb500", "#ecff00", "#84ff00", "#13ff00", "#00ff65", "#00ffe3", "#0098ff", "#0016ff", "#6a00ff", "#e900ff", "#ff009c", "#ff4346", "#ff6143", "#ff8f43", "#ffc843", "#f1ff43", "#a5ff43", "#52ff43", "#43ff8e", "#43ffeb", "#43b4ff", "#4354ff", "#9243ff", "#ee43ff", "#ff43b6", "#d20003", "#d22100", "#d25400", "#d29500", "#c3d200", "#6dd200", "#0fd200", "#00d253", "#00d2bb", "#007ed2", "#0012d2", "#5800d2", "#c000d2", "#d20080", "#d2383a", "#d25038", "#d27638", "#d2a538", "#c7d238", "#88d238", "#43d238", "#38d275", "#38d2c1", "#3894d2", "#3845d2", "#7838d2", "#c538d2", "#d23896", "#a50002", "#a51a00", "#a54200", "#a57500", "#99a500", "#56a500", "#0ca500", "#00a542", "#00a594", "#0063a5", "#000ea5", "#4500a5", "#9700a5", "#a50065", "#a52c2e", "#a53f2c", "#a55d2c", "#a5822c", "#9da52c", "#6ba52c", "#35a52c", "#2ca55c", "#2ca598", "#2c75a5", "#2c36a5", "#5f2ca5", "#9b2ca5", "#a52c76", "#790001", "#791300", "#793000", "#795600", "#707900", "#3f7900", "#097900", "#007930", "#00796c", "#004879", "#000a79", "#320079", "#6e0079", "#79004a", "#792021", "#792e20", "#794420", "#795f20", "#727920", "#4e7920", "#277920", "#207943", "#20796f", "#205579", "#202879", "#452079", "#712079", "#792056", "#4c0001", "#4c0c00", "#4c1e00", "#4c3600", "#474c00", "#274c00", "#054c00", "#004c1e", "#004c44", "#002d4c", "#00064c", "#20004c", "#45004c", "#4c002e", "#4c1415", "#4c1d14", "#4c2b14", "#4c3c14", "#484c14", "#314c14", "#184c14", "#144c2a", "#144c46", "#14364c", "#14194c", "#2b144c", "#47144c", "#4c1436", "#ff8889", "#ff9a88", "#ffb788", "#ffdc88", "#f6ff88", "#c5ff88", "#91ff88", "#88ffb7", "#88fff2", "#88cfff", "#8892ff", "#b988ff", "#f488ff", "#ff88d0", "#ffcccc", "#ffd4cc", "#ffe0cc", "#fff0cc", "#fbffcc", "#e6ffcc", "#cfffcc", "#ccffe0", "#ccfff9", "#cceaff", "#ccd0ff", "#e1ccff", "#faccff", "#ffcceb", "#d27071", "#d27f70", "#d29770", "#d2b570", "#cbd270", "#a3d270", "#77d270", "#70d297", "#70d2c7", "#70abd2", "#7078d2", "#9970d2", "#c970d2", "#d270ac", "#d2a8a8", "#d2aea8", "#d2b9a8", "#d2c6a8", "#cfd2a8", "#bed2a8", "#abd2a8", "#a8d2b9", "#a8d2cd", "#a8c1d2", "#a8abd2", "#b9a8d2", "#cea8d2", "#d2a8c2", "#a55859", "#a56458", "#a57758", "#a58f58", "#a0a558", "#80a558", "#5ea558", "#58a577", "#58a59d", "#5886a5", "#585fa5", "#7858a5", "#9f58a5", "#a55887", "#a58485", "#a58984", "#a59184", "#a59c84", "#a3a584", "#95a584", "#87a584", "#84a591", "#84a5a2", "#8498a5", "#8487a5", "#9284a5", "#a284a5", "#a58498", "#794041", "#794940", "#795740", "#796840", "#757940", "#5e7940", "#447940", "#407957", "#407973", "#406279", "#404579", "#584079", "#744079", "#794063", "#796061", "#796460", "#796a60", "#797260", "#777960", "#6d7960", "#627960", "#60796a", "#607976", "#606f79", "#606379", "#6b6079", "#776079", "#79606f", "#4c2829", "#4c2e28", "#4c3728", "#4c4228", "#494c28", "#3b4c28", "#2b4c28", "#284c37", "#284c48", "#283e4c", "#282b4c", "#37284c", "#49284c", "#4c283e", "#4c3d3d", "#4c3f3d", "#4c433d", "#4c483d", "#4b4c3d", "#454c3d", "#3e4c3d", "#3d4c43", "#3d4c4a", "#3d464c", "#3d3e4c", "#433d4c", "#4b3d4c", "#4c3d46", "#ffffff", "#f5f5f5", "#ececec", "#e2e2e2", "#d9d9d9", "#cfcfcf", "#c6c6c6", "#bcbcbc", "#b3b3b3", "#aaaaaa", "#a0a0a0", "#979797", "#8d8d8d", "#848484", "#7a7a7a", "#717171", "#676767", "#5e5e5e", "#555555", "#4b4b4b", "#424242", "#383838", "#2f2f2f", "#252525", "#1c1c1c", "#121212", "#090909", "#000000"
    };
    ui->colors->setColors(colors);

    ui->colors->setColumns(28);

    adjustSize();

    connect(ui->colors, &ColorTableWidget::colorClicked, this, &ColorDialog::onColorClicked);

    connect(ui->r, &IntLineEdit::valueChanged, this, &ColorDialog::onRgb);
    connect(ui->g, &IntLineEdit::valueChanged, this, &ColorDialog::onRgb);
    connect(ui->b, &IntLineEdit::valueChanged, this, &ColorDialog::onRgb);

    connect(ui->h, &IntLineEdit::valueChanged, this, &ColorDialog::onHsv);
    connect(ui->s, &IntLineEdit::valueChanged, this, &ColorDialog::onHsv);
    connect(ui->v, &IntLineEdit::valueChanged, this, &ColorDialog::onHsv);

    connect(ui->hex, &QLineEdit::textChanged, this, &ColorDialog::onHex);

}

void ColorDialog::onHex(const QString& hex) {
    if (!mListen) {
        return;
    }
    static QRegularExpression rx("^#[0-9a-f]{6}$", QRegularExpression::CaseInsensitiveOption);
    if (rx.match(hex).hasMatch()) {
        QColor color(hex);
        setColor(color, true, true, false);
    }
}

void ColorDialog::onHsv(int value) {

    if (!mListen) {
        return;
    }

    int h = ui->h->value();
    int s = ui->s->value();
    int v = ui->v->value();

    if ((value < 0) || (value > 359)) {
        return;
    }

    if ((h < 0) || (h > 359)) {
        return;
    }
    if ((s < 0) || (s > 255)) {
        return;
    }
    if ((v < 0) || (v > 255)) {
        return;
    }

    QColor color = QColor::fromHsv(h, s, v);
    setColor(color, true, false, true);
}

void ColorDialog::onRgb(int value) {

    if (!mListen) {
        return;
    }

    if ((value < 0) || (value > 255)) {
        return;
    }

    int r = ui->r->value();
    int g = ui->g->value();
    int b = ui->b->value();

    if ((r < 0) || (r > 255)) {
        return;
    }
    if ((g < 0) || (g > 255)) {
        return;
    }
    if ((b < 0) || (b > 255)) {
        return;
    }

    QColor color = QColor::fromRgb(r,g,b);
    setColor(color, false, true, true);


}

void ColorDialog::setColor(const QColor& color, bool setRgb, bool setHsv, bool setHex) {

    QPalette palette = ui->hex->palette();
    QColor text = contrasting(color, Qt::white, Qt::black);
    palette.setColor(QPalette::Base, color);
    palette.setColor(QPalette::Text, text);
    ui->hex->setPalette(palette);

    if (setHex) {
        mListen = false;
        ui->hex->setText(color.name());
        mListen = true;
    }

    if (setRgb) {
        mListen = false;
        int r,g,b;
        color.getRgb(&r, &g, &b);
        ui->r->setValue(r);
        ui->g->setValue(g);
        ui->b->setValue(b);
        mListen = true;
    }

    if (setHsv) {
        mListen = false;
        int h,s,v;
        color.getHsv(&h, &s, &v);
        ui->h->setValue(h);
        ui->s->setValue(s);
        ui->v->setValue(v);
        mListen = true;
    }

}

QColor ColorDialog::color() const
{
    return QColor(ui->hex->text());
}

void ColorDialog::onColorClicked(QColor color) {

    setColor(color, true, true, true);
}

ColorDialog::~ColorDialog()
{
    delete ui;
}
