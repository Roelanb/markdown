#include "emojisupport.h"
#include <QRegularExpression>

QMap<QString, QString> EmojiSupport::emojiMap;
bool EmojiSupport::initialized = false;

void EmojiSupport::initializeEmojiMap()
{
    if (initialized) return;

    // Common emojis from GitHub's emoji list
    emojiMap[":smile:"] = "😄";
    emojiMap[":laughing:"] = "😆";
    emojiMap[":blush:"] = "😊";
    emojiMap[":smiley:"] = "😃";
    emojiMap[":relaxed:"] = "☺️";
    emojiMap[":smirk:"] = "😏";
    emojiMap[":heart_eyes:"] = "😍";
    emojiMap[":kissing_heart:"] = "😘";
    emojiMap[":kissing_closed_eyes:"] = "😚";
    emojiMap[":flushed:"] = "😳";
    emojiMap[":relieved:"] = "😌";
    emojiMap[":satisfied:"] = "😆";
    emojiMap[":grin:"] = "😁";
    emojiMap[":wink:"] = "😉";
    emojiMap[":stuck_out_tongue_winking_eye:"] = "😜";
    emojiMap[":stuck_out_tongue_closed_eyes:"] = "😝";
    emojiMap[":grinning:"] = "😀";
    emojiMap[":kissing:"] = "😗";
    emojiMap[":kissing_smiling_eyes:"] = "😙";
    emojiMap[":stuck_out_tongue:"] = "😛";
    emojiMap[":sleeping:"] = "😴";
    emojiMap[":worried:"] = "😟";
    emojiMap[":frowning:"] = "😦";
    emojiMap[":anguished:"] = "😧";
    emojiMap[":open_mouth:"] = "😮";
    emojiMap[":grimacing:"] = "😬";
    emojiMap[":confused:"] = "😕";
    emojiMap[":hushed:"] = "😯";
    emojiMap[":expressionless:"] = "😑";
    emojiMap[":unamused:"] = "😒";
    emojiMap[":sweat_smile:"] = "😅";
    emojiMap[":sweat:"] = "😓";
    emojiMap[":disappointed_relieved:"] = "😥";
    emojiMap[":weary:"] = "😩";
    emojiMap[":pensive:"] = "😔";
    emojiMap[":disappointed:"] = "😞";
    emojiMap[":confounded:"] = "😖";
    emojiMap[":fearful:"] = "😨";
    emojiMap[":cold_sweat:"] = "😰";
    emojiMap[":persevere:"] = "😣";
    emojiMap[":cry:"] = "😢";
    emojiMap[":sob:"] = "😭";
    emojiMap[":joy:"] = "😂";
    emojiMap[":astonished:"] = "😲";
    emojiMap[":scream:"] = "😱";
    emojiMap[":tired_face:"] = "😫";
    emojiMap[":angry:"] = "😠";
    emojiMap[":rage:"] = "😡";
    emojiMap[":triumph:"] = "😤";
    emojiMap[":sleepy:"] = "😪";
    emojiMap[":yum:"] = "😋";
    emojiMap[":mask:"] = "😷";
    emojiMap[":sunglasses:"] = "😎";
    emojiMap[":dizzy_face:"] = "😵";
    emojiMap[":imp:"] = "👿";
    emojiMap[":smiling_imp:"] = "😈";
    emojiMap[":neutral_face:"] = "😐";
    emojiMap[":no_mouth:"] = "😶";
    emojiMap[":innocent:"] = "😇";
    emojiMap[":alien:"] = "👽";
    emojiMap[":yellow_heart:"] = "💛";
    emojiMap[":blue_heart:"] = "💙";
    emojiMap[":purple_heart:"] = "💜";
    emojiMap[":heart:"] = "❤️";
    emojiMap[":green_heart:"] = "💚";
    emojiMap[":broken_heart:"] = "💔";
    emojiMap[":heartbeat:"] = "💓";
    emojiMap[":heartpulse:"] = "💗";
    emojiMap[":two_hearts:"] = "💕";
    emojiMap[":revolving_hearts:"] = "💞";
    emojiMap[":cupid:"] = "💘";
    emojiMap[":sparkling_heart:"] = "💖";
    emojiMap[":sparkles:"] = "✨";
    emojiMap[":star:"] = "⭐";
    emojiMap[":star2:"] = "🌟";
    emojiMap[":dizzy:"] = "💫";
    emojiMap[":boom:"] = "💥";
    emojiMap[":collision:"] = "💥";
    emojiMap[":anger:"] = "💢";
    emojiMap[":exclamation:"] = "❗";
    emojiMap[":question:"] = "❓";
    emojiMap[":grey_exclamation:"] = "❕";
    emojiMap[":grey_question:"] = "❔";
    emojiMap[":zzz:"] = "💤";
    emojiMap[":dash:"] = "💨";
    emojiMap[":sweat_drops:"] = "💦";
    emojiMap[":notes:"] = "🎶";
    emojiMap[":musical_note:"] = "🎵";
    emojiMap[":fire:"] = "🔥";
    emojiMap[":hankey:"] = "💩";
    emojiMap[":poop:"] = "💩";
    emojiMap[":shit:"] = "💩";
    emojiMap[":thumbsup:"] = "👍";
    emojiMap[":+1:"] = "👍";
    emojiMap[":thumbsdown:"] = "👎";
    emojiMap[":-1:"] = "👎";
    emojiMap[":ok_hand:"] = "👌";
    emojiMap[":punch:"] = "👊";
    emojiMap[":facepunch:"] = "👊";
    emojiMap[":fist:"] = "✊";
    emojiMap[":v:"] = "✌️";
    emojiMap[":wave:"] = "👋";
    emojiMap[":hand:"] = "✋";
    emojiMap[":raised_hand:"] = "✋";
    emojiMap[":open_hands:"] = "👐";
    emojiMap[":point_up:"] = "☝️";
    emojiMap[":point_down:"] = "👇";
    emojiMap[":point_left:"] = "👈";
    emojiMap[":point_right:"] = "👉";
    emojiMap[":raised_hands:"] = "🙌";
    emojiMap[":pray:"] = "🙏";
    emojiMap[":point_up_2:"] = "👆";
    emojiMap[":clap:"] = "👏";
    emojiMap[":muscle:"] = "💪";
    emojiMap[":rocket:"] = "🚀";
    emojiMap[":trophy:"] = "🏆";
    emojiMap[":checkered_flag:"] = "🏁";
    emojiMap[":tada:"] = "🎉";
    emojiMap[":confetti_ball:"] = "🎊";
    emojiMap[":balloon:"] = "🎈";
    emojiMap[":birthday:"] = "🎂";
    emojiMap[":gift:"] = "🎁";
    emojiMap[":bell:"] = "🔔";
    emojiMap[":no_bell:"] = "🔕";
    emojiMap[":tanabata_tree:"] = "🎋";
    emojiMap[":christmas_tree:"] = "🎄";
    emojiMap[":santa:"] = "🎅";
    emojiMap[":8ball:"] = "🎱";
    emojiMap[":alarm_clock:"] = "⏰";
    emojiMap[":apple:"] = "🍎";
    emojiMap[":art:"] = "🎨";
    emojiMap[":baby:"] = "👶";
    emojiMap[":back:"] = "🔙";
    emojiMap[":bamboo:"] = "🎍";
    emojiMap[":bangbang:"] = "‼️";
    emojiMap[":battery:"] = "🔋";
    emojiMap[":beer:"] = "🍺";
    emojiMap[":beers:"] = "🍻";
    emojiMap[":beetle:"] = "🐞";
    emojiMap[":beginner:"] = "🔰";
    emojiMap[":bike:"] = "🚲";
    emojiMap[":bird:"] = "🐦";
    emojiMap[":black_circle:"] = "⚫";
    emojiMap[":blue_book:"] = "📘";
    emojiMap[":boat:"] = "⛵";
    emojiMap[":bomb:"] = "💣";
    emojiMap[":book:"] = "📖";
    emojiMap[":bookmark:"] = "🔖";
    emojiMap[":books:"] = "📚";
    emojiMap[":bow:"] = "🙇";
    emojiMap[":bread:"] = "🍞";
    emojiMap[":briefcase:"] = "💼";
    emojiMap[":bug:"] = "🐛";
    emojiMap[":bulb:"] = "💡";
    emojiMap[":cake:"] = "🍰";
    emojiMap[":calendar:"] = "📆";
    emojiMap[":calling:"] = "📲";
    emojiMap[":camera:"] = "📷";
    emojiMap[":candy:"] = "🍬";
    emojiMap[":cat:"] = "🐱";
    emojiMap[":chart:"] = "💹";
    emojiMap[":cherry_blossom:"] = "🌸";
    emojiMap[":chocolate_bar:"] = "🍫";
    emojiMap[":clapper:"] = "🎬";
    emojiMap[":cloud:"] = "☁️";
    emojiMap[":coffee:"] = "☕";
    emojiMap[":computer:"] = "💻";
    emojiMap[":cookie:"] = "🍪";
    emojiMap[":crown:"] = "👑";
    emojiMap[":crystal_ball:"] = "🔮";
    emojiMap[":dart:"] = "🎯";
    emojiMap[":diamonds:"] = "♦️";
    emojiMap[":dog:"] = "🐶";
    emojiMap[":dollar:"] = "💵";
    emojiMap[":door:"] = "🚪";
    emojiMap[":email:"] = "📧";
    emojiMap[":x:"] = "❌";
    emojiMap[":white_check_mark:"] = "✅";
    emojiMap[":zap:"] = "⚡";

    initialized = true;
}

QString EmojiSupport::replaceEmojiCodes(const QString &text)
{
    initializeEmojiMap();

    QString result = text;

    // Replace all emoji codes with actual emoji characters
    QMapIterator<QString, QString> i(emojiMap);
    while (i.hasNext()) {
        i.next();
        result.replace(i.key(), i.value());
    }

    return result;
}
