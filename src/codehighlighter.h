#ifndef CODEHIGHLIGHTER_H
#define CODEHIGHLIGHTER_H

#include <QString>
#include <QMap>

class CodeHighlighter
{
public:
    static QString highlightCode(const QString &code, const QString &language);

private:
    static QString highlightKeywords(const QString &code, const QStringList &keywords, const QString &color);
    static QString highlightStrings(const QString &code);
    static QString highlightComments(const QString &code, const QString &language);
    static QString highlightNumbers(const QString &code);

    static QStringList getKeywordsForLanguage(const QString &language);
};

#endif // CODEHIGHLIGHTER_H
