#ifndef ITEMDELEGATEWITHCOMPLETOR_H
#define ITEMDELEGATEWITHCOMPLETOR_H

#include <QObject>
#include <QItemDelegate>
#include "tokens.h"

class ItemDelegateWithCompleter : public QItemDelegate
{
public:
    ItemDelegateWithCompleter(const QStringList& completions,
                              QObject *parent = Q_NULLPTR);

    QWidget* createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const Q_DECL_OVERRIDE;

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const Q_DECL_OVERRIDE;

    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const Q_DECL_OVERRIDE;


protected:
    QStringList mCompletions;
};

#endif // ITEMDELEGATEWITHCOMPLETOR_H
