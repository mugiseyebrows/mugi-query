#include "schema2arrange.h"

#include "schema2tableitem.h"
#include <QDebug>
#include <QPointF>
#include <math.h>
#include "schema2tablesmodel.h"
#include "schema2tablemodel.h"
#include "schema2relationsmodel.h"
#include <algorithm>

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

    //qDebug() << QString("%1 tables, grid %2..%3 x %2..%3").arg(n).arg(-N).arg(N);

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

    if (related.isEmpty()) {
        related.append({0, 0});
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



static QList<Node> buildNodes(Schema2TablesModel* tablesModel, bool all) {

    QList<Node> nodes;

    //QList<QStringList> keys = relationModels.keys();

    auto tables = tablesModel->tableNames();

    QStringList skip;
    if (!all) {
        auto items = tablesModel->tableItems();
        for(auto* item: items) {
            if (item->grayed()) {
                skip.append(item->tableName().toLower());
            }
        }
    }

    QList<QStringList> relationsList;

    for(const QString& table: tables) {
        auto relations = tablesModel->table(table)->relations()->values();
        for(auto* relation: relations) {
            auto childTable = table.toLower();
            auto parentTable = relation->parentTable().toLower();
            if (skip.contains(childTable) || skip.contains(parentTable)) {
                continue;
            }
            if (childTable != parentTable) {
                relationsList.append({childTable, parentTable});
            }
        }
    }

    for(const QString& table: tables) {
        Node node(table);
        if (skip.contains(table.toLower())) {
            continue;
        }

        for(const QStringList item: relationsList) {
            if (table.toLower() == item[0]) {
                node.connections.append(item[1]);
            } else if (table.toLower() == item[1]) {
                node.connections.append(item[0]);
            }
        }
        nodes.append(node);
    }

    std::sort(nodes.begin(), nodes.end(), lessThan);

    return nodes;
}

static void setPos(QList<Node>& nodes, int nodeIndex, QList<Node>& positioned, int gridIndex, QList<QPointF>& grid) {
    Node node = nodes.takeAt(nodeIndex);
    QPointF pos = grid.takeAt(gridIndex);
    node.pos = pos;
    positioned.append(node);
}



void arrangeTables(GridType type, Schema2TablesModel* tablesModel, bool all) {

    auto tables = tablesModel->tableNames();

    if (tables.isEmpty()) {
        return;
    }

    QList<QPointF> grid = createGrid(type, tables.size());
    QList<Node> nodes = buildNodes(tablesModel, all);
    QList<Node> positioned = {};

    int gridIndex = findCenter(grid);
    int nodeIndex = nodes.size() - 1;
    setPos(nodes, nodeIndex, positioned, gridIndex, grid);

    while(!nodes.isEmpty()) {
        nodeIndex = findMostConnected(nodes, positioned);
        gridIndex = findBestPos(nodes[nodeIndex], positioned, grid);
        setPos(nodes, nodeIndex, positioned, gridIndex, grid);
    }

    for(const Node& node: std::as_const(positioned)) {
        tablesModel->tableItem(node.name)->setCenterPos(node.pos);
    }

}
