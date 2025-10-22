#ifndef GITWIDGET_H
#define GITWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include <QProcess>
#include <QString>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTimer>

class GitWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GitWidget(QWidget *parent = nullptr);
    ~GitWidget();

    void setWorkingDirectory(const QString &path);

private slots:
    void refreshStatus();
    void stageFile(QTreeWidgetItem *item);
    void unstageFile(QTreeWidgetItem *item);
    void stageAll();
    void unstageAll();
    void commitChanges();
    void pushChanges();
    void pullChanges();
    void syncChanges();
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void autoRefresh();

private:
    void setupUI();
    void runGitCommand(const QStringList &arguments);
    void appendOutput(const QString &text, bool isError = false);
    void updateFileList(const QString &statusOutput);

    // UI Components
    QLabel *workingDirLabel;
    QTextEdit *commitMessageEdit;
    QPushButton *commitButton;
    QPushButton *refreshButton;
    QPushButton *pullButton;
    QPushButton *pushButton;
    QPushButton *syncButton;
    QTreeWidget *changesTree;
    QTreeWidget *stagedTree;
    QTreeWidgetItem *changesRoot;
    QTreeWidgetItem *stagedRoot;
    QTimer *autoRefreshTimer;

    // Git state
    QString workingDirectory;
    QProcess *gitProcess;
    QString currentCommand;
};

#endif // GITWIDGET_H
