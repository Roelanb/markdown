#include "previewwidget.h"
#include "emojisupport.h"
#include "codehighlighter.h"

#include <QVBoxLayout>
#include <QRegularExpression>
#include <QTimer>

PreviewWidget::PreviewWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    webView = new QWebEngineView(this);
    layout->addWidget(webView);

    setLayout(layout);
}

PreviewWidget::~PreviewWidget()
{
}

void PreviewWidget::updatePreview(const QString &markdown)
{
    // Save current scroll position before updating
    QString getScrollScript = R"(
        (function() {
            var scrollTop = window.pageYOffset || document.documentElement.scrollTop;
            var scrollHeight = document.body.scrollHeight;
            return scrollHeight > 0 ? scrollTop / scrollHeight : 0;
        })();
    )";
    
    webView->page()->runJavaScript(getScrollScript, [this, markdown](const QVariant &result) {
        double scrollPercentage = result.toDouble();
        
        // Update the HTML content
        QString html = markdownToHtml(markdown);
        webView->setHtml(html);
        
        // Restore scroll position after a short delay to allow content to load
        QTimer::singleShot(10, [this, scrollPercentage]() {
            scrollToPercentage(scrollPercentage);
        });
    });
}

void PreviewWidget::scrollToPercentage(double percentage)
{
    // Use JavaScript to scroll the web view to the specified percentage
    QString script = QString("window.scrollTo(0, document.body.scrollHeight * %1);").arg(percentage);
    webView->page()->runJavaScript(script);
}

QString PreviewWidget::getStyleSheet()
{
    return R"(
        <style>
            body {
                font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Roboto', 'Oxygen', 'Ubuntu', 'Cantarell', 'Fira Sans', 'Droid Sans', 'Helvetica Neue', sans-serif;
                line-height: 1.6;
                color: #333;
                margin: 0;
                padding: 16px;
            }
            h1, h2, h3, h4, h5, h6 {
                margin-top: 24px;
                margin-bottom: 12px;
                font-weight: 600;
                line-height: 1.25;
            }
            h1 {
                font-size: 2em;
                border-bottom: 1px solid #eaecef;
                padding-bottom: 0.3em;
            }
            h2 {
                font-size: 1.5em;
                border-bottom: 1px solid #eaecef;
                padding-bottom: 0.3em;
            }
            h3 { font-size: 1.25em; }
            h4 { font-size: 1em; }
            h5 { font-size: 0.875em; }
            h6 { font-size: 0.85em; color: #6a737d; }

            p {
                margin-top: 0;
                margin-bottom: 16px;
            }

            a {
                color: #0366d6;
                text-decoration: none;
            }
            a:hover {
                text-decoration: underline;
            }

            code {
                background-color: rgba(27, 31, 35, 0.05);
                border-radius: 3px;
                font-family: 'SFMono-Regular', 'Consolas', 'Liberation Mono', 'Menlo', 'Courier', monospace;
                font-size: 85%;
                margin: 0;
                padding: 0.2em 0.4em;
            }

            pre {
                background-color: #f6f8fa;
                border-radius: 3px;
                font-family: 'SFMono-Regular', 'Consolas', 'Liberation Mono', 'Menlo', 'Courier', monospace;
                font-size: 85%;
                line-height: 1.45;
                overflow: auto;
                padding: 16px;
            }

            pre code {
                background-color: transparent;
                border: 0;
                display: inline;
                line-height: inherit;
                margin: 0;
                overflow: visible;
                padding: 0;
                word-wrap: normal;
            }

            blockquote {
                border-left: 4px solid #dfe2e5;
                color: #6a737d;
                margin: 0 0 16px 0;
                padding: 0 1em;
            }

            ul, ol {
                margin-top: 0;
                margin-bottom: 16px;
                padding-left: 2em;
            }

            li + li {
                margin-top: 0.25em;
            }

            table {
                border-collapse: collapse;
                border-spacing: 0;
                margin-top: 0;
                margin-bottom: 16px;
                width: 100%;
            }

            table th, table td {
                border: 1px solid #dfe2e5;
                padding: 6px 13px;
            }

            table th {
                background-color: #f6f8fa;
                font-weight: 600;
            }

            table tr:nth-child(2n) {
                background-color: #f6f8fa;
            }

            img {
                max-width: 100%;
                box-sizing: border-box;
            }

            hr {
                background-color: #e1e4e8;
                border: 0;
                height: 2px;
                margin: 24px 0;
                padding: 0;
            }

            del {
                text-decoration: line-through;
            }

            input[type="checkbox"] {
                margin-right: 0.5em;
            }

            .yaml-frontmatter {
                background-color: #f6f8fa;
                border: 1px solid #d0d7de;
                border-radius: 6px;
                padding: 16px;
                margin-bottom: 24px;
                font-family: 'SFMono-Regular', 'Consolas', 'Liberation Mono', 'Menlo', 'Courier', monospace;
                font-size: 0.9em;
            }

            .yaml-frontmatter-title {
                font-weight: 600;
                color: #0969da;
                margin-bottom: 8px;
                font-size: 0.95em;
            }

            .yaml-frontmatter pre {
                background-color: transparent;
                padding: 0;
                margin: 0;
                border: 0;
                color: #24292f;
            }
        </style>
    )";
}

