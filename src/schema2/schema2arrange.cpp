#include "schema2arrange.h"

#include "schema2tableitem.h"
#include <QDebug>

template<class T>
QList<T> shuffled(const QList<T>& items) {

    QList<QPair<int, int>> order;
    for(int i=0;i<items.size();i++) {
        order.append({i, rand()});
    }
    qSort(order.begin(), order.end(), [](const QPair<int, int>& v1, const QPair<int, int>& v2){
        return v1.second < v2.second;
    });
    QList<T> result;
    for(int i=0;i<items.size();i++) {
        result.append(items[order[i].first]);
    }
    return result;
}

class Item {
public:
    Item(Schema2TableItem* item) : item(item) {
        height = item->boundingRect().height();
    }
    Schema2TableItem* item;
    int height;
};

class Column {
public:
    Column(int heigthLimit, int spacing) : heigthLimit(heigthLimit), spacing(spacing) {

    }

    int height() const {
        if (items.isEmpty()) {
            return 0;
        }
        int tot = 0;
        for(Item item: items) {
            tot += item.height;
        }
        return tot + (items.size() - 1) * spacing;
    }

    QList<int> ys() const {
        int acc = 0;
        QList<int> res;
        for(Item item: items) {
            res.append(acc);
            acc += spacing + item.height;
        }
        return res;
    }

    bool isEmpty() const {
        return items.isEmpty();
    }

    bool append(Schema2TableItem * item) {
        return append(Item(item));
    }

    bool append(const Item& item) {
        if (items.isEmpty()) {
            items.append(item);
            return true;
        }
        if (height() + spacing + item.height > heigthLimit) {
            return false;
        }
        items.append(item);
        return true;
    }

    QList<Item> items;
    int heigthLimit;
    int spacing;
};

class Arragement {
public:
    Arragement() {

    }

    Arragement(const QList<Schema2TableItem *> &items, int heigthLimit, int spacing)
        : heigthLimit(heigthLimit), spacing(spacing)
    {
        Column column(heigthLimit, spacing);
        for(Schema2TableItem * item: items) {
            if (!column.append(item)) {
                columns.append(column);
                column = Column(heigthLimit, spacing);
                column.append(item);
            }
        }
        if (!column.isEmpty()) {
            columns.append(column);
            column = Column(heigthLimit, spacing);
        }
    }

    int invScore() const {
        QList<int> height;
        int total = 0;
        for(int i=0;i<columns.size();i++) {
            height.append(columns[i].height());
            total += columns[i].height();
        }
        int average = total / columns.size();
        int res = 0;
        for(int i=0;i<columns.size();i++) {
            res += abs(height[i] - average);
        }
        return res;
    }

    int columnCount() const {
        return columns.size();
    }

    QList<Column> columns;
    int heigthLimit;
    int spacing;
};


void arrangeInGrid(const QList<Schema2TableItem *> &items, int width, int heigth, int spacing)
{
    Arragement best = Arragement(shuffled(items), heigth, spacing);
    for(int i=0;i<20;i++) {
        Arragement current = Arragement(shuffled(items), heigth, spacing);
        qDebug() << current.invScore() << best.invScore();
        if (current.invScore() < best.invScore()) {
            best = current;
        }
    }
    for(int c=0;c<best.columnCount();c++) {
        Column column = best.columns[c];
        int x = (width + spacing) * c;
        QList<int> ys = column.ys();
        for(int r=0;r<column.items.size();r++) {
            column.items[r].item->setPos(x, ys[r]);
        }
    }
}
