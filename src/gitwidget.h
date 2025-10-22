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

class GitWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GitWidget(QWidget *parent = nullptr);
    ~GitWidget();

    void setWorkingDirectory(const QString &path);

private slots:
    void runGitStatus();
    void runGitDiff();
    void runGitAdd();
    void runGitCommit();
    void runGitPush();
    void runGitPull();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    void setupUI();
    void runGitCommand(const QStringList &arguments);
    void appendOutput(const QString &text, bool isError = false);
    void updateFileList(const QString &statusOutput);

    // UI Components
    QLabel *workingDirLabel;
    QTextEdit *outputDisplay;
    QLineEdit *commitMessageEdit;
    QListWidget *fileListWidget;
    QPushButton *statusButton;
    QPushButton *diffButton;
    QPushButton *addButton;
    QPushButton *commitButton;
    QPushButton *pushButton;
    QPushButton *pullButton;

    // Git state
    QString workingDirectory;
    QProcess *gitProcess;
    QString currentCommand;
};

#endif // GITWIDGET_H
