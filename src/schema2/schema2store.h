#ifndef SCHEMA2STORE_H
#define SCHEMA2STORE_H

#include <QObject>

#include <QHash>

class Schema2Store : public QObject
{
    Q_OBJECT
public:

    static Schema2Store* instance(QObject *parent = nullptr);

    static Schema2Store* mInstance;

    QHash<QString, QPointF> loadPos(const QString& connectionName);

    void savePos(const QString& connectionName, const QHash<QString, QPointF>& pos);

protected:
    Schema2Store(QObject *parent = nullptr);

signals:

};

#endif // SCHEMA2STORE_H
