#include "schema2export.h"

#include <QSvgGenerator>

#include "schema2tableitem.h"
#include "schema2relation.h"
#include "schema2tablemodel.h"
#include "schema2tablesmodel.h"
#include "schema2relationsmodel.h"
#include "schema2view.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QFile>
#include <QMessageBox>

static QString dotQuoted(const QString& name) {
    if (name.contains(" ") || name.contains("-")) {
        return "\"" + name + "\"";
    }
    return name;
}

static QString dotTableRow(const QString& cell, bool bold = false) {
    return QString("<tr><td>%1</td></tr>").arg(bold ? "<b>" + cell + "</b>" : cell);
}

static QString dotTable(Schema2TableItem * table, double posk) {

    QStringList columns;
    auto names = table->model()->columnNames();
    for(const QString& name: names) {
        columns.append(dotTableRow(name));
    }

    QString label = QString("<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\">\n"
                    "%1\n"
                    "<tr><td>\n"
                    "<table border=\"0\" cellborder=\"0\" cellspacing=\"0\" >\n"
                    "%2\n"
                    "</table>\n"
                    "</td></tr>\n"
                    "</table>>")
            .arg(dotTableRow(table->tableName(), true))
            .arg(columns.join("\n"));

    QPointF pos = table->centerPos() * posk;

    return QString("%1 [label=%2\npos=\"%3,%4!\"]\n").arg(dotQuoted(table->tableName())).arg(label).arg(pos.x()).arg(-pos.y());
}

static QString dotRelation(Schema2Relation* relation) {
    auto childTable = relation->childTable();
    auto parentTable = relation->parentTable();
    return QString("%1 -> %2\n")
            .arg(dotQuoted(childTable))
            .arg(dotQuoted(parentTable));
}

#include <QApplication>
#include <QClipboard>

static void outputText(bool clipboard, const QString& path, const QString& text, QWidget* widget) {
    if (clipboard) {
        QApplication::clipboard()->setText(text);
        QMessageBox::information(widget, "Success", "Saved to clipbard");
    } else {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(widget, "Error", QString("Cannot open %1 for writing").arg(path));
            return;
        }
        file.write(text.toUtf8());
    }
}

Schema2Export::Schema2Export(Schema2View *view)
    : mView(view)
{

}

void Schema2Export::saveAs(bool clipboard, const QString &path, const QRectF& rect,
                           bool onlySelected, ExportFormat format, QWidget *widget)
{
    if (format == PngFormat) {

        if (clipboard) {
            QMessageBox::critical(widget, "Error", "Not implemented");
            return;
        }

        //QRectF rect = mView->sceneRect();

        QImage image(rect.toRect().size(), QImage::Format_ARGB32);
        image.fill(Qt::white);

        QPainter painter;
        painter.begin(&image);

        QPointF p = rect.topLeft();
        QTransform transform = QTransform().translate(-p.x(), -p.y());
        painter.setTransform(transform);

        auto* mScene = mView->scene();

        mScene->render(&painter, rect);
        painter.end();

        if (clipboard) {

        } else {
            image.save(path);
        }


    } else if (format == SvgFormat) {

        if (clipboard) {
            QMessageBox::critical(widget, "Error", "Not implemented");
            return;
        }

        QSvgGenerator generator;

        //auto rect = mView->sceneRect();

        generator.setFileName(path);
        generator.setSize(rect.size().toSize());
        generator.setViewBox(rect);
        QPainter painter;
        painter.begin(&generator);
        auto* scene = mView->scene();
        scene->render(&painter, rect);
        painter.end();

    } else if (format == DotFormat) {

        QStringList res;
        double posk = 0.01;

        auto* mTables = mView->tables();

        auto tables = mTables->tableItems();
        for(auto* table: tables) {
            if (table->checked() && onlySelected) {
                continue;
            }
            res.append(dotTable(table, posk));
        }

        for(auto* table: tables) {
            auto relations = table->model()->relations()->values();
            for(auto* relation: relations) {
                auto* childTable = mTables->tableItem(relation->parentTable());
                auto* parentTable = mTables->tableItem(relation->childTable());
                if (childTable->checked() && onlySelected) {
                    continue;
                }
                if (parentTable->checked() && onlySelected) {
                    continue;
                }
                res.append(dotRelation(relation));
            }
        }

        QString text = QString("digraph G {\nnode [shape=plain]\n%1\n}").arg(res.join("\n"));

#if 0
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(widget, "Error", QString("Cannot open %1 for writing").arg(path));
            return;
        }


        file.write(text.toUtf8());
#endif
        outputText(clipboard, path, text, widget);

    } else if (format == DbioFormat) {

        QStringList tablesText;
        QStringList relationsText;

        auto* mTables = mView->tables();

        auto tables = mTables->tableItems();

        for(auto* table: tables) {

            if (table->checked() && onlySelected) {
                continue;
            }

            auto relations = table->model()->relations()->values();

            for(Schema2Relation* relation: relations) {
                auto* childTable = mTables->tableItem(relation->parentTable());
                auto* parentTable = mTables->tableItem(relation->childTable());
                if (childTable->checked() && onlySelected) {
                    continue;
                }
                if (parentTable->checked() && onlySelected) {
                    continue;
                }
                QString item = QString("ref {\n    %1.%2 > %3.%4\n}")
                        .arg(relation->childTable())
                        .arg(relation->childColumns().join(""))
                        .arg(relation->parentTable())
                        .arg(relation->parentColumns().join(""));
                relationsText.append(item);
            }

            Schema2TableModel* tableModel = table->model();

            QStringList fields;
            for(int row=0;row<tableModel->rowCount();row++) {
                QString type = tableModel->type(row);
                type.replace(" unsigned", "");
                QString field = QString("    %1 %2")
                        .arg(tableModel->name(row))
                        .arg(type);
                fields.append(field);
            }
            QString item = QString("table %1\n{\n%2\n}")
                    .arg(table->tableName())
                    .arg(fields.join("\n"));
            tablesText.append(item);
        }

#if 0
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(widget, "Error", QString("Cannot open %1 for writing").arg(path));
            return;
        }
        file.write((tablesText.join("\n") + "\n").toUtf8());
        file.write((relationsText.join("\n") + "\n").toUtf8());
#endif

        QString text = tablesText.join("\n") + "\n" + relationsText.join("\n") + "\n";
        outputText(clipboard, path, text, widget);

    }
}
