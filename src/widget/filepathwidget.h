#ifndef FILEPATHWIDGET_H
#define FILEPATHWIDGET_H

#include <QWidget>

namespace Ui {
class FilePathWidget;
}

class FilePathWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FilePathWidget(QWidget *parent = nullptr);
    ~FilePathWidget();

    bool clipboard() const;

    QString path() const;

    void init(const QString& path, const QString& name, const QString& ext, bool overwrite);

    bool overwrite() const;

    bool outputToFile() const;
    void setPath(const QString &path);

    void setName(const QString& name);

    void setExt(const QString &ext);
protected:
    QString mExt;

private slots:
    void on_selectDirectory_clicked();

    void on_selectFile_clicked();

private:
    Ui::FilePathWidget *ui;
};

#endif // FILEPATHWIDGET_H
