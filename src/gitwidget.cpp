#include "gitwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QHeaderView>
#include <QScrollBar>

GitWidget::GitWidget(QWidget *parent)
    : QWidget(parent)
    , gitProcess(nullptr)
    , autoRefreshTimer(nullptr)
{
    setupUI();
    
    // Auto-refresh every 2 seconds
    autoRefreshTimer = new QTimer(this);
    connect(autoRefreshTimer, &QTimer::timeout, this, &GitWidget::autoRefresh);
    autoRefreshTimer->start(2000);
}

GitWidget::~GitWidget()
{
    if (gitProcess) {
        gitProcess->kill();
        gitProcess->waitForFinished();
        delete gitProcess;
    }
}

void GitWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Title and working directory
    QLabel *titleLabel = new QLabel("<b>Source Control</b>", this);
    mainLayout->addWidget(titleLabel);

    workingDirLabel = new QLabel("Working Dir: <i>Not set</i>", this);
    workingDirLabel->setWordWrap(true);
    workingDirLabel->setStyleSheet("QLabel { color: #666; font-size: 10px; padding: 5px; }");
    mainLayout->addWidget(workingDirLabel);

    // Commit message section (like VS Code - at the top)
    commitMessageEdit = new QTextEdit(this);
    commitMessageEdit->setPlaceholderText("Message (Ctrl+Enter to commit)");
    commitMessageEdit->setMaximumHeight(80);
    commitMessageEdit->setMinimumHeight(60);
    mainLayout->addWidget(commitMessageEdit);

    // Commit and action buttons
    QHBoxLayout *actionLayout = new QHBoxLayout();
    
    commitButton = new QPushButton("✓ Commit", this);
    commitButton->setToolTip("Commit staged changes");
    connect(commitButton, &QPushButton::clicked, this, &GitWidget::commitChanges);
    actionLayout->addWidget(commitButton);
    
    refreshButton = new QPushButton("↻", this);
    refreshButton->setToolTip("Refresh");
    refreshButton->setMaximumWidth(40);
    connect(refreshButton, &QPushButton::clicked, this, &GitWidget::refreshStatus);
    actionLayout->addWidget(refreshButton);
    
    pullButton = new QPushButton("↓ Pull", this);
    pullButton->setToolTip("Pull from remote");
    connect(pullButton, &QPushButton::clicked, this, &GitWidget::pullChanges);
    actionLayout->addWidget(pullButton);
    
    pushButton = new QPushButton("↑ Push", this);
    pushButton->setToolTip("Push to remote");
    connect(pushButton, &QPushButton::clicked, this, &GitWidget::pushChanges);
    actionLayout->addWidget(pushButton);
    
    mainLayout->addLayout(actionLayout);

    // Changes section (unstaged)
    QLabel *changesLabel = new QLabel("<b>Changes</b>", this);
    mainLayout->addWidget(changesLabel);
    
    changesTree = new QTreeWidget(this);
    changesTree->setHeaderHidden(true);
    changesTree->setRootIsDecorated(false);
    changesTree->setIndentation(10);
    changesTree->setMaximumHeight(200);
    connect(changesTree, &QTreeWidget::itemDoubleClicked, this, &GitWidget::onItemDoubleClicked);
    mainLayout->addWidget(changesTree);
    
    QHBoxLayout *changesActions = new QHBoxLayout();
    QPushButton *stageAllBtn = new QPushButton("+ Stage All", this);
    connect(stageAllBtn, &QPushButton::clicked, this, &GitWidget::stageAll);
    changesActions->addWidget(stageAllBtn);
    changesActions->addStretch();
    mainLayout->addLayout(changesActions);

    // Staged changes section
    QLabel *stagedLabel = new QLabel("<b>Staged Changes</b>", this);
    mainLayout->addWidget(stagedLabel);
    
    stagedTree = new QTreeWidget(this);
    stagedTree->setHeaderHidden(true);
    stagedTree->setRootIsDecorated(false);
    stagedTree->setIndentation(10);
    stagedTree->setMaximumHeight(200);
    connect(stagedTree, &QTreeWidget::itemDoubleClicked, this, &GitWidget::onItemDoubleClicked);
    mainLayout->addWidget(stagedTree);
    
    QHBoxLayout *stagedActions = new QHBoxLayout();
    QPushButton *unstageAllBtn = new QPushButton("- Unstage All", this);
    connect(unstageAllBtn, &QPushButton::clicked, this, &GitWidget::unstageAll);
    stagedActions->addWidget(unstageAllBtn);
    stagedActions->addStretch();
    mainLayout->addLayout(stagedActions);

    mainLayout->addStretch();
    setLayout(mainLayout);
}

