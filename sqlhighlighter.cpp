#include "sqlhighlighter.hpp"

#include <QtGui>
#include <iostream>

SqlHighlighter::SqlHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightningRule rule;

    // Set the formats
    keywordFormat.          setForeground(Qt::blue);
    functionFormat.         setForeground(Qt::darkCyan);
    numberFormat.           setForeground(Qt::red);
    singleLineCommentFormat.setForeground(Qt::darkRed);
    multiLineCommentFormat. setForeground(Qt::darkMagenta);
    quotationFormat.        setForeground(Qt::darkYellow);
    stringFormat.           setForeground(Qt::darkGreen);

    // Keywords pattern. Load keywords from resource file.
    QFile keywordsFile(":/main/highlighter/sql_keywords.txt");
    rule.format  = keywordFormat;

    if (!keywordsFile.open(QIODevice::ReadOnly))
    {
        std::cerr << "Could not open keywords file!" << std::endl;
        exit(1);
    }

    while (!keywordsFile.atEnd())
    {
        QString line = keywordsFile.readLine().replace("\n", "");
        // We should totally ignore empty lines, dude!
        if (line.length() > 0)
        {
            QString keyword = QString("\\b%1\\b").arg(line);
            rule.pattern = QRegExp(keyword, Qt::CaseInsensitive, QRegExp::RegExp2);
            highlightningRules.append(rule);
        }
    }
    keywordsFile.close();

    // Functions pattern. Load functions from resource file.
    QFile functionsFile(":/main/highlighter/sql_functions.txt");
    rule.format  = functionFormat;

    if (!functionsFile.open(QIODevice::ReadOnly))
    {
        std::cerr << "Could not open functions file!" << std::endl;
        exit(1);
    }

    while (!functionsFile.atEnd())
    {
        QString line = functionsFile.readLine().replace("\n", "");
        // We should totally ignore empty lines, dude!
        if (line.length() > 0)
        {
            QString function = QString("\\b%1\\b").arg(line);
            rule.pattern = QRegExp(function, Qt::CaseInsensitive, QRegExp::RegExp2);
            highlightningRules.append(rule);
        }
    }
    functionsFile.close();

    // Quotation pattern
    QString quotationPattern("\\b`[a-z]*`\\b");
    rule.pattern = QRegExp(quotationPattern, Qt::CaseInsensitive, QRegExp::RegExp2);
    rule.format  = quotationFormat;
    highlightningRules.append(rule);

    // Number pattern
    QString numberPattern("\\b((-)?\\d+(\\.\\d+)?)\\b");
    rule.pattern = QRegExp(numberPattern, Qt::CaseInsensitive, QRegExp::RegExp2);
    rule.format  = numberFormat;
    highlightningRules.append(rule);

    // Line comments pattern
    rule.pattern = QRegExp("^--.*", Qt::CaseInsensitive, QRegExp::RegExp2);
    rule.format  = singleLineCommentFormat;
    highlightningRules.append(rule);

    /*
    Here is a complicated regexp, which is used for detecting quoted strings.
    SQLite uses Pascal-quoting, which is double-char instead of backslash-char like this:
      C-Style:      "A \"Quoted\" String"
      Pascal-style: "A ""Quoted"" String"
    The regexp works by first looking for 0-* chars that is not a quote, and then looking for
    a double-quote and anything else after that, repeated. Here's a bit more visual:
          "  A    ""   Quoted  ""   String  "
          ^-----^ ^----------^ ^-----------^
             1st       2nd         3rd
    The first part is just like a normal string. If the string starts with a double-qoute, this
    will be empty. 2nd and 3rd uses the same part of the regexp, and starts with a double-quote
    and continues on to the first quote or to end-of-string.

    SQLite also supports so called BLOB literals, which is just like a string but with a "x" in
    front.

    Finished regexp:
       x?"([^"]*(""[^"]*)*)("|$)

       x?"(               # Opening quote. Support BLOB literal
           [^"]*          # 1st part
           (""[^"]*)*     # 2nd and 3rd part. Double-quote and then a normal repeat.
       )("|$)             # Closing quote or end of string

    Now, SQLite uses this for both single and double quoted strings, so we need to reuse it a
    bit. We therefore replace the quote with a placeholder (%1). We also need to quote some chars
    for the C++ string literal.
    */
    QString quotedString("%1([^%1]*(%1%1[^%1]*)*)(%1|$)");

    // String pattern. Double quotes and single quotes
    rule.format  = stringFormat;
    rule.pattern = QRegExp(quotedString.arg('"'), Qt::CaseInsensitive, QRegExp::RegExp2);
    highlightningRules.append(rule);
    rule.pattern = QRegExp(quotedString.arg('\''), Qt::CaseInsensitive, QRegExp::RegExp2);
    highlightningRules.append(rule);

    // Multi-Line comments pattern
    commentStartExpression = QRegExp("/\\*", Qt::CaseInsensitive, QRegExp::RegExp2);
    commentEndExpression   = QRegExp("\\*/", Qt::CaseInsensitive, QRegExp::RegExp2);
}

void SqlHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightningRule &rule, highlightningRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index+length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
    {
        startIndex = commentStartExpression.indexIn(text);
    }

    while (startIndex >= 0)
    {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, endIndex + commentLength);
    }
}
