#ifndef EMOJISUPPORT_H
#define EMOJISUPPORT_H

#include <QString>
#include <QMap>

class EmojiSupport
{
public:
    static QString replaceEmojiCodes(const QString &text);
    static void initializeEmojiMap();

private:
    static QMap<QString, QString> emojiMap;
    static bool initialized;
};

#endif // EMOJISUPPORT_H
