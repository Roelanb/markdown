#include "gitwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

GitWidget::GitWidget(QWidget *parent)
    : QWidget(parent)
    , gitProcess(nullptr)
{
    setupUI();
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

    // Title
    QLabel *titleLabel = new QLabel("<b>Git Operations</b>", this);
    mainLayout->addWidget(titleLabel);

    // Working directory display
    workingDirLabel = new QLabel("Working Dir: <i>Not set</i>", this);
    workingDirLabel->setWordWrap(true);
    workingDirLabel->setStyleSheet("QLabel { color: #666; font-size: 10px; padding: 5px; }");
    mainLayout->addWidget(workingDirLabel);

    // File list section
    QGroupBox *fileGroup = new QGroupBox("Changed Files", this);
    QVBoxLayout *fileLayout = new QVBoxLayout(fileGroup);
    
    fileListWidget = new QListWidget(this);
    fileListWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    fileLayout->addWidget(fileListWidget);
    
    mainLayout->addWidget(fileGroup);

    // Commit message section
    QGroupBox *commitGroup = new QGroupBox("Commit Message", this);
    QVBoxLayout *commitLayout = new QVBoxLayout(commitGroup);
    
    commitMessageEdit = new QLineEdit(this);
    commitMessageEdit->setPlaceholderText("Enter commit message...");
    commitLayout->addWidget(commitMessageEdit);
    
    mainLayout->addWidget(commitGroup);

    // Buttons section
    QGroupBox *buttonGroup = new QGroupBox("Git Commands", this);
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonGroup);

    // First row: Status and Diff
    QHBoxLayout *row1 = new QHBoxLayout();
    statusButton = new QPushButton("Status", this);
    diffButton = new QPushButton("Diff", this);
    connect(statusButton, &QPushButton::clicked, this, &GitWidget::runGitStatus);
    connect(diffButton, &QPushButton::clicked, this, &GitWidget::runGitDiff);
    row1->addWidget(statusButton);
    row1->addWidget(diffButton);
    buttonLayout->addLayout(row1);

    // Second row: Add and Commit
    QHBoxLayout *row2 = new QHBoxLayout();
    addButton = new QPushButton("Add Selected", this);
    commitButton = new QPushButton("Commit", this);
    connect(addButton, &QPushButton::clicked, this, &GitWidget::runGitAdd);
    connect(commitButton, &QPushButton::clicked, this, &GitWidget::runGitCommit);
    row2->addWidget(addButton);
    row2->addWidget(commitButton);
    buttonLayout->addLayout(row2);

    // Third row: Push and Pull
    QHBoxLayout *row3 = new QHBoxLayout();
    pushButton = new QPushButton("Push", this);
    pullButton = new QPushButton("Pull", this);
    connect(pushButton, &QPushButton::clicked, this, &GitWidget::runGitPush);
    connect(pullButton, &QPushButton::clicked, this, &GitWidget::runGitPull);
    row3->addWidget(pushButton);
    row3->addWidget(pullButton);
    buttonLayout->addLayout(row3);

    mainLayout->addWidget(buttonGroup);

    // Output display
    QGroupBox *outputGroup = new QGroupBox("Output", this);
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);
    
    outputDisplay = new QTextEdit(this);
    outputDisplay->setReadOnly(true);
    outputDisplay->setMaximumHeight(200);
    outputLayout->addWidget(outputDisplay);
    
    mainLayout->addWidget(outputGroup);

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
        appendOutput("Git repository found: " + workingDirectory);
        runGitStatus();
    } else {
        workingDirectory = startPath;
        workingDirLabel->setText("Working Dir: <b>" + startPath + "</b> <i>(Not a git repo)</i>");
        workingDirLabel->setStyleSheet("QLabel { color: red; font-size: 10px; padding: 5px; }");
        appendOutput("Warning: Not a git repository", true);
    }
}

