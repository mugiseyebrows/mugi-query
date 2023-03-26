#ifndef SCHEMA2RELATIONGUESSER_H
#define SCHEMA2RELATIONGUESSER_H

#include <QObject>
class Schema2TableModel;

class Schema2RelationGuesser : public QObject
{
    Q_OBJECT
public:
    explicit Schema2RelationGuesser(Schema2TableModel* childTable,
                                    Schema2TableModel* parentTable,
                                    QObject *parent = nullptr);

    QString relationName();

    QStringList parentColumns();

    QStringList childColumns(const QStringList& parentColumns);

protected:
    Schema2TableModel* mParentTable;
    Schema2TableModel* mChildTable;

signals:

};

#endif // SCHEMA2RELATIONGUESSER_H
