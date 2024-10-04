#ifndef SCHEMA2TABLEITEM_H
#define SCHEMA2TABLEITEM_H

#include <QGraphicsItem>

class Schema2TableModel;
class Schema2RelationItem2;
#include "uncheckedmode.h"

class Schema2TableItem : public QGraphicsItem
{
public:
    enum { Type = UserType + 1 };

    Schema2TableItem(Schema2TableModel* model, QGraphicsItem *parent = nullptr);

    void addRelation(Schema2RelationItem2 *relation);

    QString tableName() const;

    QString tableNamePrev() const;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setCenterPos(const QPointF& point);

    QPointF centerPos() const;

    void setChecked(bool value);

    bool checked() const {
        return mChecked;
    }

    Schema2TableModel* model() const {
        return mModel;
    }

    UncheckedMode uncheckedMode() const {
        return mMode;
    }

    void setUncheckedMode(UncheckedMode mode);

#if 0
    bool isIndexColumn(const QString& column) const;
#endif

    void removeRelation(Schema2RelationItem2 *relation);
protected:
    Schema2TableModel* mModel;
    QList<Schema2RelationItem2*> mRelations;
    bool mChecked;
    UncheckedMode mMode = UncheckedGrayed;

    // QGraphicsItem interface
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    // QGraphicsItem interface
public:
    int type() const override;
};

#endif // SCHEMA2TABLEITEM_H