QString PreviewWidget::markdownToHtml(const QString &markdown)
{
    QString html = markdown;

    // Process YAML front matter first (before anything else)
    html = processYamlFrontMatter(html);

    // Process emojis first (before escaping)
    html = processEmojis(html);

    // Process code blocks first (they need special handling)
    html = processCodeBlocks(html);

    // Process tables (before other block elements)
    html = processTables(html);

    // Escape HTML entities (but preserve our code block markers)
    html.replace("&", "&amp;");
    // Don't escape < and > yet - we need them for our HTML tags

    // Headers (process before inline formatting)
    html.replace(QRegularExpression("^######\\s+(.+)$", QRegularExpression::MultilineOption), "<h6>\\1</h6>");
    html.replace(QRegularExpression("^#####\\s+(.+)$", QRegularExpression::MultilineOption), "<h5>\\1</h5>");
    html.replace(QRegularExpression("^####\\s+(.+)$", QRegularExpression::MultilineOption), "<h4>\\1</h4>");
    html.replace(QRegularExpression("^###\\s+(.+)$", QRegularExpression::MultilineOption), "<h3>\\1</h3>");
    html.replace(QRegularExpression("^##\\s+(.+)$", QRegularExpression::MultilineOption), "<h2>\\1</h2>");
    html.replace(QRegularExpression("^#\\s+(.+)$", QRegularExpression::MultilineOption), "<h1>\\1</h1>");

    // Images (before links, as they have similar syntax)
    html.replace(QRegularExpression("!\\[([^\\]]*)\\]\\(([^)]+)\\)"), "<img src=\"\\2\" alt=\"\\1\" />");

    // Links
    html.replace(QRegularExpression("\\[([^\\]]+)\\]\\(([^)]+)\\)"), "<a href=\"\\2\">\\1</a>");

    // Bold (before italic to handle ** correctly)
    html.replace(QRegularExpression("\\*\\*([^*]+)\\*\\*"), "<strong>\\1</strong>");
    html.replace(QRegularExpression("__([^_]+)__"), "<strong>\\1</strong>");

    // Italic
    html.replace(QRegularExpression("(?<!\\*)\\*([^*]+)\\*(?!\\*)"), "<em>\\1</em>");
    html.replace(QRegularExpression("(?<!_)_([^_]+)_(?!_)"), "<em>\\1</em>");

    // Strikethrough
    html.replace(QRegularExpression("~~([^~]+)~~"), "<del>\\1</del>");

    // Inline code
    html.replace(QRegularExpression("`([^`]+)`"), "<code>\\1</code>");

    // Horizontal rules
    html.replace(QRegularExpression("^\\s*([-*_]){3,}\\s*$", QRegularExpression::MultilineOption), "<hr>");

    // Blockquotes
    html.replace(QRegularExpression("^>\\s+(.+)$", QRegularExpression::MultilineOption), "<blockquote>\\1</blockquote>");

    // Lists - process line by line to handle properly
    QStringList lines = html.split('\n');
    QStringList processedLines;
    enum ListType { None, Unordered, Ordered };
    ListType currentListType = None;
    
    for (const QString &line : lines) {
        // Check for unordered list items
        QRegularExpression unorderedListRegex("^[-*+]\\s+(.+)$");
        QRegularExpressionMatch unorderedMatch = unorderedListRegex.match(line);
        
        // Check for ordered list items
        QRegularExpression orderedListRegex("^\\d+\\.\\s+(.+)$");
        QRegularExpressionMatch orderedMatch = orderedListRegex.match(line);
        
        if (unorderedMatch.hasMatch()) {
            // Close ordered list if we were in one
            if (currentListType == Ordered) {
                processedLines.append("</ol>");
                currentListType = None;
            }
            // Open unordered list if not already in one
            if (currentListType == None) {
                processedLines.append("<ul>");
                currentListType = Unordered;
            }
            processedLines.append("<li>" + unorderedMatch.captured(1) + "</li>");
        } else if (orderedMatch.hasMatch()) {
            // Close unordered list if we were in one
            if (currentListType == Unordered) {
                processedLines.append("</ul>");
                currentListType = None;
            }
            // Open ordered list if not already in one
            if (currentListType == None) {
                processedLines.append("<ol>");
                currentListType = Ordered;
            }
            processedLines.append("<li>" + orderedMatch.captured(1) + "</li>");
        } else {
            // Close any open list
            if (currentListType == Unordered) {
                processedLines.append("</ul>");
                currentListType = None;
            } else if (currentListType == Ordered) {
                processedLines.append("</ol>");
                currentListType = None;
            }
            processedLines.append(line);
        }
    }
    
    // Close any remaining open list
    if (currentListType == Unordered) {
        processedLines.append("</ul>");
    } else if (currentListType == Ordered) {
        processedLines.append("</ol>");
    }
    
    html = processedLines.join('\n');

    // Line breaks and paragraphs
    html.replace("\n\n", "</p><p>");
    html.replace("\n", "<br>");

    // Wrap in paragraphs
    html = "<p>" + html + "</p>";

    // Clean up empty paragraphs and fix paragraph/list/table nesting
    html.replace(QRegularExpression("<p>\\s*<ul>"), "<ul>");
    html.replace(QRegularExpression("</ul>\\s*</p>"), "</ul>");
    html.replace(QRegularExpression("<p>\\s*<ol>"), "<ol>");
    html.replace(QRegularExpression("</ol>\\s*</p>"), "</ol>");
    html.replace(QRegularExpression("<p>\\s*<table>"), "<table>");
    html.replace(QRegularExpression("</table>\\s*</p>"), "</table>");
    html.replace(QRegularExpression("<p>\\s*<h([1-6])>"), "<h\\1>");
    html.replace(QRegularExpression("</h([1-6])>\\s*</p>"), "</h\\1>");
    html.replace(QRegularExpression("<p>\\s*<hr>\\s*</p>"), "<hr>");
    html.replace(QRegularExpression("<p>\\s*<pre>"), "<pre>");
    html.replace(QRegularExpression("</pre>\\s*</p>"), "</pre>");
    html.replace(QRegularExpression("<p>\\s*<blockquote>"), "<blockquote>");
    html.replace(QRegularExpression("</blockquote>\\s*</p>"), "</blockquote>");
    
    // Remove <br> tags before and after block elements
    html.replace(QRegularExpression("<br>\\s*<table>"), "<table>");
    html.replace(QRegularExpression("</table>\\s*<br>"), "</table>");
    html.replace(QRegularExpression("<br>\\s*<ul>"), "<ul>");
    html.replace(QRegularExpression("</ul>\\s*<br>"), "</ul>");
    html.replace(QRegularExpression("<br>\\s*<ol>"), "<ol>");
    html.replace(QRegularExpression("</ol>\\s*<br>"), "</ol>");
    html.replace(QRegularExpression("<br>\\s*<h([1-6])>"), "<h\\1>");
    html.replace(QRegularExpression("</h([1-6])>\\s*<br>"), "</h\\1>");
    html.replace(QRegularExpression("<br>\\s*<pre>"), "<pre>");
    html.replace(QRegularExpression("</pre>\\s*<br>"), "</pre>");
    html.replace(QRegularExpression("<br>\\s*<hr>"), "<hr>");
    html.replace(QRegularExpression("<hr>\\s*<br>"), "<hr>");
    html.replace(QRegularExpression("<br>\\s*<blockquote>"), "<blockquote>");
    html.replace(QRegularExpression("</blockquote>\\s*<br>"), "</blockquote>");
    
    // Clean up empty paragraphs
    html.replace(QRegularExpression("<p>\\s*</p>"), "");

    // Complete HTML document
    QString fullHtml = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">";
    fullHtml += getStyleSheet();
    fullHtml += "</head><body>";
    fullHtml += html;
    fullHtml += "</body></html>";

    return fullHtml;
}

