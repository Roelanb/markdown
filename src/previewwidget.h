#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QWidget>
#include <QTextBrowser>

class PreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewWidget(QWidget *parent = nullptr);
    ~PreviewWidget();

    void updatePreview(const QString &markdown);
    void scrollToPercentage(double percentage);

private:
    QString markdownToHtml(const QString &markdown);
    QString getStyleSheet();
    QString processEmojis(const QString &text);
    QString processCodeBlocks(const QString &text);
    QString processTables(const QString &text);
    QString convertTableToHtml(const QStringList &tableLines);
    QString processYamlFrontMatter(const QString &text);
    QString escapeHtmlInText(const QString &text);
    QString restoreEscapedHtml(const QString &text);
    QString processBackslashEscapes(const QString &text);
    QString processAutolinks(const QString &text);
    QString processBlockquotes(const QString &text);
    QString processLists(const QString &text);
    QString processInlineCode(const QString &text);

    QTextBrowser *webView;
};

#endif // PREVIEWWIDGET_H
