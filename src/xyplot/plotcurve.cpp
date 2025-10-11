#include "plotcurve.h"

#include <QwtSymbol>
#include <QDebug>
#include <QwtGraphic>
#include <QApplication>
#include <QStyle>

PlotCurve::PlotCurve(const QString &title, bool addButton)
    : QwtPlotCurve(title), mAddButton(addButton) {

}

bool PlotCurve::addButton() const {
    return mAddButton;
}

QString PlotCurve::x() const {
    return mX;
}

QString PlotCurve::y() const {
    return mY;
}

void PlotCurve::setX(const QString &value) {
    mX = value;
    updateTitle();
}

void PlotCurve::setY(const QString &value) {
    mY = value;
    updateTitle();
}

void PlotCurve::updateTitle() {
    QString title = "untitled";
    if (mX.isEmpty() && !mY.isEmpty()) {
        title = mY;
    } else if (!mX.isEmpty() && !mY.isEmpty()) {
        title = QString("%1(%2)").arg(mY).arg(mX);
    }
    setTitle(title);
}

void PlotCurve::updateLegendAttributes() {
    bool line = style() != NoCurve;
    setLegendAttribute(QwtPlotCurve::LegendShowLine, line);
    setLegendAttribute(QwtPlotCurve::LegendShowSymbol, symbol() != nullptr);
    //setLegendIconSize(QSize(20, 15));
}

#include <QwtText>

#if 0
QList< QwtLegendData > PlotCurve::legendData() const {
    QList< QwtLegendData > data = QwtPlotCurve::legendData();

    if (!mAddButton) {
        return data;
    }

    QwtText label = title();

    qDebug() << label.renderFlags();

    label.setRenderFlags( label.renderFlags() & Qt::AlignRight );
    data[0].setValue( QwtLegendData::TitleRole,
                  QVariant::fromValue( label ) );

    return data;
}
#endif

#if 0
QwtGraphic PlotCurve::drawLegendIcon() const {
    QSize size = legendIconSize();

    const QwtSymbol* symbol = this->symbol();

    double w = size.width();
    double h = size.height();

    QwtGraphic graphic;
    graphic.setDefaultSize( size );
    graphic.setRenderHint( QwtGraphic::RenderPensUnscaled, true );

    QPainter painter(&graphic);
    painter.setRenderHint(QPainter::Antialiasing);

    if (style() != NoCurve) {
        painter.setPen(pen());
        painter.setBrush(Qt::NoBrush);
        painter.drawLine(QPointF(0, h/2), QPointF(w, h/2));
    }
    if (symbol != nullptr) {

        QSize symbolSize = symbol->size();
        if (symbolSize.height() > h) {
            symbolSize = QSize(h, h);
        }

        painter.setPen(symbol->pen());
        painter.setBrush(symbol->brush());

        QwtSymbol symbol1(symbol->style(), symbol->brush(), symbol->pen(), symbolSize);
        symbol1.drawSymbol(&painter, QPointF(w/2, h/2));
    }
    return graphic;
}
#endif

#if 0
QList<QwtLegendData> PlotCurve::legendData() const
{
    QList<QwtLegendData> data = QwtPlotCurve::legendData();
    if (!data.isEmpty()) {
        //QwtGraphic graphic = drawLegendIcon();
        //data[0].setValue(QwtLegendData::IconRole, QVariant::fromValue(graphic));
    }
    return data;
}
#endif

QwtGraphic PlotCurve::legendIcon( int index, const QSizeF& size ) const {

    QwtGraphic icon = QwtPlotCurve::legendIcon(index, QSize(30, 15));
    return icon;
}
