#include "dataplot.h"
#include "ui_dataplot.h"

#include "modelappender.h"
#include <QStringListModel>
#include <QStandardItemModel>
#include "itemdelegatewithcompleter.h"
#include <QSqlQueryModel>
#include "rowvaluegetter.h"
#include <qwt_plot_curve.h>
#include <QPolygonF>
#include "zipunzip.h"
#include "rowvaluesetter.h"
#include <qwt_symbol.h>
#include <qwt_plot_item.h>
#include "deleteeventfilter.h"
#include "lit.h"
using namespace Lit;

namespace  {

QList<QwtPlotCurve*> curves(QwtPlot* plot) {
    QList<QwtPlotCurve*> result;
    QwtPlotItemList items = plot->itemList();
    foreach (QwtPlotItem* item, items) {
        if (item->rtti() == QwtPlotItem::Rtti_PlotCurve) {
            QwtPlotCurve* curve = static_cast<QwtPlotCurve*>(item);
            result.append(curve);
        }
    }
    return result;
}

QStringList headerData(const QAbstractItemModel* model, Qt::Orientation orientation) {
    QStringList result;
    int count = orientation == Qt::Horizontal ? model->columnCount() : model->rowCount();
    for(int section=0;section<count;section++) {
        result.append(model->headerData(section,orientation).toString());
    }
    return result;
}


QVariantList columnData(const QAbstractItemModel* model,int column) {
    QVariantList result;
    if (column < 0) {
        for(int row = 0; row < model->rowCount(); row++) {
            result << row;
        }
    } else {
        for(int row = 0; row < model->rowCount(); row++) {
            result << model->data(model->index(row,column));
        }
    }
    return result;
}

QList<QPair<QVariant,QVariant> > filterNull(const QList<QPair<QVariant,QVariant> >& data) {
    QList<QPair<QVariant,QVariant> > result;
    for(int i=0;i<data.size();i++) {
        if (data[i].first.isNull() || data[i].second.isNull()) {
            continue;
        }
        result.append(data[i]);
    }
    return result;
}

QPolygonF toPolygon(const QList<QPair<QVariant,QVariant> >& data) {
    QPolygonF result;
    for(int i=0;i<data.size();i++) {
        result.append(QPointF(data[i].first.toDouble(),data[i].second.toDouble()));
    }
    return result;
}

#if 0
bool equal(const QList<QStringList>& vs1, const QList<QStringList>& vs2) {
    if (vs1.size() != vs2.size()) {
        return false;
    }
    for(int i=0;i<vs1.size();i++) {
        if (vs1[i] != vs2[i]) {
            return false;
        }
    }
    return true;
}
#endif

QString getColor(int i) {
    static QStringList colors;
    if (colors.isEmpty()) {
        colors << "red"
               << "green"
               << "blue"
               << "cyan"
               << "magenta"
               << "yellow"
               << "darkRed"
               << "darkGreen"
               << "darkBlue"
               << "darkCyan"
               << "darkMagenta"
               << "darkYellow";
    }
    return colors[i % colors.size()];
}

QMap<QString, Qt::GlobalColor> getColors() {
    static QMap<QString, Qt::GlobalColor> colors;
    if (colors.isEmpty()) {
        colors["black"] = Qt::black;
        colors["white"] = Qt::white;
        colors["darkGray"] = Qt::darkGray;
        colors["gray"] = Qt::gray;
        colors["lightGray"] = Qt::lightGray;
        colors["red"] = Qt::red;
        colors["green"] = Qt::green;
        colors["blue"] = Qt::blue;
        colors["cyan"] = Qt::cyan;
        colors["magenta"] = Qt::magenta;
        colors["yellow"] = Qt::yellow;
        colors["darkRed"] = Qt::darkRed;
        colors["darkGreen"] = Qt::darkGreen;
        colors["darkBlue"] = Qt::darkBlue;
        colors["darkCyan"] = Qt::darkCyan;
        colors["darkMagenta"] = Qt::darkMagenta;
        colors["darkYellow"] = Qt::darkYellow;
        colors["transparent"] = Qt::transparent;
        colors["none"] = Qt::transparent;
    }
    return colors;
}

}

DataPlot::DataPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataPlot),
    mAppender(new ModelAppender())
{
    ui->setupUi(this);
    ui->plot->setAxisAutoScale(QwtPlot::xBottom);
    ui->plot->setAxisAutoScale(QwtPlot::yLeft);
}

