#ifndef ITEMDELEGATEWITHCOMPLETOR_H
#define ITEMDELEGATEWITHCOMPLETOR_H

#include <QObject>
#include <QItemDelegate>
#include "tokens.h"

class ItemDelegateWithCompleter : public QItemDelegate
{
public:
    ItemDelegateWithCompleter(const QStringList& completions,
                              QObject *parent = nullptr);

    QWidget* createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const override;

    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override;

protected:
    QStringList mCompletions;
};

#endif // ITEMDELEGATEWITHCOMPLETOR_H
