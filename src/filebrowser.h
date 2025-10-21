#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>

class FileBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit FileBrowser(QWidget *parent = nullptr);
    ~FileBrowser();

    void setRootPath(const QString &path);

signals:
    void fileSelected(const QString &filePath);

private slots:
    void onItemClicked(const QModelIndex &index);

private:
    QTreeView *treeView;
    QFileSystemModel *fileSystemModel;
};

#endif // FILEBROWSER_H
