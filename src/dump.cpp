#include "dump.h"

#include <QFile>
#include <QFileInfo>

void dump(const QString& path, const QStringList& lines) {
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) {
        qDebug() << "!open" << path;
        return;
    }
    QTextStream stream(&f);
    stream.setEncoding(QStringConverter::Utf8);
    foreach(const QString& line, lines) {
        stream << line << "\n";
    }
    qDebug() << "text writen to" << path;
}

QString nextPath(const QString& pattern) {
    int i = 1;
    QString path = pattern.arg(i, 3, 10, QChar('0'));
    while (QFileInfo(path).exists()) {
        i += 1;
        path = pattern.arg(i, 3, 10, QChar('0'));
    }
    return path;
}
