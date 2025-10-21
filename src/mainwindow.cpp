#include "mainwindow.h"
#include "markdowneditor.h"
#include "previewwidget.h"
#include "filebrowser.h"

#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , currentFilePath("")
    , splitEditorEnabled(false)
{
    setWindowTitle("Markdown Editor");
    resize(1400, 800);

    // Create actions first
    createActions();

    // Create menu bar
    createMenuBar();

    // Create toolbar
    createToolBar();

    // Create the main layout
    mainSplitter = new QSplitter(Qt::Horizontal, this);

    // Create file browser
    fileBrowser = new FileBrowser(this);
    connect(fileBrowser, &FileBrowser::fileSelected, this, &MainWindow::onFileSelected);

    // Create editor/preview splitter
    editorSplitter = new QSplitter(Qt::Horizontal, this);

    // Create editor view splitter (for split editor mode)
    editorViewSplitter = new QSplitter(Qt::Horizontal, this);

    // Create editors
    editor = new MarkdownEditor(this);
    connect(editor, &MarkdownEditor::textChanged, [this]() {
        preview->updatePreview(editor->toPlainText());
        if (splitEditorEnabled && editor2) {
            syncEditors();
        }
    });

    editor2 = new MarkdownEditor(this);
    editor2->setVisible(false);  // Hidden by default
    connect(editor2, &MarkdownEditor::textChanged, [this]() {
        if (splitEditorEnabled) {
            syncEditors();
        }
    });

    // Create preview
    preview = new PreviewWidget(this);

    // Connect scroll synchronization
    connect(editor, &MarkdownEditor::scrollPercentageChanged, 
            preview, &PreviewWidget::scrollToPercentage);

    // Setup initial layout (single editor + preview)
    updateEditorLayout();

    mainSplitter->addWidget(fileBrowser);
    mainSplitter->addWidget(editorSplitter);
    mainSplitter->setStretchFactor(0, 0);
    mainSplitter->setStretchFactor(1, 1);

    // Set sizes
    mainSplitter->setSizes(QList<int>() << 250 << 1150);

    setCentralWidget(mainSplitter);

    // Connect editor actions
    connect(undoAction, &QAction::triggered, editor, &MarkdownEditor::undo);
    connect(redoAction, &QAction::triggered, editor, &MarkdownEditor::redo);
    connect(cutAction, &QAction::triggered, editor, &MarkdownEditor::cut);
    connect(copyAction, &QAction::triggered, editor, &MarkdownEditor::copy);
    connect(pasteAction, &QAction::triggered, editor, &MarkdownEditor::paste);
    connect(selectAllAction, &QAction::triggered, editor, &MarkdownEditor::selectAll);

    // Update action states
    connect(editor, &MarkdownEditor::undoAvailable, undoAction, &QAction::setEnabled);
    connect(editor, &MarkdownEditor::redoAvailable, redoAction, &QAction::setEnabled);
    connect(editor, &MarkdownEditor::copyAvailable, [this](bool available) {
        cutAction->setEnabled(available);
        copyAction->setEnabled(available);
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    openAction = new QAction(tr("&Open"), this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    openFolderAction = new QAction(tr("Open &Folder"), this);
    connect(openFolderAction, &QAction::triggered, this, &MainWindow::openFolder);

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    saveAsAction = new QAction(tr("Save &As..."), this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);

    undoAction = new QAction(tr("&Undo"), this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setEnabled(false);

    redoAction = new QAction(tr("&Redo"), this);
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setEnabled(false);

    cutAction = new QAction(tr("Cu&t"), this);
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setEnabled(false);

    copyAction = new QAction(tr("&Copy"), this);
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setEnabled(false);

    pasteAction = new QAction(tr("&Paste"), this);
    pasteAction->setShortcut(QKeySequence::Paste);

    selectAllAction = new QAction(tr("Select &All"), this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);

    fullScreenAction = new QAction(tr("&Full Screen"), this);
    fullScreenAction->setShortcut(QKeySequence::FullScreen);
    connect(fullScreenAction, &QAction::triggered, this, &MainWindow::toggleFullScreen);

    exitFullScreenAction = new QAction(tr("E&xit Full Screen"), this);
    exitFullScreenAction->setShortcut(Qt::Key_Escape);
    connect(exitFullScreenAction, &QAction::triggered, this, &MainWindow::toggleFullScreen);

    splitEditorAction = new QAction(tr("&Split Editor"), this);
    splitEditorAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Backslash));
    splitEditorAction->setCheckable(true);
    connect(splitEditorAction, &QAction::triggered, this, &MainWindow::toggleSplitEditor);
}

void MainWindow::createMenuBar()
{
    QMenuBar *menuBar = this->menuBar();

    // File menu
    QMenu *fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(openFolderAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    // Edit menu
    QMenu *editMenu = menuBar->addMenu(tr("&Edit"));
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addSeparator();
    editMenu->addAction(selectAllAction);

    // View menu
    QMenu *viewMenu = menuBar->addMenu(tr("&View"));
    viewMenu->addAction(splitEditorAction);
    viewMenu->addSeparator();
    viewMenu->addAction(fullScreenAction);
    viewMenu->addAction(exitFullScreenAction);
}

void MainWindow::createToolBar()
{
    QToolBar *toolBar = addToolBar(tr("Main Toolbar"));
    toolBar->setMovable(false);

    toolBar->addAction(openAction);
    toolBar->addAction(saveAction);
    toolBar->addAction(saveAsAction);
    toolBar->addSeparator();
    toolBar->addAction(undoAction);
    toolBar->addAction(redoAction);
    toolBar->addSeparator();
    toolBar->addAction(cutAction);
    toolBar->addAction(copyAction);
    toolBar->addAction(pasteAction);
    toolBar->addSeparator();
    toolBar->addAction(splitEditorAction);
    toolBar->addAction(fullScreenAction);
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open Markdown File"),
        QString(),
        tr("Markdown Files (*.md *.markdown);;All Files (*)")
    );

    if (!filePath.isEmpty()) {
        loadFile(filePath);
    }
}

void MainWindow::openFolder()
{
    QString dirPath = QFileDialog::getExistingDirectory(
        this,
        tr("Open Folder"),
        QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!dirPath.isEmpty()) {
        fileBrowser->setRootPath(dirPath);
    }
}

void MainWindow::saveFile()
{
    if (currentFilePath.isEmpty()) {
        saveFileAs();
    } else {
        saveFileToPath(currentFilePath);
    }
}

void MainWindow::saveFileAs()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save Markdown File"),
        QString(),
        tr("Markdown Files (*.md *.markdown);;All Files (*)")
    );

    if (!filePath.isEmpty()) {
        if (saveFileToPath(filePath)) {
            currentFilePath = filePath;
            setWindowTitle("Markdown Editor - " + filePath);
        }
    }
}

