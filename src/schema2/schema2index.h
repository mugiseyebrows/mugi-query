#ifndef SCHEMA2INDEX_H
#define SCHEMA2INDEX_H

#include <QStringList>

class Schema2Index
{
public:
    Schema2Index();

    Schema2Index(const QString& name, const QStringList& columns, bool existing);

    QString createQuery(const QString& tableName) const;

    QString dropQuery(const QString& tableName) const;

    QString name() const;

    bool existing() const;

    void setExisting(bool value);

    bool isIndexColumn(const QString& column);

protected:
    QString mName;
    QStringList mColumns;
    QStringList mColumnsLower;
    bool mExisting;
};

#endif // SCHEMA2INDEX_H
