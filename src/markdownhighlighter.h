#ifndef MARKDOWNHIGHLIGHTER_H
#define MARKDOWNHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class MarkdownHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit MarkdownHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
        int captureGroup;
    };

    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat headingFormat;
    QTextCharFormat boldFormat;
    QTextCharFormat italicFormat;
    QTextCharFormat codeFormat;
    QTextCharFormat linkFormat;
    QTextCharFormat imageFormat;
    QTextCharFormat listFormat;
    QTextCharFormat blockquoteFormat;
    QTextCharFormat strikethroughFormat;
    QTextCharFormat tableFormat;
    QTextCharFormat horizontalRuleFormat;
    QTextCharFormat codeBlockFormat;
};

#endif // MARKDOWNHIGHLIGHTER_H