void MainWindow::toggleFullScreen()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void MainWindow::onFileSelected(const QString &filePath)
{
    loadFile(filePath);
}

void MainWindow::loadFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot read file %1:\n%2")
            .arg(filePath)
            .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    editor->setPlainText(content);
    file.close();

    // Explicitly update the preview
    preview->updatePreview(content);
    
    // Ensure both editor and preview are visible
    editor->setVisible(true);
    preview->setVisible(true);
    
    // Ensure the splitter shows both panes with proper sizes
    QList<int> sizes = editorSplitter->sizes();
    if (sizes.size() == 2 && (sizes[0] == 0 || sizes[1] == 0)) {
        // If either pane is collapsed, reset to 50/50 split
        int totalWidth = editorSplitter->width();
        editorSplitter->setSizes(QList<int>() << totalWidth / 2 << totalWidth / 2);
    }

    currentFilePath = filePath;
    setWindowTitle("Markdown Editor - " + filePath);
}

bool MainWindow::saveFileToPath(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot write file %1:\n%2")
            .arg(filePath)
            .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out << editor->toPlainText();
    file.close();

    return true;
}

void MainWindow::toggleSplitEditor()
{
    splitEditorEnabled = !splitEditorEnabled;
    splitEditorAction->setChecked(splitEditorEnabled);
    updateEditorLayout();

    if (splitEditorEnabled) {
        // Copy content from main editor to second editor
        editor2->setPlainText(editor->toPlainText());
    }
}

void MainWindow::updateEditorLayout()
{
    // Remove widgets by setting their parent to nullptr
    // This automatically removes them from the splitter
    editor->setParent(nullptr);
    editor2->setParent(nullptr);
    preview->setParent(nullptr);
    editorViewSplitter->setParent(nullptr);

    if (splitEditorEnabled) {
        // Split editor mode: editor1 | editor2 | preview
        editorViewSplitter->addWidget(editor);
        editorViewSplitter->addWidget(editor2);
        editorViewSplitter->setStretchFactor(0, 1);
        editorViewSplitter->setStretchFactor(1, 1);

        editorSplitter->addWidget(editorViewSplitter);
        editorSplitter->addWidget(preview);
        editorSplitter->setStretchFactor(0, 2);
        editorSplitter->setStretchFactor(1, 1);

        editor->setVisible(true);
        editor2->setVisible(true);
        
        // Set explicit sizes for split editor mode
        editorSplitter->setSizes(QList<int>() << 800 << 400);
    } else {
        // Normal mode: editor | preview
        editorSplitter->addWidget(editor);
        editorSplitter->addWidget(preview);
        editorSplitter->setStretchFactor(0, 1);
        editorSplitter->setStretchFactor(1, 1);

        editor->setVisible(true);
        editor2->setVisible(false);
        
        // Set explicit sizes for normal mode (50/50 split)
        editorSplitter->setSizes(QList<int>() << 600 << 600);
    }
}

void MainWindow::syncEditors()
{
    // Prevent infinite loop when syncing
    static bool syncing = false;
    if (syncing) return;

    syncing = true;

    // Get the currently focused editor
    QWidget *focusedWidget = QApplication::focusWidget();

    if (focusedWidget == editor) {
        // Update editor2 to match editor
        if (editor2->toPlainText() != editor->toPlainText()) {
            QTextCursor cursor = editor2->textCursor();
            int position = cursor.position();

            editor2->setPlainText(editor->toPlainText());

            // Restore cursor position
            cursor.setPosition(qMin(position, editor2->toPlainText().length()));
            editor2->setTextCursor(cursor);
        }
    } else if (focusedWidget == editor2) {
        // Update editor to match editor2
        if (editor->toPlainText() != editor2->toPlainText()) {
            QTextCursor cursor = editor->textCursor();
            int position = cursor.position();

            editor->setPlainText(editor2->toPlainText());

            // Restore cursor position
            cursor.setPosition(qMin(position, editor->toPlainText().length()));
            editor->setTextCursor(cursor);
        }
    }

    syncing = false;
}
