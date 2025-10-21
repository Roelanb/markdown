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

signals:
    void scrollPercentageChanged(double percentage);

protected:
    void scrollContentsBy(int dx, int dy) override;

private:
    MarkdownHighlighter *highlighter;
};

#endif // MARKDOWNEDITOR_H
