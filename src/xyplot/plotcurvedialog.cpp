#include "plotcurvedialog.h"
#include "ui_plotcurvedialog.h"

#include "colordialog.h"

#include <QStandardItemModel>
#include <QwtSymbol>
#include <QPainter>
#include "plotcurve.h"
#include <QStringListModel>
#include <QCompleter>

static QAbstractItemModel* symbolsModel() {
    QStandardItemModel* model = new QStandardItemModel(0,1);

    for(int style=0;style<QwtSymbol::Hexagon;style++) {

        QwtSymbol symbol((QwtSymbol::Style) style, QBrush(Qt::black), QPen(Qt::black), QSize(20, 20));

        QImage image(40, 40, QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        //image.fill(Qt::red);
        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing);
        symbol.drawSymbol(&painter, QPointF(20, 20));
        QIcon icon = QIcon(QPixmap::fromImage(image));

        auto* item = new QStandardItem();

        //auto icon = QIcon::fromTheme(QIcon::ThemeIcon::EditCopy);
        item->setIcon(icon);
        model->appendRow(item);
    }
    return model;
}

PlotCurveDialog::PlotCurveDialog(const QStringList& columns, PlotCurve* curve, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PlotCurveDialog)
{
    ui->setupUi(this);
    //ui->lineColor->setColor(Qt::red);
    //ui->symbolColor->setColor(Qt::green);
    auto* model = symbolsModel();
    ui->symbolStyle->setModel(model);

    mCurve = curve;

    QColor lineColor = curve->pen().color();
    QColor symbolColor = lineColor;
    const QwtSymbol* symbol = curve->symbol();

    ui->line->setChecked(curve->style() != QwtPlotCurve::NoCurve);
    ui->symbol->setChecked(symbol != nullptr);

    QwtSymbol::Style symbolStyle = QwtSymbol::Ellipse;

    if (symbol != nullptr) {
        symbolColor = symbol->brush().color();
        symbolStyle = symbol->style();
    }

    ui->lineColor->setColor(lineColor);
    ui->symbolColor->setColor(symbolColor);
    ui->symbolStyle->setCurrentIndex(symbolStyle);

    ui->x->setText(curve->x());
    ui->y->setText(curve->y());

    QCompleter* completer = new QCompleter(this);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);

    QStringListModel* completerModel = new QStringListModel(columns);
    completer->setModel(completerModel);

    ui->x->setCompleter(completer);
    ui->y->setCompleter(completer);

    connect(ui->lineColor, &ColorButton::colorChanged, [=](QColor){
        ui->line->setChecked(true);
    });

    connect(ui->symbolColor, &ColorButton::colorChanged, [=](QColor){
        ui->symbol->setChecked(true);
    });

    connect(ui->symbolStyle, &QComboBox::currentIndexChanged, [=](int){
        ui->symbol->setChecked(true);
    });

}

PlotCurveDialog::~PlotCurveDialog()
{
    delete ui;
}

bool PlotCurveDialog::remove() const {
    return mRemove;
}

void PlotCurveDialog::updateCurve()
{

    QColor lineColor = ui->lineColor->color();
    QColor symbolColor = ui->symbolColor->color();
    QwtSymbol::Style symbolStyle = (QwtSymbol::Style) ui->symbolStyle->currentIndex();

    bool line = ui->line->isChecked();

    mCurve->setStyle(line ? QwtPlotCurve::Lines : QwtPlotCurve::NoCurve);
    mCurve->setPen(lineColor, 2.0);
    QwtSymbol* symbol = nullptr;
    if (ui->symbol->isChecked()) {
        symbol = new QwtSymbol(symbolStyle, QBrush(symbolColor), QPen(symbolColor), QSize(6,6));
    }
    mCurve->setSymbol(symbol);

    mCurve->setX(ui->x->text());
    mCurve->setY(ui->y->text());

    mCurve->updateLegendAttributes();
}

void PlotCurveDialog::on_remove_clicked()
{
    mRemove = true;
    accept();
}

