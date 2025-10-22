#include "previewwidget.h"
#include "emojisupport.h"
#include "codehighlighter.h"

#include <QVBoxLayout>
#include <QRegularExpression>
#include <QTimer>
#include <QScrollBar>

PreviewWidget::PreviewWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    webView = new QTextBrowser(this);
    webView->setOpenExternalLinks(true);
    layout->addWidget(webView);

    setLayout(layout);
}

PreviewWidget::~PreviewWidget()
{
}

void PreviewWidget::updatePreview(const QString &markdown)
{
    // Save current scroll position before updating
    QScrollBar *vScrollBar = webView->verticalScrollBar();
    int scrollPos = vScrollBar ? vScrollBar->value() : 0;

    // Update the HTML content
    QString html = markdownToHtml(markdown);
    webView->setHtml(html);

    // Restore scroll position after a short delay to allow content to load
    QTimer::singleShot(10, [this, scrollPos]() {
        QScrollBar *vScrollBar = webView->verticalScrollBar();
        if (vScrollBar) {
            vScrollBar->setValue(scrollPos);
        }
    });
}

void PreviewWidget::scrollToPercentage(double percentage)
{
    // Scroll the text browser to the specified percentage
    QScrollBar *vScrollBar = webView->verticalScrollBar();
    if (vScrollBar) {
        int maxScroll = vScrollBar->maximum();
        vScrollBar->setValue(static_cast<int>(maxScroll * percentage));
    }
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

            li {
                margin-top: 0.25em;
                margin-bottom: 0.25em;
                line-height: 1.5;
            }

            li:first-child {
                margin-top: 0;
            }

            li:last-child {
                margin-bottom: 0;
            }

            li > p {
                margin-top: 0;
                margin-bottom: 0;
                line-height: 1.5;
            }

            li p:first-child {
                margin-top: 0;
            }

            li p:last-child {
                margin-bottom: 0;
            }

            /* Nested lists */
            li > ul, li > ol {
                margin-top: 0.25em;
                margin-bottom: 0.25em;
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

    // Process emojis early (before escaping)
    html = processEmojis(html);

    // Protect code blocks first - they should not be processed as markdown
    html = processCodeBlocks(html);

    // Process tables (after code blocks but before other processing)
    html = processTables(html);

    // Now escape HTML entities in remaining text (outside of code blocks/tables)
    html = escapeHtmlInText(html);

    // Process backslash escapes (before other inline processing)
    html = processBackslashEscapes(html);

    // Process autolinks (before regular links)
    html = processAutolinks(html);

    // Headers (space is now optional)
    html.replace(QRegularExpression("^######\\s*(.+)$", QRegularExpression::MultilineOption), "<h6>\\1</h6>");
    html.replace(QRegularExpression("^#####\\s*(.+)$", QRegularExpression::MultilineOption), "<h5>\\1</h5>");
    html.replace(QRegularExpression("^####\\s*(.+)$", QRegularExpression::MultilineOption), "<h4>\\1</h4>");
    html.replace(QRegularExpression("^###\\s*(.+)$", QRegularExpression::MultilineOption), "<h3>\\1</h3>");
    html.replace(QRegularExpression("^##\\s*(.+)$", QRegularExpression::MultilineOption), "<h2>\\1</h2>");
    html.replace(QRegularExpression("^#\\s*(.+)$", QRegularExpression::MultilineOption), "<h1>\\1</h1>");

    // Horizontal rules (enforce same character repeated at least 3 times)
    html.replace(QRegularExpression("^\\s*(-{3,}|\\*{3,}|_{3,})\\s*$", QRegularExpression::MultilineOption), "<hr>");

    // Blockquotes (improved to handle multi-line)
    html = processBlockquotes(html);

    // Lists - improved with nested list and task list support
    html = processLists(html);

    // Process inline code BEFORE other inline elements (code content is literal)
    html = processInlineCode(html);

    // Images (before links, as they have similar syntax)
    html.replace(QRegularExpression("!\\[([^\\]]*)\\]\\(([^)]+)\\)"), "<img src=\"\\2\" alt=\"\\1\" />");

    // Links
    html.replace(QRegularExpression("\\[([^\\]]+)\\]\\(([^)]+)\\)"), "<a href=\"\\2\">\\1</a>");

    // Bold and italic with proper nesting support
    // Process *** (bold+italic) first
    html.replace(QRegularExpression("\\*\\*\\*([^*]+)\\*\\*\\*"), "<strong><em>\\1</em></strong>");
    html.replace(QRegularExpression("___([^_]+)___"), "<strong><em>\\1</em></strong>");

    // Bold
    html.replace(QRegularExpression("\\*\\*([^*]+)\\*\\*"), "<strong>\\1</strong>");
    html.replace(QRegularExpression("__([^_]+)__"), "<strong>\\1</strong>");

    // Italic
    html.replace(QRegularExpression("(?<!\\*)\\*([^*]+)\\*(?!\\*)"), "<em>\\1</em>");
    html.replace(QRegularExpression("(?<!_)_([^_]+)_(?!_)"), "<em>\\1</em>");

    // Strikethrough
    html.replace(QRegularExpression("~~([^~]+)~~"), "<del>\\1</del>");

    // Protect block elements from paragraph/linebreak transforms
    QMap<QString, QString> blockPlaceholders;
    auto protectBlocks = [&](const QString &prefix, const QRegularExpression &rx) {
        QRegularExpressionMatchIterator it = rx.globalMatch(html);
        int idx = 0;
        while (it.hasNext()) {
            auto m = it.next();
            QString block = m.captured(0);
            QString key = QString("\x01BLK_%1_%2\x01").arg(prefix).arg(idx++);
            blockPlaceholders.insert(key, block);
            html.replace(block, key);
        }
    };
    QRegularExpression rxTable("<table[\\s\\S]*?</table>", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression rxPre("<pre[\\s\\S]*?</pre>", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression rxUL("<ul[\\s\\S]*?</ul>", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression rxOL("<ol[\\s\\S]*?</ol>", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression rxBQ("<blockquote[\\s\\S]*?</blockquote>", QRegularExpression::DotMatchesEverythingOption);
    protectBlocks("TABLE", rxTable);
    protectBlocks("PRE", rxPre);
    protectBlocks("UL", rxUL);
    protectBlocks("OL", rxOL);
    protectBlocks("BQ", rxBQ);

    // Hard line breaks (two or more spaces before newline)
    html.replace(QRegularExpression("  +\\n"), "<br>\n");

    // Line breaks and paragraphs
    html.replace("\n\n", "</p><p>");
    html.replace("\n", "<br>");

    // Wrap in paragraphs
    html = "<p>" + html + "</p>";

    // Restore protected blocks (now that paragraphs are created)
    for (auto it = blockPlaceholders.begin(); it != blockPlaceholders.end(); ++it) {
        html.replace(it.key(), it.value());
    }

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
    html.replace(QRegularExpression("<p>\\s*<div"), "<div");
    html.replace(QRegularExpression("</div>\\s*</p>"), "</div>");

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
    html.replace(QRegularExpression("<br>\\s*<div"), "<div");
    html.replace(QRegularExpression("</div>\\s*<br>"), "</div>");

    // Clean up empty paragraphs
    html.replace(QRegularExpression("<p>\\s*</p>"), "");

    // Restore escaped HTML characters
    html = restoreEscapedHtml(html);

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

    // Process fenced code blocks (```language\ncode\n```)
    QRegularExpression codeBlockRegex("```(\\w*)\\n?([\\s\\S]*?)```");
    QRegularExpressionMatchIterator i = codeBlockRegex.globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString language = match.captured(1);
        QString code = match.captured(2);

        // HTML escape the code content
        code.replace("&", "&amp;");
        code.replace("<", "&lt;");
        code.replace(">", "&gt;");

        QString highlighted = CodeHighlighter::highlightCode(code, language);
        QString replacement = QString("<pre><code>%1</code></pre>").arg(highlighted);

        result.replace(match.captured(0), replacement);
    }

    // Process indented code blocks (4 spaces or 1 tab at start of line)
    QStringList lines = result.split('\n');
    QStringList processedLines;
    bool inCodeBlock = false;
    QString codeBlockContent;

    for (const QString &line : lines) {
        // Check if line is already processed HTML (contains tags)
        if (line.contains("<pre>") || line.contains("</pre>") || line.contains("<table>")) {
            if (inCodeBlock) {
                // End code block
                codeBlockContent.replace("&", "&amp;");
                codeBlockContent.replace("<", "&lt;");
                codeBlockContent.replace(">", "&gt;");
                processedLines.append("<pre><code>" + codeBlockContent + "</code></pre>");
                codeBlockContent.clear();
                inCodeBlock = false;
            }
            processedLines.append(line);
            continue;
        }

        // Check if line starts with 4 spaces or tab (indented code block)
        if (line.startsWith("    ") || line.startsWith("\t")) {
            if (!inCodeBlock) {
                inCodeBlock = true;
                codeBlockContent.clear();
            }
            // Remove the indentation (4 spaces or 1 tab)
            QString codeLine = line.startsWith("    ") ? line.mid(4) : line.mid(1);
            if (!codeBlockContent.isEmpty()) {
                codeBlockContent += "\n";
            }
            codeBlockContent += codeLine;
        } else if (line.trimmed().isEmpty() && inCodeBlock) {
            // Empty line in code block - keep it
            codeBlockContent += "\n";
        } else {
            // Not indented - end code block if we were in one
            if (inCodeBlock) {
                codeBlockContent.replace("&", "&amp;");
                codeBlockContent.replace("<", "&lt;");
                codeBlockContent.replace(">", "&gt;");
                processedLines.append("<pre><code>" + codeBlockContent + "</code></pre>");
                codeBlockContent.clear();
                inCodeBlock = false;
            }
            processedLines.append(line);
        }
    }

    // Close any remaining code block
    if (inCodeBlock) {
        codeBlockContent.replace("&", "&amp;");
        codeBlockContent.replace("<", "&lt;");
        codeBlockContent.replace(">", "&gt;");
        processedLines.append("<pre><code>" + codeBlockContent + "</code></pre>");
    }

    result = processedLines.join('\n');
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

        // Check if this line contains pipe characters (potential table row)
        if (line.contains('|')) {
            // Check if next line is a separator line (|---|---|)
            bool isSeparatorNext = false;
            if (i + 1 < lines.size()) {
                QString nextLine = lines[i + 1];
                // Check if it's a multi-column separator line like: |---|:---:|---:|
                QRegularExpression separatorRegex("^\\s*\\|?(?:\\s*:?-+\\s*:?\\s*\\|)+\\s*:?-+\\s*:?\\s*\\|?\\s*$");
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
        } else if (inTable) {
            // Check if this is a multi-column separator line inside the table
            QRegularExpression separatorRegex("^\\s*\\|?(?:\\s*:?-+\\s*:?\\s*\\|)+\\s*:?-+\\s*:?\\s*\\|?\\s*$");
            if (separatorRegex.match(line).hasMatch()) {
                tableLines.append(line);
            } else if (line.trimmed().isEmpty()) {
                // Empty line ends table
                result.append(convertTableToHtml(tableLines));
                tableLines.clear();
                inTable = false;
                result.append(line);
            } else if (line.contains('|')) {
                // Another table row
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
    bool bodyOpened = false;

    // Determine column alignment from the first separator line (if any)
    // e.g. |:---|:---:|---:|
    QStringList alignments; // "left", "center", "right", or ""
    QRegularExpression sepRegex("^\\s*\\|?\\s*[-:]+.*\\|?\\s*$");
    for (const QString &raw : tableLines) {
        QString line = raw.trimmed();
        if (sepRegex.match(line).hasMatch()) {
            // Strip outer pipes
            if (line.startsWith('|')) line = line.mid(1);
            if (line.endsWith('|')) line.chop(1);
            QStringList parts = line.split('|');
            for (const QString &partRaw : parts) {
                QString part = partRaw.trimmed();
                bool left = part.startsWith(':');
                bool right = part.endsWith(':');
                if (left && right) alignments << "center";
                else if (right) alignments << "right";
                else if (left) alignments << "left";
                else alignments << "";
            }
            break;
        }
    }

    for (int i = 0; i < tableLines.size(); ++i) {
        QString line = tableLines[i].trimmed();

        // Skip separator lines (they just separate header from body)
        QRegularExpression separatorRegex("^\\s*\\|?\\s*[-:]+\\s*[-:|\\s]*\\|?\\s*$");
        if (separatorRegex.match(line).hasMatch()) {
            continue;
        }

        // Remove leading and trailing pipes if they exist
        if (line.startsWith('|')) {
            line = line.mid(1);
        }
        if (line.endsWith('|')) {
            line.chop(1);
        }

        // Split by pipe and trim each cell
        QStringList rawCells = line.split('|');
        QStringList cells;
        for (const QString &cell : rawCells) {
            cells << cell.trimmed();
        }

        // Ensure alignments size matches cells
        if (!alignments.isEmpty() && alignments.size() < cells.size()) {
            while (alignments.size() < cells.size()) alignments << "";
        }

        if (!headerProcessed) {
            // First row is header
            html += "<thead>\n<tr>\n";
            for (int c = 0; c < cells.size(); ++c) {
                QString cellContent = cells[c];
                cellContent.replace("&", "&amp;");
                cellContent.replace("<", "&lt;");
                cellContent.replace(">", "&gt;");
                QString style = "";
                if (!alignments.isEmpty() && !alignments[c].isEmpty()) {
                    style = " style=\"text-align:" + alignments[c] + "\"";
                }
                html += "<th" + style + ">" + cellContent + "</th>\n";
            }
            html += "</tr>\n</thead>\n";
            headerProcessed = true;
        } else {
            if (!bodyOpened) {
                html += "<tbody>\n";
                bodyOpened = true;
            }
            html += "<tr>\n";
            for (int c = 0; c < cells.size(); ++c) {
                QString cellContent = cells[c];
                cellContent.replace("&", "&amp;");
                cellContent.replace("<", "&lt;");
                cellContent.replace(">", "&gt;");
                QString style = "";
                if (!alignments.isEmpty() && c < alignments.size() && !alignments[c].isEmpty()) {
                    style = " style=\"text-align:" + alignments[c] + "\"";
                }
                html += "<td" + style + ">" + cellContent + "</td>\n";
            }
            html += "</tr>\n";
        }
    }

    if (bodyOpened) {
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

QString PreviewWidget::escapeHtmlInText(const QString &text)
{
    QString result = text;

    // Protect entire blocks (not just tags) so inner markup is not escaped
    QMap<QString, QString> blocks;
    auto protect = [&](const QString &prefix, const QRegularExpression &rx) {
        QRegularExpressionMatchIterator it = rx.globalMatch(result);
        int idx = 0;
        while (it.hasNext()) {
            auto m = it.next();
            QString block = m.captured(0);
            QString key = QString("\x01PROTECT_%1_%2\x01").arg(prefix).arg(idx++);
            blocks.insert(key, block);
            result.replace(block, key);
        }
    };
    QRegularExpression rxTable("<table[\\s\\S]*?</table>", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression rxPre("<pre[\\s\\S]*?</pre>", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression rxDiv("<div[\\s\\S]*?</div>", QRegularExpression::DotMatchesEverythingOption);
    protect("TABLE", rxTable);
    protect("PRE", rxPre);
    protect("DIV", rxDiv);

    // Now escape HTML in remaining text
    result.replace("&", "&amp;");
    result.replace("<", "&lt;");
    result.replace(">", "&gt;");

    // Restore protected blocks
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        result.replace(it.key(), it.value());
    }

    return result;
}

QString PreviewWidget::restoreEscapedHtml(const QString &text)
{
    QString result = text;

    // Restore backslash-escaped characters
    static const QString ESCAPE_PLACEHOLDER = "\x01ESC\x01";

    result.replace(ESCAPE_PLACEHOLDER + "ASTERISK", "*");
    result.replace(ESCAPE_PLACEHOLDER + "UNDERSCORE", "_");
    result.replace(ESCAPE_PLACEHOLDER + "LBRACKET", "[");
    result.replace(ESCAPE_PLACEHOLDER + "RBRACKET", "]");
    result.replace(ESCAPE_PLACEHOLDER + "LPAREN", "(");
    result.replace(ESCAPE_PLACEHOLDER + "RPAREN", ")");
    result.replace(ESCAPE_PLACEHOLDER + "HASH", "#");
    result.replace(ESCAPE_PLACEHOLDER + "PLUS", "+");
    result.replace(ESCAPE_PLACEHOLDER + "MINUS", "-");
    result.replace(ESCAPE_PLACEHOLDER + "DOT", ".");
    result.replace(ESCAPE_PLACEHOLDER + "EXCLAIM", "!");
    result.replace(ESCAPE_PLACEHOLDER + "BACKTICK", "`");
    result.replace(ESCAPE_PLACEHOLDER + "TILDE", "~");
    result.replace(ESCAPE_PLACEHOLDER + "GT", ">");
    result.replace(ESCAPE_PLACEHOLDER + "LT", "<");
    result.replace(ESCAPE_PLACEHOLDER + "PIPE", "|");
    result.replace(ESCAPE_PLACEHOLDER + "BACKSLASH", "\\");

    return result;
}

QString PreviewWidget::processBackslashEscapes(const QString &text)
{
    QString result = text;

    // Use a unique placeholder that won't appear in normal text
    static const QString ESCAPE_PLACEHOLDER = "\x01ESC\x01";

    // Escape special markdown characters
    result.replace("\\*", ESCAPE_PLACEHOLDER + "ASTERISK");
    result.replace("\\_", ESCAPE_PLACEHOLDER + "UNDERSCORE");
    result.replace("\\[", ESCAPE_PLACEHOLDER + "LBRACKET");
    result.replace("\\]", ESCAPE_PLACEHOLDER + "RBRACKET");
    result.replace("\\(", ESCAPE_PLACEHOLDER + "LPAREN");
    result.replace("\\)", ESCAPE_PLACEHOLDER + "RPAREN");
    result.replace("\\#", ESCAPE_PLACEHOLDER + "HASH");
    result.replace("\\+", ESCAPE_PLACEHOLDER + "PLUS");
    result.replace("\\-", ESCAPE_PLACEHOLDER + "MINUS");
    result.replace("\\.", ESCAPE_PLACEHOLDER + "DOT");
    result.replace("\\!", ESCAPE_PLACEHOLDER + "EXCLAIM");
    result.replace("\\`", ESCAPE_PLACEHOLDER + "BACKTICK");
    result.replace("\\~", ESCAPE_PLACEHOLDER + "TILDE");
    result.replace("\\>", ESCAPE_PLACEHOLDER + "GT");
    result.replace("\\<", ESCAPE_PLACEHOLDER + "LT");
    result.replace("\\|", ESCAPE_PLACEHOLDER + "PIPE");
    result.replace("\\\\", ESCAPE_PLACEHOLDER + "BACKSLASH");

    // After all markdown processing, restore the escaped characters
    // This will be done in a later pass

    return result;
}

QString PreviewWidget::processAutolinks(const QString &text)
{
    QString result = text;

    // Autolink for URLs: <http://example.com> or <https://example.com>
    QRegularExpression urlAutolink("<(https?://[^>]+)>");
    result.replace(urlAutolink, "<a href=\"\\1\">\\1</a>");

    // Autolink for email: <user@example.com>
    QRegularExpression emailAutolink("<([^@>]+@[^@>]+\\.[^@>]+)>");
    result.replace(emailAutolink, "<a href=\"mailto:\\1\">\\1</a>");

    return result;
}

QString PreviewWidget::processBlockquotes(const QString &text)
{
    QStringList lines = text.split('\n');
    QStringList result;
    bool inBlockquote = false;
    QString blockquoteContent;
    int blockquoteLevel = 0;

    for (const QString &line : lines) {
        // Count leading > characters
        int level = 0;
        int pos = 0;
        while (pos < line.length() && line[pos] == '>') {
            level++;
            pos++;
            // Skip optional space after >
            if (pos < line.length() && line[pos] == ' ') {
                pos++;
            }
        }

        if (level > 0) {
            // We're in a blockquote
            QString content = line.mid(pos);

            if (!inBlockquote) {
                // Start new blockquote
                inBlockquote = true;
                blockquoteLevel = level;
                blockquoteContent = content;
            } else {
                // Continue blockquote
                blockquoteContent += "\n" + content;
            }
        } else if (line.trimmed().isEmpty() && inBlockquote) {
            // Empty line inside blockquote - continue
            blockquoteContent += "\n";
        } else {
            // End of blockquote
            if (inBlockquote) {
                // Wrap in blockquote tags (one level for simplicity)
                result.append("<blockquote>" + blockquoteContent + "</blockquote>");
                inBlockquote = false;
                blockquoteContent.clear();
                blockquoteLevel = 0;
            }
            result.append(line);
        }
    }

    // Close any remaining blockquote
    if (inBlockquote) {
        result.append("<blockquote>" + blockquoteContent + "</blockquote>");
    }

    return result.join('\n');
}

QString PreviewWidget::processLists(const QString &text)
{
    QStringList lines = text.split('\n');
    QStringList processedLines;

    enum ListType { None, Unordered, Ordered };
    ListType currentListType = None;
    int currentIndent = 0;

    for (int i = 0; i < lines.size(); ++i) {
        const QString &line = lines[i];

        // Skip already-processed HTML
        if (line.contains("<pre>") || line.contains("</pre>") ||
            line.contains("<table>") || line.contains("<blockquote>")) {
            if (currentListType != None) {
                if (currentListType == Unordered) {
                    processedLines.append("</ul>");
                } else {
                    processedLines.append("</ol>");
                }
                currentListType = None;
                currentIndent = 0;
            }
            processedLines.append(line);
            continue;
        }

        // Measure indentation
        int indent = 0;
        while (indent < line.length() && line[indent] == ' ') {
            indent++;
        }

        QString trimmedLine = line.trimmed();

        // Check for task list items (- [ ] or - [x])
        QRegularExpression taskListRegex("^[-*+]\\s+\\[([ xX])\\]\\s+(.+)$");
        QRegularExpressionMatch taskMatch = taskListRegex.match(trimmedLine);

        // Check for unordered list items
        QRegularExpression unorderedListRegex("^[-*+]\\s+(.+)$");
        QRegularExpressionMatch unorderedMatch = unorderedListRegex.match(trimmedLine);

        // Check for ordered list items
        QRegularExpression orderedListRegex("^(\\d+)\\.\\s+(.+)$");
        QRegularExpressionMatch orderedMatch = orderedListRegex.match(trimmedLine);

        if (taskMatch.hasMatch()) {
            // Task list item
            QString checked = taskMatch.captured(1).toLower() == "x" ? "checked" : "";
            QString content = taskMatch.captured(2);

            if (currentListType == Ordered) {
                processedLines.append("</ol>");
                currentListType = None;
            }
            if (currentListType == None) {
                processedLines.append("<ul>");
                currentListType = Unordered;
            }

            QString checkbox = QString("<input type=\"checkbox\" disabled %1>").arg(checked);
            processedLines.append("<li>" + checkbox + content + "</li>");
            currentIndent = indent;

        } else if (unorderedMatch.hasMatch()) {
            // Regular unordered list item
            QString content = unorderedMatch.captured(1);

            if (currentListType == Ordered) {
                processedLines.append("</ol>");
                currentListType = None;
            }
            if (currentListType == None) {
                processedLines.append("<ul>");
                currentListType = Unordered;
            } else if (indent > currentIndent) {
                // Nested list
                processedLines.append("<ul>");
            } else if (indent < currentIndent) {
                // End nested list
                processedLines.append("</ul>");
            }

            processedLines.append("<li>" + content + "</li>");
            currentIndent = indent;

        } else if (orderedMatch.hasMatch()) {
            // Ordered list item
            QString content = orderedMatch.captured(2);

            if (currentListType == Unordered) {
                processedLines.append("</ul>");
                currentListType = None;
            }
            if (currentListType == None) {
                processedLines.append("<ol>");
                currentListType = Ordered;
            } else if (indent > currentIndent) {
                // Nested list
                processedLines.append("<ol>");
            } else if (indent < currentIndent) {
                // End nested list
                processedLines.append("</ol>");
            }

            processedLines.append("<li>" + content + "</li>");
            currentIndent = indent;

        } else {
            // Not a list item - close any open lists
            if (currentListType == Unordered) {
                processedLines.append("</ul>");
                currentListType = None;
            } else if (currentListType == Ordered) {
                processedLines.append("</ol>");
                currentListType = None;
            }
            currentIndent = 0;
            processedLines.append(line);
        }
    }

    // Close any remaining open list
    if (currentListType == Unordered) {
        processedLines.append("</ul>");
    } else if (currentListType == Ordered) {
        processedLines.append("</ol>");
    }

    return processedLines.join('\n');
}

QString PreviewWidget::processInlineCode(const QString &text)
{
    QString result = text;

    // Use a unique placeholder to protect code content from further processing
    static const QString CODE_PLACEHOLDER = "\x01CODE";
    static const QString CODE_END = "ENDCODE\x01";

    QRegularExpression codeRegex("`([^`]+)`");
    QRegularExpressionMatchIterator i = codeRegex.globalMatch(text);

    int placeholderIndex = 0;
    QMap<QString, QString> codePlaceholders;

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString code = match.captured(1);

        // Don't process any markdown inside code
        QString placeholder = CODE_PLACEHOLDER + QString::number(placeholderIndex++) + CODE_END;
        QString replacement = "<code>" + code + "</code>";

        codePlaceholders[placeholder] = replacement;
        result.replace(match.captured(0), placeholder);
    }

    // Restore code placeholders
    for (auto it = codePlaceholders.begin(); it != codePlaceholders.end(); ++it) {
        result.replace(it.key(), it.value());
    }

    // Restore escaped backticks (done in restoreEscapedHtml)

    return result;
}
