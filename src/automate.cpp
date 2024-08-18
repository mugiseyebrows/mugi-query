#include "automate.h"

#include "automation.h"
#include <QWidget>
#include <QApplication>
#include "mainwindow.h"
#include <QSqlQuery>
#include <QSqlError>

static MainWindow* mainWindow() {
    auto windows = qApp->topLevelWidgets();
    for(auto* window: windows) {
        if (MainWindow* mainWindow = qobject_cast<MainWindow*>(window)) {
            return mainWindow;
        }
    }
    return 0;
}

void automate(QWidget *widget)
{
#if 0
    // debug xjoin
    Automation* automation = Automation::instance(widget);
    automation->connectToDatabaseFromHistory("foo");
    automation->connectToDatabaseFromHistory("bar");
    automation->toolXJoin("foo", "bar");
    automation->start();
#endif

#if 0
    Automation* automation = Automation::instance(widget);
    //QString database = 0 ? "testaccess" : "test2";
    //QString database = "mysql1";
    QString database = "mysql1";
    automation->connectToDatabaseFromHistory(database);
    automation->schemaEdit();

    //automation->showJoinHelper();
    automation->start();
#endif

#if 0
    Automation* automation = Automation::instance(widget);
    QString database = "sample";
    automation->connectToDatabaseFromHistory(database);
    automation->schemaEdit();

    QStringList create = QString("CREATE TABLE `category` (`category_id` int(11) NOT NULL PRIMARY KEY, `name` varchar(25) NOT NULL, `last_update` timestamp DEFAULT (current_timestamp())); "
                                 "CREATE TABLE `film` (`film_id` int(11) NOT NULL PRIMARY KEY, `title` varchar(255) NOT NULL, `description` text, `release_year` int(11), `language_id` int(11) NOT NULL, `rental_duration` smallint(6) NOT NULL DEFAULT (3), `rental_rate` decimal(4,2) NOT NULL DEFAULT (4.99), `length` smallint(6), `replacement_cost` decimal(5,2) NOT NULL DEFAULT (19.99), `rating` enum('G','PG','PG-13','R','NC-17') DEFAULT 'G', `last_update` timestamp DEFAULT (current_timestamp()), `special_features` longtext, `fulltext_` text NOT NULL); "
                                 "CREATE TABLE `film_category` (`film_id` int(11) NOT NULL, `category_id` int(11) NOT NULL, `last_update` timestamp DEFAULT (current_timestamp()))").split(";");

    QStringList relate = QString("ALTER TABLE `film_category` ADD CONSTRAINT `film_category_category_id_fkey` FOREIGN KEY (`category_id`) REFERENCES `category`(`category_id`); "
                                 "ALTER TABLE `film_category` ADD CONSTRAINT `film_category_film_id_fkey` FOREIGN KEY (`film_id`) REFERENCES `film`(`film_id`) ").split(";");

    QStringList alter = QString("ALTER TABLE `category` DROP COLUMN `last_update`; "
                                "ALTER TABLE `category` CHANGE COLUMN `name` `name` text NOT NULL; "
                                "ALTER TABLE `category` ADD COLUMN `description` varchar(10) NOT NULL DEFAULT 'x' ").split(";");

    QStringList drop = QString("drop table film_category; drop table category; drop table film").split(";");

    Function fn1("create", [=](){

        QSqlQuery q(QSqlDatabase::database("sample"));
        for(const QString& query: create) {
            if (!q.exec(query)) {
                qDebug() << q.lastError().text();
            }
        }

        automation->onPullSchema();
        mainWindow()->expandSchemaTree();

    });
    Function fn2("relate", [=](){

        QSqlQuery q(QSqlDatabase::database("sample"));
        for(const QString& query: relate) {
            if (!q.exec(query)) {
                qDebug() << q.lastError().text();
            }
        }
        mainWindow()->expandSchemaTree();
        automation->onPullSchema();

    });

    Function fn3("alter columns", [=](){

        QSqlQuery q(QSqlDatabase::database("sample"));
        for(const QString& query: alter) {
            if (!q.exec(query)) {
                qDebug() << q.lastError().text();
            }
        }
        mainWindow()->expandSchemaTree();
        automation->onPullSchema();


    });

    Function fn4("drop", [=](){
        QSqlQuery q(QSqlDatabase::database("sample"));
        for(const QString& query: drop) {
            if (!q.exec(query)) {
                qDebug() << q.lastError().text();
            }
        }
        automation->onPullSchema();
    });

    automation->runSteps({fn1, fn2, fn3, fn4});

    automation->start();
#endif


#if 0
    Automation* automation = Automation::instance(widget);
    //QString database = 0 ? "testaccess" : "test2";
    QString database = "mysql1";
    automation->connectToDatabaseFromHistory(database);
    automation->schemaEdit();
    automation->createTable("foo", {{"foo_id", "integer"}, {"b", "integer"}});
    automation->createPrimaryKey("pk_foo", "foo", {"foo_id"});
    automation->createTable("bar", {{"bar_id", "integer"}, {"foo_id", "integer"}});
    automation->createPrimaryKey("pk_bar", "bar", {"bar_id"});
    automation->createRelation("fk_foo_bar", "bar", {"foo_id"}, "foo", {"foo_id"}, true);
    automation->pushSchema();

    automation->start();
#endif

#if 0

    Automation* automation = Automation::instance(widget);
    QString database = "mysql1";
    automation->connectToDatabaseFromHistory(database);
    automation->schemaEdit();
    automation->selectTablesNone();
    automation->selectTables({"foo"});

    QList<int> formats = {0, 1, 2, 3};
    QList<bool> cropAlls = {true, false};
    QList<bool> itemsAlls = {true, false};

    for(int format: formats) {
        for(bool cropAll: cropAlls) {
            for(bool itemsAll: itemsAlls) {
                bool clipboard = false;

                QString path = QString("D:\\w\\export-%1-crop-%2-items-%3")
                        .arg(format)
                        .arg(cropAll ? 1 : 0)
                        .arg(itemsAll ? 1 : 0);
                
                automation->exportTo(format, cropAll, itemsAll, clipboard, path);
            }
        }
    }
    /*int format = 0;
    bool cropAll = true;
    bool itemsAll = true;*/
    automation->start();
#endif

}
