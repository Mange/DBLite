#include "sqlhighlighter.hpp"

#include <QtGui>
#include <iostream>

SqlHighlighter::SqlHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightningRule rule;

    // Set the formats
    keywordFormat.          setForeground(Qt::blue);
    functionFormat.         setForeground(Qt::cyan);
    numberFormat.           setForeground(Qt::red);
    singleLineCommentFormat.setForeground(Qt::darkMagenta);
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
        QString line = keywordsFile.readLine();
        // We should totally ignore empty lines, dude!
        if (line.length() > 0)
        {
            QString keyword = QString("\\b%1\\b").arg(line.replace("\n", ""));
            rule.pattern = QRegExp(keyword, Qt::CaseInsensitive);
            highlightningRules.append(rule);
        }
    }
    keywordsFile.close();

    // Functions pattern
    //QString keywordPattern("\\b(select|insert|update|delete|grant|drop|use|replace|alter|in|asc|desc)\\b");
    //rule.pattern = QRegExp(keywordPattern, Qt::CaseInsensitive);
    //rule.format  = keywordFormat;

    //highlightningRules.append(rule);



    // Line comments pattern
    rule.pattern = QRegExp("^--[^\n]*");
    rule.format  = singleLineCommentFormat;

    highlightningRules.append(rule);

    // Multi-Line comments pattern
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression   = QRegExp("\\*/");
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
    if (previousBlockState() != 0)
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
