#include "markdowneditor.h"
#include "markdownhighlighter.h"

#include <QFont>
#include <QScrollBar>

MarkdownEditor::MarkdownEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    // Set a monospace font
    QFont font("Monospace", 11);
    font.setStyleHint(QFont::TypeWriter);
    setFont(font);

    // Set tab stop width (4 spaces)
    QFontMetrics metrics(font);
    setTabStopDistance(4 * metrics.horizontalAdvance(' '));

    // Enable line wrapping
    setLineWrapMode(QPlainTextEdit::WidgetWidth);

    // Create syntax highlighter
    highlighter = new MarkdownHighlighter(document());
}

MarkdownEditor::~MarkdownEditor()
{
}

void MarkdownEditor::scrollContentsBy(int dx, int dy)
{
    QPlainTextEdit::scrollContentsBy(dx, dy);
    
    // Calculate scroll percentage
    QScrollBar *vScrollBar = verticalScrollBar();
    if (vScrollBar->maximum() > 0) {
        double percentage = static_cast<double>(vScrollBar->value()) / vScrollBar->maximum();
        emit scrollPercentageChanged(percentage);
    } else {
        emit scrollPercentageChanged(0.0);
    }
}
