#include "markdowneditor.h"
#include "markdownhighlighter.h"

#include <QFont>
#include <QScrollBar>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QTextCursor>

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
    
    // Enable drag and drop
    setAcceptDrops(true);
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

void MarkdownEditor::setCurrentFilePath(const QString &path)
{
    currentFilePath = path;
}

QString MarkdownEditor::getCurrentFilePath() const
{
    return currentFilePath;
}

void MarkdownEditor::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept drag if it contains URLs (file paths)
    if (event->mimeData()->hasUrls()) {
        // Check if any of the URLs are image files
        bool hasImageFile = false;
        for (const QUrl &url : event->mimeData()->urls()) {
            if (url.isLocalFile()) {
                QString filePath = url.toLocalFile();
                QString suffix = QFileInfo(filePath).suffix().toLower();
                if (suffix == "png" || suffix == "jpg" || suffix == "jpeg" || 
                    suffix == "gif" || suffix == "bmp" || suffix == "svg" || 
                    suffix == "webp" || suffix == "ico") {
                    hasImageFile = true;
                    break;
                }
            }
        }
        
        if (hasImageFile) {
            event->acceptProposedAction();
            return;
        }
    }
    
    // Otherwise, use default behavior
    QPlainTextEdit::dragEnterEvent(event);
}

void MarkdownEditor::dragMoveEvent(QDragMoveEvent *event)
{
    // Accept drag move if it contains image URLs
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        QPlainTextEdit::dragMoveEvent(event);
    }
}

void MarkdownEditor::dropEvent(QDropEvent *event)
{
    // Handle file drops
    if (event->mimeData()->hasUrls()) {
        for (const QUrl &url : event->mimeData()->urls()) {
            if (url.isLocalFile()) {
                QString filePath = url.toLocalFile();
                QString suffix = QFileInfo(filePath).suffix().toLower();
                
                // Check if it's an image file
                if (suffix == "png" || suffix == "jpg" || suffix == "jpeg" || 
                    suffix == "gif" || suffix == "bmp" || suffix == "svg" || 
                    suffix == "webp" || suffix == "ico") {
                    insertImageMarkdown(filePath, event->position().toPoint());
                    event->acceptProposedAction();
                    return;
                }
            }
        }
    }
    
    // Otherwise, use default behavior (for text drops)
    QPlainTextEdit::dropEvent(event);
}

void MarkdownEditor::insertImageMarkdown(const QString &imagePath, const QPoint &dropPos)
{
    QString markdownImage;
    
    // Calculate relative path if current file is saved
    if (!currentFilePath.isEmpty()) {
        QString relativePath = calculateRelativePath(currentFilePath, imagePath);
        QFileInfo imageInfo(imagePath);
        QString altText = imageInfo.baseName();
        markdownImage = QString("![%1](%2)").arg(altText, relativePath);
    } else {
        // Use absolute path if current file is not saved
        QFileInfo imageInfo(imagePath);
        QString altText = imageInfo.baseName();
        markdownImage = QString("![%1](%2)").arg(altText, imagePath);
    }
    
    // Get cursor at drop position
    QTextCursor cursor = cursorForPosition(dropPos);
    cursor.insertText(markdownImage);
    setTextCursor(cursor);
}

QString MarkdownEditor::calculateRelativePath(const QString &fromFile, const QString &toFile) const
{
    QFileInfo fromInfo(fromFile);
    QFileInfo toInfo(toFile);
    
    // Get the directory of the markdown file
    QDir fromDir = fromInfo.absoluteDir();
    
    // Calculate relative path from the markdown file's directory to the image
    QString relativePath = fromDir.relativeFilePath(toInfo.absoluteFilePath());
    
    return relativePath;
}
