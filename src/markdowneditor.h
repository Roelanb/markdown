#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include <QPlainTextEdit>

class MarkdownHighlighter;

class MarkdownEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit MarkdownEditor(QWidget *parent = nullptr);
    ~MarkdownEditor();

    void setCurrentFilePath(const QString &path);
    QString getCurrentFilePath() const;

signals:
    void scrollPercentageChanged(double percentage);

protected:
    void scrollContentsBy(int dx, int dy) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    MarkdownHighlighter *highlighter;
    QString currentFilePath;
    
    void insertImageMarkdown(const QString &imagePath, const QPoint &dropPos);
    QString calculateRelativePath(const QString &fromFile, const QString &toFile) const;
};

#endif // MARKDOWNEDITOR_H
