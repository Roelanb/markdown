#include "codehighlighter.h"
#include <QRegularExpression>

QString CodeHighlighter::highlightCode(const QString &code, const QString &language)
{
    QString highlighted = code;

    // Get keywords for the language
    QStringList keywords = getKeywordsForLanguage(language.toLower());

    // Apply highlighting
    highlighted = highlightComments(highlighted, language.toLower());
    highlighted = highlightStrings(highlighted);
    highlighted = highlightNumbers(highlighted);
    highlighted = highlightKeywords(highlighted, keywords, "#0000FF");

    return highlighted;
}

QString CodeHighlighter::highlightKeywords(const QString &code, const QStringList &keywords, const QString &color)
{
    QString result = code;

    for (const QString &keyword : keywords) {
        QRegularExpression regex("\\b" + keyword + "\\b");
        result.replace(regex, QString("<span style=\"color: %1; font-weight: bold;\">%2</span>").arg(color, keyword));
    }

    return result;
}

QString CodeHighlighter::highlightStrings(const QString &code)
{
    QString result = code;

    // Double-quoted strings
    result.replace(QRegularExpression("\"([^\"]*)\""),
                   "<span style=\"color: #A31515;\">\"\\1\"</span>");

    // Single-quoted strings
    result.replace(QRegularExpression("'([^']*)'"),
                   "<span style=\"color: #A31515;\">'\\1'</span>");

    return result;
}

QString CodeHighlighter::highlightComments(const QString &code, const QString &language)
{
    QString result = code;

    // C-style comments
    if (language == "c" || language == "c++" || language == "cpp" || language == "java" ||
        language == "javascript" || language == "js" || language == "typescript" || language == "ts" ||
        language == "c#" || language == "csharp" || language == "go" || language == "rust" ||
        language == "kotlin" || language == "scala" || language == "swift" || language == "css") {

        // Multi-line comments
        result.replace(QRegularExpression("/\\*.*?\\*/", QRegularExpression::DotMatchesEverythingOption),
                       "<span style=\"color: #008000; font-style: italic;\">\\0</span>");

        // Single-line comments
        result.replace(QRegularExpression("//.*$", QRegularExpression::MultilineOption),
                       "<span style=\"color: #008000; font-style: italic;\">\\0</span>");
    }

    // Python, Ruby, Shell comments
    if (language == "python" || language == "py" || language == "ruby" || language == "shell" ||
        language == "bash" || language == "sh" || language == "makefile" || language == "cmake") {

        result.replace(QRegularExpression("#.*$", QRegularExpression::MultilineOption),
                       "<span style=\"color: #008000; font-style: italic;\">\\0</span>");
    }

    // SQL comments
    if (language == "sql") {
        result.replace(QRegularExpression("--.*$", QRegularExpression::MultilineOption),
                       "<span style=\"color: #008000; font-style: italic;\">\\0</span>");
    }

    // HTML, XML comments
    if (language == "html" || language == "xml") {
        result.replace(QRegularExpression("<!--.*?-->", QRegularExpression::DotMatchesEverythingOption),
                       "<span style=\"color: #008000; font-style: italic;\">\\0</span>");
    }

    return result;
}

QString CodeHighlighter::highlightNumbers(const QString &code)
{
    QString result = code;

    result.replace(QRegularExpression("\\b\\d+(\\.\\d+)?\\b"),
                   "<span style=\"color: #098658;\">\\0</span>");

    return result;
}

