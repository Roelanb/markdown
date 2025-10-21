#include "markdownhighlighter.h"

MarkdownHighlighter::MarkdownHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // Headings (ATX style: # ## ### etc.)
    headingFormat.setForeground(QColor("#4A90E2"));
    headingFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("^#{1,6}\\s+.*$");
    rule.format = headingFormat;
    rule.captureGroup = 0;
    highlightingRules.append(rule);

    // Bold (**text** or __text__)
    boldFormat.setFontWeight(QFont::Bold);
    boldFormat.setForeground(QColor("#E74C3C"));
    rule.pattern = QRegularExpression("(\\*\\*|__)(?=\\S)(.+?[*_]*)(?<=\\S)\\1");
    rule.format = boldFormat;
    rule.captureGroup = 0;
    highlightingRules.append(rule);

    // Italic (*text* or _text_)
    italicFormat.setFontItalic(true);
    italicFormat.setForeground(QColor("#E67E22"));
    rule.pattern = QRegularExpression("(?<!\\*)(\\*|_)(?=\\S)(.+?)(?<=\\S)\\1(?!\\*)");
    rule.format = italicFormat;
    rule.captureGroup = 0;
    highlightingRules.append(rule);

    // Strikethrough (~~text~~)
    strikethroughFormat.setFontStrikeOut(true);
    strikethroughFormat.setForeground(QColor("#95A5A6"));
    rule.pattern = QRegularExpression("~~(?=\\S)(.+?)(?<=\\S)~~");
    rule.format = strikethroughFormat;
    rule.captureGroup = 0;
    highlightingRules.append(rule);

    // Inline code (`code`)
    codeFormat.setForeground(QColor("#27AE60"));
    codeFormat.setBackground(QColor("#F0F0F0"));
    codeFormat.setFontFamilies(QStringList() << "Monospace" << "Courier New" << "Courier");
    rule.pattern = QRegularExpression("`[^`]+`");
    rule.format = codeFormat;
    rule.captureGroup = 0;
    highlightingRules.append(rule);

    // Links [text](url)
    linkFormat.setForeground(QColor("#3498DB"));
    linkFormat.setFontUnderline(true);
    rule.pattern = QRegularExpression("\\[([^\\]]+)\\]\\(([^)]+)\\)");
    rule.format = linkFormat;
    rule.captureGroup = 0;
    highlightingRules.append(rule);

    // Images ![alt](url)
    imageFormat.setForeground(QColor("#9B59B6"));
    rule.pattern = QRegularExpression("!\\[([^\\]]+)\\]\\(([^)]+)\\)");
    rule.format = imageFormat;
    rule.captureGroup = 0;
    highlightingRules.append(rule);

    // Unordered lists (-, *, +)
    listFormat.setForeground(QColor("#E74C3C"));
    listFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("^\\s*[-*+]\\s+");
    rule.format = listFormat;
    rule.captureGroup = 0;
    highlightingRules.append(rule);

    // Ordered lists (1., 2., etc.)
    rule.pattern = QRegularExpression("^\\s*\\d+\\.\\s+");
    rule.format = listFormat;
    rule.captureGroup = 0;
    highlightingRules.append(rule);

    // Blockquotes
    blockquoteFormat.setForeground(QColor("#7F8C8D"));
    blockquoteFormat.setFontItalic(true);
    rule.pattern = QRegularExpression("^>+\\s+.*$");
    rule.format = blockquoteFormat;
    rule.captureGroup = 0;
    highlightingRules.append(rule);

    // Horizontal rules (---, ***, ___)
    horizontalRuleFormat.setForeground(QColor("#BDC3C7"));
    horizontalRuleFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("^\\s*([-*_]){3,}\\s*$");
    rule.format = horizontalRuleFormat;
    rule.captureGroup = 0;
    highlightingRules.append(rule);

    // Tables (|)
    tableFormat.setForeground(QColor("#16A085"));
    rule.pattern = QRegularExpression("^\\|(.+)\\|$");
    rule.format = tableFormat;
    rule.captureGroup = 0;
    highlightingRules.append(rule);

    // Code blocks (``` or ~~~)
    codeBlockFormat.setForeground(QColor("#27AE60"));
    codeBlockFormat.setBackground(QColor("#F8F8F8"));
    codeBlockFormat.setFontFamilies(QStringList() << "Monospace" << "Courier New" << "Courier");
}

void MarkdownHighlighter::highlightBlock(const QString &text)
{
    // Apply regular highlighting rules
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            int start = match.capturedStart(rule.captureGroup);
            int length = match.capturedLength(rule.captureGroup);
            setFormat(start, length, rule.format);
        }
    }

    // Handle code blocks (multiline)
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) {
        QRegularExpression startExpression("^```");
        QRegularExpressionMatch match = startExpression.match(text);
        startIndex = match.hasMatch() ? match.capturedStart() : -1;
    }

    while (startIndex >= 0) {
        QRegularExpression endExpression("^```");
        QRegularExpressionMatch match = endExpression.match(text, startIndex + 3);
        int endIndex = match.capturedStart();
        int blockLength;

        if (endIndex == -1) {
            setCurrentBlockState(1);
            blockLength = text.length() - startIndex;
        } else {
            blockLength = endIndex - startIndex + match.capturedLength();
        }

        setFormat(startIndex, blockLength, codeBlockFormat);

        QRegularExpression nextStartExpression("^```");
        match = nextStartExpression.match(text, startIndex + blockLength);
        startIndex = match.hasMatch() ? match.capturedStart() : -1;
    }
}