void GitWidget::setWorkingDirectory(const QString &path)
{
    QFileInfo fileInfo(path);
    QString startPath;
    if (fileInfo.isFile()) {
        startPath = fileInfo.absolutePath();
    } else {
        startPath = path;
    }
    
    // Find the git repository root
    QDir dir(startPath);
    bool foundGit = false;
    
    // Check current directory and parent directories
    do {
        if (dir.exists(".git")) {
            workingDirectory = dir.absolutePath();
            foundGit = true;
            break;
        }
    } while (dir.cdUp());
    
    if (foundGit) {
        workingDirLabel->setText("Working Dir: <b>" + workingDirectory + "</b>");
        workingDirLabel->setStyleSheet("QLabel { color: green; font-size: 10px; padding: 5px; }");
        refreshStatus();
    } else {
        workingDirectory = startPath;
        workingDirLabel->setText("Working Dir: <b>" + startPath + "</b> <i>(Not a git repo)</i>");
        workingDirLabel->setStyleSheet("QLabel { color: red; font-size: 10px; padding: 5px; }");
    }
}

void GitWidget::runGitCommand(const QStringList &arguments)
{
    if (workingDirectory.isEmpty()) {
        return;
    }

    if (gitProcess) {
        if (gitProcess->state() == QProcess::Running) {
            return;
        }
        delete gitProcess;
    }

    gitProcess = new QProcess(this);
    gitProcess->setWorkingDirectory(workingDirectory);
    
    connect(gitProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &GitWidget::onProcessFinished);
    connect(gitProcess, &QProcess::errorOccurred,
            this, &GitWidget::onProcessError);

    currentCommand = "git " + arguments.join(" ");
    gitProcess->start("git", arguments);
}

void GitWidget::refreshStatus()
{
    runGitCommand(QStringList() << "status" << "--porcelain");
}

void GitWidget::autoRefresh()
{
    if (!workingDirectory.isEmpty() && (!gitProcess || gitProcess->state() != QProcess::Running)) {
        refreshStatus();
    }
}

void GitWidget::stageFile(QTreeWidgetItem *item)
{
    if (!item) return;
    QString fileName = item->data(0, Qt::UserRole).toString();
    if (!fileName.isEmpty()) {
        runGitCommand(QStringList() << "add" << fileName);
    }
}

void GitWidget::unstageFile(QTreeWidgetItem *item)
{
    if (!item) return;
    QString fileName = item->data(0, Qt::UserRole).toString();
    if (!fileName.isEmpty()) {
        runGitCommand(QStringList() << "reset" << "HEAD" << fileName);
    }
}

void GitWidget::stageAll()
{
    runGitCommand(QStringList() << "add" << "-A");
}

void GitWidget::unstageAll()
{
    runGitCommand(QStringList() << "reset" << "HEAD");
}

void GitWidget::commitChanges()
{
    QString message = commitMessageEdit->toPlainText().trimmed();
    
    if (message.isEmpty()) {
        QMessageBox::warning(this, "Commit Error", "Please enter a commit message");
        return;
    }

    runGitCommand(QStringList() << "commit" << "-m" << message);
}

void GitWidget::pushChanges()
{
    runGitCommand(QStringList() << "push");
}

void GitWidget::pullChanges()
{
    runGitCommand(QStringList() << "pull");
}