QStringList CodeHighlighter::getKeywordsForLanguage(const QString &language)
{
    QMap<QString, QStringList> languageKeywords;

    // C
    languageKeywords["c"] = {
        "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if",
        "int", "long", "register", "return", "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
    };

    // C++
    languageKeywords["c++"] = languageKeywords["cpp"] = languageKeywords["c"];
    languageKeywords["c++"] << "class" << "namespace" << "public" << "private" << "protected"
                            << "virtual" << "override" << "template" << "typename" << "using"
                            << "new" << "delete" << "this" << "try" << "catch" << "throw"
                            << "bool" << "true" << "false" << "nullptr" << "constexpr";

    // C#
    languageKeywords["c#"] = languageKeywords["csharp"] = {
        "abstract", "as", "base", "bool", "break", "byte", "case", "catch", "char",
        "checked", "class", "const", "continue", "decimal", "default", "delegate", "do",
        "double", "else", "enum", "event", "explicit", "extern", "false", "finally",
        "fixed", "float", "for", "foreach", "goto", "if", "implicit", "in", "int",
        "interface", "internal", "is", "lock", "long", "namespace", "new", "null",
        "object", "operator", "out", "override", "params", "private", "protected",
        "public", "readonly", "ref", "return", "sbyte", "sealed", "short", "sizeof",
        "stackalloc", "static", "string", "struct", "switch", "this", "throw", "true",
        "try", "typeof", "uint", "ulong", "unchecked", "unsafe", "ushort", "using",
        "virtual", "void", "volatile", "while"
    };

    // Java
    languageKeywords["java"] = {
        "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
        "class", "const", "continue", "default", "do", "double", "else", "enum",
        "extends", "final", "finally", "float", "for", "goto", "if", "implements",
        "import", "instanceof", "int", "interface", "long", "native", "new", "package",
        "private", "protected", "public", "return", "short", "static", "strictfp",
        "super", "switch", "synchronized", "this", "throw", "throws", "transient",
        "try", "void", "volatile", "while", "true", "false", "null"
    };

    // JavaScript
    languageKeywords["javascript"] = languageKeywords["js"] = {
        "async", "await", "break", "case", "catch", "class", "const", "continue",
        "debugger", "default", "delete", "do", "else", "export", "extends", "finally",
        "for", "function", "if", "import", "in", "instanceof", "let", "new", "return",
        "super", "switch", "this", "throw", "try", "typeof", "var", "void", "while",
        "with", "yield", "true", "false", "null", "undefined"
    };

    // TypeScript
    languageKeywords["typescript"] = languageKeywords["ts"] = languageKeywords["js"];
    languageKeywords["typescript"] << "interface" << "type" << "enum" << "namespace"
                                   << "module" << "declare" << "abstract" << "as"
                                   << "implements" << "private" << "protected" << "public"
                                   << "readonly" << "static";

    // Python
    languageKeywords["python"] = languageKeywords["py"] = {
        "False", "None", "True", "and", "as", "assert", "async", "await", "break",
        "class", "continue", "def", "del", "elif", "else", "except", "finally",
        "for", "from", "global", "if", "import", "in", "is", "lambda", "nonlocal",
        "not", "or", "pass", "raise", "return", "try", "while", "with", "yield"
    };

    // Ruby
    languageKeywords["ruby"] = {
        "BEGIN", "END", "alias", "and", "begin", "break", "case", "class", "def",
        "defined?", "do", "else", "elsif", "end", "ensure", "false", "for", "if",
        "in", "module", "next", "nil", "not", "or", "redo", "rescue", "retry",
        "return", "self", "super", "then", "true", "undef", "unless", "until",
        "when", "while", "yield"
    };

    // PHP
    languageKeywords["php"] = {
        "abstract", "and", "array", "as", "break", "callable", "case", "catch",
        "class", "clone", "const", "continue", "declare", "default", "die", "do",
        "echo", "else", "elseif", "empty", "enddeclare", "endfor", "endforeach",
        "endif", "endswitch", "endwhile", "eval", "exit", "extends", "final",
        "finally", "for", "foreach", "function", "global", "goto", "if", "implements",
        "include", "include_once", "instanceof", "insteadof", "interface", "isset",
        "list", "namespace", "new", "or", "print", "private", "protected", "public",
        "require", "require_once", "return", "static", "switch", "throw", "trait",
        "try", "unset", "use", "var", "while", "xor", "yield"
    };

    // Go
    languageKeywords["go"] = {
        "break", "case", "chan", "const", "continue", "default", "defer", "else",
        "fallthrough", "for", "func", "go", "goto", "if", "import", "interface",
        "map", "package", "range", "return", "select", "struct", "switch", "type",
        "var", "true", "false", "nil"
    };

    // Rust
    languageKeywords["rust"] = {
        "as", "async", "await", "break", "const", "continue", "crate", "dyn", "else",
        "enum", "extern", "false", "fn", "for", "if", "impl", "in", "let", "loop",
        "match", "mod", "move", "mut", "pub", "ref", "return", "self", "Self",
        "static", "struct", "super", "trait", "true", "type", "unsafe", "use",
        "where", "while"
    };

    // Kotlin
    languageKeywords["kotlin"] = {
        "abstract", "annotation", "as", "break", "by", "catch", "class", "companion",
        "const", "constructor", "continue", "crossinline", "data", "do", "else",
        "enum", "external", "false", "final", "finally", "for", "fun", "get", "if",
        "import", "in", "infix", "init", "inline", "inner", "interface", "internal",
        "is", "lateinit", "noinline", "null", "object", "open", "operator", "out",
        "override", "package", "private", "protected", "public", "return", "sealed",
        "set", "super", "suspend", "tailrec", "this", "throw", "true", "try", "typealias",
        "typeof", "val", "var", "vararg", "when", "where", "while"
    };

    // Scala
    languageKeywords["scala"] = {
        "abstract", "case", "catch", "class", "def", "do", "else", "extends",
        "false", "final", "finally", "for", "forSome", "if", "implicit", "import",
        "lazy", "match", "new", "null", "object", "override", "package", "private",
        "protected", "return", "sealed", "super", "this", "throw", "trait", "try",
        "true", "type", "val", "var", "while", "with", "yield"
    };

    // Swift
    languageKeywords["swift"] = {
        "associatedtype", "class", "deinit", "enum", "extension", "fileprivate",
        "func", "import", "init", "inout", "internal", "let", "open", "operator",
        "private", "protocol", "public", "static", "struct", "subscript", "typealias",
        "var", "break", "case", "continue", "default", "defer", "do", "else",
        "fallthrough", "for", "guard", "if", "in", "repeat", "return", "switch",
        "where", "while", "as", "catch", "false", "is", "nil", "rethrows", "super",
        "self", "Self", "throw", "throws", "true", "try"
    };

    // SQL
    languageKeywords["sql"] = {
        "SELECT", "FROM", "WHERE", "INSERT", "UPDATE", "DELETE", "CREATE", "ALTER",
        "DROP", "TABLE", "INDEX", "VIEW", "JOIN", "INNER", "LEFT", "RIGHT", "OUTER",
        "ON", "AND", "OR", "NOT", "NULL", "PRIMARY", "KEY", "FOREIGN", "REFERENCES",
        "CONSTRAINT", "UNIQUE", "DEFAULT", "CHECK", "AS", "ORDER", "BY", "GROUP",
        "HAVING", "LIMIT", "OFFSET", "UNION", "INTERSECT", "EXCEPT", "EXISTS", "IN",
        "BETWEEN", "LIKE", "DISTINCT", "COUNT", "SUM", "AVG", "MIN", "MAX"
    };

    // Shell
    languageKeywords["shell"] = languageKeywords["bash"] = languageKeywords["sh"] = {
        "if", "then", "else", "elif", "fi", "case", "esac", "for", "while", "until",
        "do", "done", "in", "function", "select", "time", "until", "echo", "exit",
        "return", "source", "export", "readonly", "local", "declare", "typeset"
    };

    return languageKeywords.value(language.toLower(), QStringList());
}