void GitWidget::runGitCommand(const QStringList &arguments)
{
    if (workingDirectory.isEmpty()) {
        appendOutput("Error: No working directory set", true);
        return;
    }

    if (gitProcess) {
        if (gitProcess->state() == QProcess::Running) {
            appendOutput("Error: A git command is already running", true);
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
    appendOutput("\n> " + currentCommand);
    
    gitProcess->start("git", arguments);
}

void GitWidget::runGitStatus()
{
    runGitCommand(QStringList() << "status" << "--short");
}

void GitWidget::runGitDiff()
{
    QStringList args;
    args << "diff";
    
    // If files are selected, show diff for those files only
    QList<QListWidgetItem*> selectedItems = fileListWidget->selectedItems();
    if (!selectedItems.isEmpty()) {
        for (QListWidgetItem *item : selectedItems) {
            QString fileName = item->text().mid(3); // Remove status prefix
            args << fileName;
        }
    }
    
    runGitCommand(args);
}

void GitWidget::runGitAdd()
{
    QList<QListWidgetItem*> selectedItems = fileListWidget->selectedItems();
    
    if (selectedItems.isEmpty()) {
        appendOutput("Error: No files selected", true);
        return;
    }

    QStringList args;
    args << "add";
    
    for (QListWidgetItem *item : selectedItems) {
        QString fileName = item->text().mid(3); // Remove status prefix (e.g., "M  ")
        args << fileName;
    }
    
    runGitCommand(args);
}

void GitWidget::runGitCommit()
{
    QString message = commitMessageEdit->text().trimmed();
    
    if (message.isEmpty()) {
        QMessageBox::warning(this, "Commit Error", "Please enter a commit message");
        return;
    }

    QStringList args;
    args << "commit" << "-m" << message;
    
    runGitCommand(args);
}

void GitWidget::runGitPush()
{
    runGitCommand(QStringList() << "push");
}

void GitWidget::runGitPull()
{
    runGitCommand(QStringList() << "pull");
}

void GitWidget::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit) {
        QString output = gitProcess->readAllStandardOutput();
        QString error = gitProcess->readAllStandardError();
        
        if (!output.isEmpty()) {
            appendOutput(output);
        }
        
        if (!error.isEmpty()) {
            appendOutput(error, exitCode != 0);
        }
        
        if (exitCode == 0) {
            appendOutput("✓ Command completed successfully");
            
            // Clear commit message after successful commit
            if (currentCommand.contains("commit")) {
                commitMessageEdit->clear();
            }
            
            // Update file list after status command
            if (currentCommand.contains("status")) {
                updateFileList(output);
            }
        } else {
            appendOutput("✗ Command failed with exit code " + QString::number(exitCode), true);
        }
    } else {
        appendOutput("✗ Process crashed", true);
    }
}

void GitWidget::onProcessError(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start git. Make sure git is installed and in PATH.";
            break;
        case QProcess::Crashed:
            errorMsg = "Git process crashed.";
            break;
        case QProcess::Timedout:
            errorMsg = "Git process timed out.";
            break;
        case QProcess::WriteError:
            errorMsg = "Write error occurred.";
            break;
        case QProcess::ReadError:
            errorMsg = "Read error occurred.";
            break;
        default:
            errorMsg = "Unknown error occurred.";
    }
    
    appendOutput("Error: " + errorMsg, true);
}

void GitWidget::appendOutput(const QString &text, bool isError)
{
    if (isError) {
        outputDisplay->setTextColor(Qt::red);
    } else {
        outputDisplay->setTextColor(Qt::black);
    }
    
    outputDisplay->append(text);
    
    // Scroll to bottom
    QTextCursor cursor = outputDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    outputDisplay->setTextCursor(cursor);
}

void GitWidget::updateFileList(const QString &statusOutput)
{
    fileListWidget->clear();
    
    QStringList lines = statusOutput.split('\n', Qt::SkipEmptyParts);
    
    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        if (!trimmedLine.isEmpty()) {
            fileListWidget->addItem(line);
        }
    }
    
    if (fileListWidget->count() == 0) {
        fileListWidget->addItem("(No changes)");
    }
}