void GitWidget::syncChanges()
{
    // Pull then push
    runGitCommand(QStringList() << "pull" << "--rebase");
}

void GitWidget::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    
    if (!item) return;
    
    // Check which tree the item belongs to
    if (item->treeWidget() == changesTree) {
        stageFile(item);
    } else if (item->treeWidget() == stagedTree) {
        unstageFile(item);
    }
}

void GitWidget::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        QString output = gitProcess->readAllStandardOutput();
        
        if (currentCommand.contains("status") || currentCommand.contains("porcelain")) {
            updateFileList(output);
        } else if (currentCommand.contains("commit")) {
            commitMessageEdit->clear();
            refreshStatus();
        } else if (currentCommand.contains("add") || currentCommand.contains("reset")) {
            refreshStatus();
        } else if (currentCommand.contains("pull") || currentCommand.contains("push")) {
            refreshStatus();
        }
    }
}

void GitWidget::onProcessError(QProcess::ProcessError error)
{
    Q_UNUSED(error);
    // Silently ignore errors for auto-refresh
}

void GitWidget::updateFileList(const QString &statusOutput)
{
    changesTree->clear();
    stagedTree->clear();
    
    QStringList lines = statusOutput.split('\n', Qt::SkipEmptyParts);
    
    for (const QString &line : lines) {
        if (line.length() < 3) continue;
        
        QString statusCode = line.left(2);
        QString fileName = line.mid(3);
        
        // Parse git status codes
        QChar indexStatus = statusCode[0];
        QChar workTreeStatus = statusCode[1];
        
        QString displayText;
        QString icon;
        
        // Determine file status
        if (indexStatus == 'M' || indexStatus == 'A' || indexStatus == 'D' || indexStatus == 'R' || indexStatus == 'C') {
            // File is staged
            if (indexStatus == 'M') {
                icon = "M";
                displayText = fileName + " (Modified)";
            } else if (indexStatus == 'A') {
                icon = "A";
                displayText = fileName + " (Added)";
            } else if (indexStatus == 'D') {
                icon = "D";
                displayText = fileName + " (Deleted)";
            } else if (indexStatus == 'R') {
                icon = "R";
                displayText = fileName + " (Renamed)";
            } else if (indexStatus == 'C') {
                icon = "C";
                displayText = fileName + " (Copied)";
            }
            
            QTreeWidgetItem *item = new QTreeWidgetItem(stagedTree);
            item->setText(0, icon + "  " + displayText);
            item->setData(0, Qt::UserRole, fileName);
            item->setToolTip(0, "Double-click to unstage");
        }
        
        if (workTreeStatus == 'M' || workTreeStatus == 'D' || workTreeStatus == '?') {
            // File has unstaged changes
            if (workTreeStatus == 'M') {
                icon = "M";
                displayText = fileName + " (Modified)";
            } else if (workTreeStatus == 'D') {
                icon = "D";
                displayText = fileName + " (Deleted)";
            } else if (workTreeStatus == '?') {
                icon = "U";
                displayText = fileName + " (Untracked)";
            }
            
            QTreeWidgetItem *item = new QTreeWidgetItem(changesTree);
            item->setText(0, icon + "  " + displayText);
            item->setData(0, Qt::UserRole, fileName);
            item->setToolTip(0, "Double-click to stage");
        }
    }
    
    // Update labels with counts
    int changesCount = changesTree->topLevelItemCount();
    int stagedCount = stagedTree->topLevelItemCount();
    
    if (changesCount == 0) {
        QTreeWidgetItem *item = new QTreeWidgetItem(changesTree);
        item->setText(0, "No changes");
        item->setDisabled(true);
    }
    
    if (stagedCount == 0) {
        QTreeWidgetItem *item = new QTreeWidgetItem(stagedTree);
        item->setText(0, "No staged changes");
        item->setDisabled(true);
    }
}

void GitWidget::appendOutput(const QString &text, bool isError)
{
    Q_UNUSED(text);
    Q_UNUSED(isError);
    // Removed output display for cleaner VS Code-like interface
}
