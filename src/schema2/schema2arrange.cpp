#include "schema2arrange.h"

#include "schema2tableitem.h"
#include <QDebug>

#if 0

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

#endif

class Node {
public:
    Node(const QString& name) : name(name) {

    }
    QString name;
    QStringList connections;
    QPointF pos;
};

static bool lessThan(const Node& node1, const Node& node2) {
    return node1.connections.size() < node2.connections.size();
}

#if 0
static bool moreThan(const Node& node1, const Node& node2) {
    return node1.connections.size() > node2.connections.size();
}
#endif

#if 0

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

#endif

#include <QPointF>
#include <math.h>

double dist(const QPointF& p1, const QPointF& p2) {
    QPointF p = p2 - p1;
    return sqrt(p.x()*p.x() + p.y()*p.y());
}

double distSum(const QPointF& pos, const QList<QPointF>& related) {
    double res = 0;
    for(int i=0;i<related.size();i++) {
        res += dist(related[i], pos);
    }
    return res;
}

#if 0

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
#endif


#if 0
QPoint findBestPos(const Node& node, const QList<Node>& finished) {
    QList<QPointF> related;
    for(const Node& other: finished) {
        if (node.connections.contains(other.name)) {
            related.append(other.pos);
        }
    }
    QPointF best = {-3,-3};
    double bestDist = distSum(best, related);
    for(int i=-3;i<=3;i++) {
        for(int j=-3;j<=3;j++) {
            QPointF pos = QPointF(i,j);
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
#endif

QList<QPointF> createGrid(GridType type, int n) {

    QList<QPointF> res;

    if (type == GridSquare) {

        double w = 200 + 40;

        for(int i=-3;i<=3;i++) {
            for(int j=-3;j<=3;j++) {
                res.append(QPointF(i * w, j * w));
            }
        }

    } else if (type == GridTriangle) {

        double w = 300 + 40;

        double h = cos(M_PI / 3) * w;
        double s = w / 2;

        for(int i=-3;i<=3;i++) {
            for(int j=-3;j<=3;j++) {
                res.append(QPointF(i * w + (j % 2) * s, j * h));
            }
        }

    }

    return res;
}

int countConnections(const Node& node, const QList<Node>& positioned) {
    int count = 0;
    for(const Node& other: positioned) {
        if (node.connections.contains(other.name)) {
            count += 1;
        }
    }
    return count;
}

int findMostConnected(QList<Node>& nodes, QList<Node>& positioned) {
    int bestIndex = 0;
    int bestConnections = countConnections(nodes[bestIndex], positioned);
    for(int index=1;index < nodes.size();index++) {
        int connections = countConnections(nodes[index], positioned);
        if (connections >= bestConnections) {
            bestIndex = index;
            bestConnections = connections;
        }
    }
    return bestIndex;
}

int findCenter(const QList<QPointF>& grid) {
    for(int i=0;i<grid.size();i++) {
        if (dist(grid[i],{0,0}) < 10) {
            return i;
        }
    }
    return -1;
}

int findBestPos(const Node& node, const QList<Node>& positioned, const QList<QPointF>& grid) {
    QList<QPointF> related;
    for(const Node& other: positioned) {
        if (node.connections.contains(other.name)) {
            related.append(other.pos);
        }
    }

    int bestIndex = 0;
    double bestDist = distSum(grid[bestIndex], related);

    for(int index=1;index<grid.size();index++) {
        double dist = distSum(grid[index], related);
        if (dist < bestDist) {
            bestIndex = index;
            bestDist = dist;
        }
    }

    return bestIndex;
}

QList<Node> buildNodes(const QStringList tables,
                       const QHash<QStringList, Schema2RelationModel *> &relationModels) {

    QList<Node> nodes;

    QList<QStringList> keys = relationModels.keys();

    for(const QString& table: tables) {
        Node node(table);
        for(const QStringList& key: qAsConst(keys)) {
            if (key[0] == table) {
                node.connections.append(key[1]);
            } else if (key[1] == table) {
                node.connections.append(key[0]);
            }
        }
        nodes.append(node);
    }
    qSort(nodes.begin(), nodes.end(), lessThan);

    return nodes;
}

void setPos(QList<Node>& nodes, int nodeIndex, QList<Node>& positioned, int gridIndex, QList<QPointF>& grid) {
    Node node = nodes.takeAt(nodeIndex);
    QPointF pos = grid.takeAt(gridIndex);
    node.pos = pos;
    positioned.append(node);
}

void arrangeTables(GridType type, const QStringList tables,
             const QHash<QStringList, Schema2RelationModel *> &relationModels,
             const QHash<QString, Schema2TableItem*>& tableItems) {

    if (tables.isEmpty()) {
        return;
    }

    QList<QPointF> grid = createGrid(type, tables.size());

    QList<Node> nodes = buildNodes(tables, relationModels);

    QList<Node> positioned = {};

    int gridIndex = findCenter(grid);

    int nodeIndex = nodes.size() - 1;

    setPos(nodes, nodeIndex, positioned, gridIndex, grid);

    while(!nodes.isEmpty()) {
        nodeIndex = findMostConnected(nodes, positioned);
        gridIndex = findBestPos(nodes[nodeIndex], positioned, grid);
        setPos(nodes, nodeIndex, positioned, gridIndex, grid);
    }

    for(const Node& node: qAsConst(positioned)) {
        tableItems[node.name]->setPos(node.pos);
    }

}

#if 0
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
#endif
