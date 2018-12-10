#ifndef JOINHELPERWIDGET_H
#define JOINHELPERWIDGET_H

#include <QWidget>
#include "tokens.h"

class ModelAppender;

namespace Ui {
class JoinHelperWidget;
}

class JoinHelperWidget : public QWidget
{
    Q_OBJECT

public:
    explicit JoinHelperWidget(QWidget *parent = nullptr);
    ~JoinHelperWidget() override;

    void init(const QString &connectionName);
    void init(const Tokens &tokens);

    void closeEvent(QCloseEvent *event) override;

protected:
    QString filePath() const;
    void findPath();
    void saveRelationsModel();
    void loadRelationsModel();
signals:
    void appendQuery(QString);

private slots:

    void onAdjustSplitters();
    void on_add_clicked();
    void on_remove_clicked();
    void on_filter_textChanged(const QString &arg1);
    void on_allTables_doubleClicked(const QModelIndex &index);
    void on_selectedTables_doubleClicked(const QModelIndex &index);

    void on_copy_clicked();

private:
    Ui::JoinHelperWidget *ui;
    QString mConnectionName;
    Tokens mTokens;
    ModelAppender* mAppender;
};

#endif // JOINHELPERWIDGET_H