DataPlot::~DataPlot()
{
    delete ui;
}

void DataPlot::setDefaultColors() {
    mAppender->setActive(false);
    QAbstractItemModel* model = ui->xy->model();
    for(int row = 0; row < model->rowCount() - 1; row++) {
        qDebug() << row;
        RowValueGetter g(model,row);
        RowValueSetter s(model,row);
        if (g(col_line).isNull()) {
            s(col_line,getColor(row));
        }
        if (g(col_marker).isNull()) {
            s(col_marker,"none");
        }
    }
    mAppender->setActive(true);
}



void DataPlot::setModel(QSqlQueryModel *model)
{
    mModel = model;

    QStandardItemModel* xyModel = new QStandardItemModel(1,cols_size);
    ui->xy->setModel(xyModel);

    QStringList header = sl("X","Y","Line Color","Symbol Color");
    for(int section=0; section<header.size(); section++) {
        xyModel->setHeaderData(section,Qt::Horizontal,header[section]);
    }

    ItemDelegateWithCompleter* xyCompleter = new ItemDelegateWithCompleter(headerData(model,Qt::Horizontal));
    ItemDelegateWithCompleter* colorCompleter = new ItemDelegateWithCompleter(getColors().keys());

    ui->xy->setItemDelegateForColumn(col_x,xyCompleter);
    ui->xy->setItemDelegateForColumn(col_y,xyCompleter);
    ui->xy->setItemDelegateForColumn(col_line,colorCompleter);
    ui->xy->setItemDelegateForColumn(col_marker,colorCompleter);

    DeleteEventFilter* f = new DeleteEventFilter();
    f->setView(ui->xy);

    mAppender->setModel(xyModel);

    setDefaultColors();

    connect(xyModel,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this,SLOT(onModelDataChanged(QModelIndex,QModelIndex,QVector<int>)));
    connect(mAppender,SIGNAL(rowInserted(int)),this,SLOT(setDefaultColors()));
}

void DataPlot::onModelDataChanged(QModelIndex,QModelIndex,QVector<int>) {

    QAbstractItemModel* model = ui->xy->model();

    QStringList header = headerData(mModel,Qt::Horizontal);

    QList<QStringList> xy;
    for(int row=0;row<model->rowCount();row++) {
        RowValueGetter g(model,row);
        QString x = g(col_x).toString();
        QString y = g(col_y).toString();
        QString line = g(col_line).toString();
        QString marker = g(col_marker).toString();
        if (/*x.isEmpty() ||*/ y.isEmpty() /*|| !header.contains(x)*/ || !header.contains(y)) {
            continue;
        }
        xy << sl(x,y,line,marker);
    }
    if (/*!equal(xy,mXy)*/xy != mXy) {

        qDebug() << "xy != mXy";

        QList<QwtPlotCurve*> curves = ::curves(ui->plot);
        int attached = curves.size();
        while(curves.size() < xy.size()) {
            curves.append(new QwtPlotCurve());
        }

        auto colors = getColors();

        for(int i=0;i<xy.size();i++) {
            int x = header.indexOf(xy[i][0]);
            int y = header.indexOf(xy[i][1]);
            Qt::GlobalColor line = colors[xy[i][2]];
            Qt::GlobalColor marker = colors[xy[i][3]];

            QPolygonF polygon = toPolygon(filterNull(zipToPairList(columnData(mModel,x),columnData(mModel,y))));
            curves[i]->setSamples(polygon);

            curves[i]->setStyle(line == Qt::transparent ? QwtPlotCurve::NoCurve : QwtPlotCurve::Lines);
            curves[i]->setPen(line, 2);
            QwtSymbol *symbol = 0;
            if (marker != Qt::transparent) {
                symbol = new QwtSymbol(QwtSymbol::Ellipse, QBrush(marker), QPen(Qt::NoPen), QSize(6, 6));
            }
            curves[i]->setSymbol(symbol);
        }
        for(int i=attached;i<curves.size();i++) {
            curves[i]->attach(ui->plot);
            //curves[i]->setPen(Qt::red, 2);
            curves[i]->setRenderHint(QwtPlotItem::RenderAntialiased, true);

            /*QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
                    QBrush( Qt::red ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );

            curves[i]->setSymbol(symbol);*/
        }

        for(int i=xy.size();i<curves.size();i++) {
            curves[i]->detach();
            delete curves[i];
        }

        ui->plot->replot();
        mXy = xy;
    }

}