QString PreviewWidget::processEmojis(const QString &text)
{
    return EmojiSupport::replaceEmojiCodes(text);
}

QString PreviewWidget::processCodeBlocks(const QString &text)
{
    QString result = text;
    QRegularExpression codeBlockRegex("```(\\w*)\\n([\\s\\S]*?)```");
    QRegularExpressionMatchIterator i = codeBlockRegex.globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString language = match.captured(1);
        QString code = match.captured(2);

        QString highlighted = CodeHighlighter::highlightCode(code, language);
        QString replacement = QString("<pre><code>%1</code></pre>").arg(highlighted);

        result.replace(match.captured(0), replacement);
    }

    return result;
}

QString PreviewWidget::processTables(const QString &text)
{
    QStringList lines = text.split('\n');
    QStringList result;
    bool inTable = false;
    QStringList tableLines;
    
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        
        // Check if this line contains table separators (|)
        if (line.contains('|')) {
            // Check if next line is a separator line (|---|---|)
            bool isSeparatorNext = false;
            if (i + 1 < lines.size()) {
                QString nextLine = lines[i + 1];
                QRegularExpression separatorRegex("^\\s*\\|?\\s*[-:]+\\s*\\|");
                isSeparatorNext = separatorRegex.match(nextLine).hasMatch();
            }
            
            if (isSeparatorNext || inTable) {
                if (!inTable) {
                    inTable = true;
                    tableLines.clear();
                }
                tableLines.append(line);
            } else {
                result.append(line);
            }
        } else if (inTable && line.trimmed().isEmpty()) {
            // End of table
            result.append(convertTableToHtml(tableLines));
            tableLines.clear();
            inTable = false;
            result.append(line);
        } else if (inTable) {
            // Check if this is a separator line
            QRegularExpression separatorRegex("^\\s*\\|?\\s*[-:]+");
            if (separatorRegex.match(line).hasMatch()) {
                tableLines.append(line);
            } else {
                // End of table
                result.append(convertTableToHtml(tableLines));
                tableLines.clear();
                inTable = false;
                result.append(line);
            }
        } else {
            result.append(line);
        }
    }
    
    // Handle table at end of document
    if (inTable && !tableLines.isEmpty()) {
        result.append(convertTableToHtml(tableLines));
    }
    
    return result.join('\n');
}

