#include "emmet.h"

#include <QHash>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include <QApplication>

#include <QHash>
#include <QJsonDocument>
#include <QDir>
#include <QJsonArray>
#include <QJsonParseError>


static QStringList toStringList(const QJsonArray& arr) {
    QStringList res;
    for(auto it=arr.begin(); it!= arr.end(); it++) {
        QString value = (*it).toString();
        res.append(value);
    }
    return res;
}

static QJsonDocument readJson(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "cannot open" << path;
        return QJsonDocument();
    }
    QByteArray data = file.readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << err.errorString();
    }
    return doc;
}

static QHash<QString, QString> loadEmmetDict() {
    QHash<QString, QString> dict;
    QDir dir(qApp->applicationDirPath());
    if (dir.dirName() == "debug") {
        dir.cdUp();
    }
    QString path = dir.filePath("emmet.json");
    QJsonDocument doc = readJson(path);
    QJsonArray arr = doc.array();
    QStringList keys;
    for(QJsonArray::iterator it=arr.begin(); it!= arr.end(); it++) {
        QJsonValue item = *it;
        if (item.isArray()) {
            keys = toStringList(item.toArray());
        } else {
            QString value = item.toString();
            if (value.startsWith("--")) {
                continue;
            }
            for(const QString& key: keys) {
                QString prev;
                if (dict.contains(key)) {
                    prev = dict[key];
                }
                dict[key] = value;
                if (!prev.isEmpty()) {
                    qDebug() << "redefinition" << key << prev << dict[key];
                }
            }
            keys = QStringList();
        }
    }
    return dict;
}

static QString parseCt(const QString& captured) {
    static QHash<QString, QString> types = {
                                            // numeric
                                            {"i","integer"},
                                            {"si","smallint"},
                                            {"ti","tinyint"},
                                            {"mi","mediumint"},
                                            {"bi","bigint"},
                                            {"de", "decimal"},
                                            {"f", "float"},
                                            {"d", "double"},
                                            {"do", "double"},
                                            // string
                                            {"c", "char"},
                                            {"vc", "varchar"},
                                            {"t", "text"},
                                            // json
                                            {"j", "json"},
                                            // date and time
                                            {"da", "date"},
                                            {"dt", "datetime"},
                                            {"ts", "timestamp"},
                                            // attributes
                                            {"pk", "primary key"},
                                            {"ai", "auto_increment"},
                                            };

    QString tail = captured;
    tail.replace(",","");
    QStringList res;
    while(true) {
        if (tail.isEmpty()) {
            break;
        }
        QString key;
        for(int len=2;len>0;len--) {
            if (types.contains(tail.left(len))) {
                key = tail.left(len);
                break;
            }
        }
        if (key.isEmpty()) {
            qDebug() << "cannot parse as create table" << tail;
            return QString();
        }
        res.append(types[key]);
        tail = tail.right(tail.size() - key.size());
    }
    QStringList columns;
    for(const QString& item: res) {
        if (item == "primary key" || item == "auto_increment") {
            columns[columns.size() - 1] = columns[columns.size() - 1] + " " + item;
        } else {
            columns.append("# " + item);
        }
    }
    return "create table # (" + columns.join(", ") + ")";
}

QString parseEmmet(const QString& text) {

    static QHash<QString, QString> dict = loadEmmetDict();

    QStringList res;
    QString tail = text;
    bool first = true;

    QRegularExpression vn("^v([0-9]+)(a?)(.*)");
    QRegularExpression ct("^ct\\(([a-z,]+)\\)(.*)");

    while(true) {
        QString key;
        auto m = vn.match(tail);
        if (m.hasMatch()) {
            int n = m.captured(1).toInt();
            QString repl = "values ( # )";
            for(int i=1;i<n;i++) {
                repl.append(", ( # )");
            }
            if (m.captured(2) == "a") {
                repl.append(" as #");
            }
            res.append(repl);
            tail = m.captured(3);
            continue;
        }
        m = ct.match(tail);
        if (m.hasMatch()) {
            QString repl = parseCt(m.captured(1));
            if (!repl.isEmpty()) {
                res.append(repl);
                tail = m.captured(2);
                continue;
            }
        }

        for(int size=4;size>0;size--) {
            QString abbr = tail.mid(0, size);
            if (dict.contains(abbr)) {
                key = abbr;
                break;
            }
        }

        if (key.isEmpty()) {
            break;
        } else {
            tail = tail.mid(key.size());
            QString repl = dict[key];
            if (key == "us") {
                if (first) {
                    repl = "update # set #";
                } else {
                    repl = "union select # from #";
                }
            } else if (key == "u") {
                if (first) {
                    repl = "update # set #";
                } else {
                    repl = "union";
                }
            }
            res.append(repl);
        }
        first = false;
        if (key == ";") {
            first = true;
        }
    }

    if (tail.isEmpty()) {
        QString expr = res.join(" ");
        expr.replace(";\n ", ";\n");
        if (expr.endsWith("#")) {
            expr = expr + " ";
        }
        return expr;
    }
    return QString();
}


QTextCursor selectEmmetExpression(const QTextCursor& cursor) {
    QTextCursor begin = cursor;
    QTextCursor startOfLine = cursor;
    startOfLine.movePosition(QTextCursor::StartOfLine);

    while (begin.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor)) {
        if (begin.selectedText().mid(0,1) == " ") {
            begin.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            break;
        }
        if (begin.position() == startOfLine.position()) {
            break;
        }
    }

    begin.setPosition(begin.position());
    begin.setPosition(cursor.position(), QTextCursor::KeepAnchor);

    /*h.movePosition(QTextCursor::StartOfWord);
    h.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
    if (h.selectedText() == ",") {
        h.movePosition(QTextCursor::StartOfWord);
    }
    t.movePosition(QTextCursor::EndOfWord);

    h.setPosition(t.position(), QTextCursor::KeepAnchor);*/

    //qDebug() << begin.selectedText();

    return begin;
}