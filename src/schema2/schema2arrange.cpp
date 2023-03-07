#include "schema2arrange.h"

#include "schema2tableitem.h"
#include <QDebug>
#include <QPointF>
#include <math.h>

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

static double dist(const QPointF& p1, const QPointF& p2) {
    QPointF p = p2 - p1;
    return sqrt(p.x()*p.x() + p.y()*p.y());
}

static double distSum(const QPointF& pos, const QList<QPointF>& related) {
    double res = 0;
    for(int i=0;i<related.size();i++) {
        res += dist(related[i], pos);
    }
    return res;
}

static QList<QPointF> createGrid(GridType type, int n) {

    QList<QPointF> res;
    /*
      grid m x m (-N..N x -N..N)
      m = N * 2 + 1     N = (m - 1) / 2
      can fit n tables with fill ratio q (q = 0..1)
      n = m * m * q
      solve for m
      m = sqrt(n / q)
      solve for N
      N = (sqrt(n / q) - 1) / 2
    */

    double q = 0.15;
    int N = (sqrt((double) n / q) - 1.0) / 2.0;
    if (N < 2) {
        N = 2;
    }

    qDebug() << QString("%1 tables, grid %2..%3 x %2..%3").arg(n).arg(-N).arg(N);

    if (type == GridSquare) {

        double w = 200 + 40;

        for(int i=-N;i<=N;i++) {
            for(int j=-N;j<=N;j++) {
                res.append(QPointF(i * w, j * w));
            }
        }

    } else if (type == GridTriangle) {

        double w = 400 + 40;

        double h = cos(M_PI / 3) * w;
        double s = w / 2;

        for(int i=-N;i<=N;i++) {
            for(int j=-N;j<=N;j++) {
                res.append(QPointF(i * w + (j % 2) * s, j * h));
            }
        }

    }

    return res;
}

static int countConnections(const Node& node, const QList<Node>& positioned) {
    int count = 0;
    for(const Node& other: positioned) {
        if (node.connections.contains(other.name)) {
            count += 1;
        }
    }
    return count;
}

static int findMostConnected(QList<Node>& nodes, QList<Node>& positioned) {
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

static int findCenter(const QList<QPointF>& grid) {
    for(int i=0;i<grid.size();i++) {
        if (dist(grid[i],{0,0}) < 10) {
            return i;
        }
    }
    return -1;
}

static int findBestPos(const Node& node, const QList<Node>& positioned, const QList<QPointF>& grid) {
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

static QList<Node> buildNodes(const QStringList tables,
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

static void setPos(QList<Node>& nodes, int nodeIndex, QList<Node>& positioned, int gridIndex, QList<QPointF>& grid) {
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
        tableItems[node.name]->setCenterPos(node.pos);
    }

}