QString PreviewWidget::convertTableToHtml(const QStringList &tableLines)
{
    if (tableLines.isEmpty()) {
        return "";
    }
    
    QString html = "<table>\n";
    bool headerProcessed = false;
    bool inBody = false;
    
    for (int i = 0; i < tableLines.size(); ++i) {
        QString line = tableLines[i].trimmed();
        
        // Skip separator line
        QRegularExpression separatorRegex("^\\|?\\s*[-:]+");
        if (separatorRegex.match(line).hasMatch()) {
            if (!inBody) {
                html += "<tbody>\n";
                inBody = true;
            }
            continue;
        }
        
        // Remove leading and trailing pipes
        if (line.startsWith('|')) {
            line = line.mid(1);
        }
        if (line.endsWith('|')) {
            line.chop(1);
        }
        
        // Split by pipe
        QStringList cells = line.split('|');
        
        if (!headerProcessed) {
            // First row is header
            html += "<thead>\n<tr>\n";
            for (const QString &cell : cells) {
                html += "<th>" + cell.trimmed() + "</th>\n";
            }
            html += "</tr>\n</thead>\n";
            headerProcessed = true;
        } else {
            // Data rows
            if (!inBody) {
                html += "<tbody>\n";
                inBody = true;
            }
            html += "<tr>\n";
            for (const QString &cell : cells) {
                html += "<td>" + cell.trimmed() + "</td>\n";
            }
            html += "</tr>\n";
        }
    }
    
    if (inBody) {
        html += "</tbody>\n";
    }
    html += "</table>";
    
    return html;
}

QString PreviewWidget::processYamlFrontMatter(const QString &text)
{
    // Check if the text starts with YAML front matter (---)
    if (!text.startsWith("---")) {
        return text;
    }
    
    // Find the closing --- delimiter
    int firstNewline = text.indexOf('\n');
    if (firstNewline == -1) {
        return text;
    }
    
    int closingDelimiter = text.indexOf("\n---", firstNewline);
    if (closingDelimiter == -1) {
        // Also check for ... as closing delimiter
        closingDelimiter = text.indexOf("\n...", firstNewline);
        if (closingDelimiter == -1) {
            return text;
        }
    }
    
    // Extract YAML content (between the delimiters)
    QString yamlContent = text.mid(firstNewline + 1, closingDelimiter - firstNewline - 1);
    
    // Get the rest of the markdown content
    int contentStart = text.indexOf('\n', closingDelimiter + 1);
    QString remainingContent = (contentStart != -1) ? text.mid(contentStart + 1) : "";
    
    // Create HTML for YAML front matter display
    QString yamlHtml = "<div class=\"yaml-frontmatter\">\n";
    yamlHtml += "<div class=\"yaml-frontmatter-title\">Document Metadata</div>\n";
    yamlHtml += "<pre>" + yamlContent.trimmed() + "</pre>\n";
    yamlHtml += "</div>\n";
    
    // Return the YAML HTML followed by the remaining markdown
    return yamlHtml + remainingContent;
}
