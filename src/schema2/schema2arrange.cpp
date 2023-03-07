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

class Node {
public:
    Node(const QString& name) : name(name) {

    }
    QString name;
    QStringList connections;
    QPoint pos;
};

bool moreThan(const Node& node1, const Node& node2) {
    return node1.connections.size() > node2.connections.size();
}

bool containsOne(const Node& node, const QList<Node>& finished) {
    int count = 0;
    for(const Node& other: finished) {
        if (node.connections.contains(other.name)) {
            count += 1;
        }
    }
    return count > 0;
}

bool containsTwo(const Node& node, const QList<Node>& finished) {
    int count = 0;
    for(const Node& other: finished) {
        if (node.connections.contains(other.name)) {
            count += 1;
        }
    }
    return count > 1;
}

#include <QPointF>
#include <math.h>

double dist(const QPoint& p1, const QPoint& p2) {
    QPointF p = QPointF(p2) - QPointF(p1);
    return sqrt(p.x()*p.x() + p.y()*p.y());
}

double distSum(const QPoint& pos, const QList<QPoint>& related) {
    double res = 0;
    for(int i=0;i<related.size();i++) {
        res += dist(related[i], pos);
    }
    return res;
}

bool isOccupied(const QPoint& pos, const QList<Node>& finished) {
    for(const Node& other: finished) {
        if (other.pos == pos) {
            return true;
        }
    }
    return false;
}

double distFromCenter(const QPoint& pos) {
    return dist(pos, QPoint(0,0));
}

QPoint findPosAroundCenter(const Node& node, const QList<Node>& finished) {
    QPoint best = {-3,-3};
    double bestDist = distFromCenter(best);
    for(int i=-3;i<=3;i++) {
        for(int j=-3;j<=3;j++) {
            QPoint pos(i,j);
            double dist = distFromCenter(pos);
            if (dist < bestDist) {
                if (!isOccupied(pos, finished)) {
                    best = pos;
                    bestDist = dist;
                }
            }
        }
    }
    return best;
}

QPoint findBestPos(const Node& node, const QList<Node>& finished) {
    QList<QPoint> related;
    for(const Node& other: finished) {
        if (node.connections.contains(other.name)) {
            related.append(other.pos);
        }
    }
    QPoint best = {-3,-3};
    double bestDist = distSum(best, related);
    for(int i=-3;i<=3;i++) {
        for(int j=-3;j<=3;j++) {
            QPoint pos = QPoint(i,j);
            double dist = distSum(pos, related);
            if (dist < bestDist) {
                if (!isOccupied(pos, finished)) {
                    best = pos;
                    bestDist = dist;
                }
            }
        }
    }
    return best;
}

void squareArrange(const QStringList tables,
                   const QHash<QStringList, Schema2RelationModel *> &relationModels,
                   const QHash<QString, Schema2TableItem*>& tableItems)
{
    QList<QStringList> keys = relationModels.keys();

    QList<Node> nodes;

    for(const QString& table: tables) {
        Node node(table);
        for(const QStringList& key: keys) {
            if (key[0] == table) {
                node.connections.append(key[1]);
            } else if (key[1] == table) {
                node.connections.append(key[0]);
            }
        }
        nodes.append(node);
    }
    qSort(nodes.begin(), nodes.end(), moreThan);

    QList<Node> finished;
    Node center = nodes.takeFirst();
    center.pos = {0,0};
    finished.append(center);

    QList<QPoint> queue = {{1,0}, {0,1}, {-1,0}, {0,-1}};

    int i=0;
    while(i < nodes.size()) {
        if (nodes[i].connections.contains(center.name)) {
            QPoint pos = queue.takeFirst();
            auto node = nodes.takeAt(i);
            node.pos = pos;
            finished.append(node);
            if (queue.isEmpty()) {
                break;
            }
            i--;
        }
        i++;
    }
    i = 0;
    while(i < nodes.size()) {
        if (containsTwo(nodes[i], finished)) {
            QPoint pos = findBestPos(nodes[i], finished);
            auto node = nodes.takeAt(i);
            node.pos = pos;
            finished.append(node);
            if (nodes.isEmpty()) {
                break;
            }
            i = -1;
        }
        i++;
    }
    i = 0;
    while(i < nodes.size()) {
        if (containsOne(nodes[i], finished)) {
            QPoint pos = findBestPos(nodes[i], finished);
            auto node = nodes.takeAt(i);
            node.pos = pos;
            finished.append(node);
            if (nodes.isEmpty()) {
                break;
            }
            i = -1;
        }
        i++;
    }
    while(!nodes.isEmpty()) {
        QPoint pos = findPosAroundCenter(nodes[0], finished);
        auto node = nodes.takeAt(0);
        node.pos = pos;
        finished.append(node);
    }

    int w = 200 + 40;

    for(const Node& node: finished) {

        QPointF p(node.pos);
        p.setX(p.x() * w);
        p.setY(p.y() * w);
        tableItems[node.name]->setPos(p);

    }

    int t = 0;



}
