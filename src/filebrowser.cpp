#include "filebrowser.h"

#include <QVBoxLayout>
#include <QDir>

FileBrowser::FileBrowser(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // Create file system model
    fileSystemModel = new QFileSystemModel(this);
    fileSystemModel->setRootPath(QDir::currentPath());

    // Set filters to show markdown and image files
    QStringList filters;
    filters << "*.md" << "*.markdown" << "*.png" << "*.jpg" << "*.jpeg" 
            << "*.gif" << "*.bmp" << "*.svg" << "*.webp" << "*.ico";
    fileSystemModel->setNameFilters(filters);
    fileSystemModel->setNameFilterDisables(false);

    // Create tree view
    treeView = new QTreeView(this);
    treeView->setModel(fileSystemModel);
    treeView->setRootIndex(fileSystemModel->index(QDir::currentPath()));

    // Hide all columns except name
    treeView->setColumnHidden(1, true);  // Size
    treeView->setColumnHidden(2, true);  // Type
    treeView->setColumnHidden(3, true);  // Date Modified

    // Enable animations
    treeView->setAnimated(true);
    treeView->setIndentation(20);
    treeView->setSortingEnabled(true);
    
    // Enable drag and drop
    treeView->setDragEnabled(true);
    treeView->setDragDropMode(QAbstractItemView::DragOnly);

    // Connect signals
    connect(treeView, &QTreeView::clicked, this, &FileBrowser::onItemClicked);

    layout->addWidget(treeView);
    setLayout(layout);
}

FileBrowser::~FileBrowser()
{
}

void FileBrowser::setRootPath(const QString &path)
{
    fileSystemModel->setRootPath(path);
    treeView->setRootIndex(fileSystemModel->index(path));
}

QString FileBrowser::getRootPath() const
{
    return fileSystemModel->rootPath();
}

void FileBrowser::onItemClicked(const QModelIndex &index)
{
    QString filePath = fileSystemModel->filePath(index);

    // Only emit signal if it's a file (not a directory)
    if (fileSystemModel->isDir(index)) {
        return;
    }

    emit fileSelected(filePath);
}
