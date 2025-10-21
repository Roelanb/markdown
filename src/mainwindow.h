#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QToolBar>
#include <QMenuBar>
#include <QAction>
#include <QString>

class MarkdownEditor;
class PreviewWidget;
class FileBrowser;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void openFolder();
    void saveFile();
    void saveFileAs();
    void toggleFullScreen();
    void toggleSplitEditor();
    void onFileSelected(const QString &filePath);

private:
    void createMenuBar();
    void createToolBar();
    void createActions();
    void loadFile(const QString &filePath);
    bool saveFileToPath(const QString &filePath);
    void updateEditorLayout();
    void syncEditors();

    // Widgets
    QSplitter *mainSplitter;
    QSplitter *editorSplitter;
    QSplitter *editorViewSplitter;  // For split editor view
    FileBrowser *fileBrowser;
    MarkdownEditor *editor;
    MarkdownEditor *editor2;  // Second editor for split view
    PreviewWidget *preview;

    // State
    bool splitEditorEnabled;

    // Actions
    QAction *openAction;
    QAction *openFolderAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *selectAllAction;
    QAction *fullScreenAction;
    QAction *exitFullScreenAction;
    QAction *splitEditorAction;

    QString currentFilePath;
};

#endif // MAINWINDOW_H
