#ifndef SQLHIGHLIGHTER_HPP
#define SQLHIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>

class QTextDocument;

class SqlHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT;

public:
    SqlHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightningRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightningRule> highlightningRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat stringFormat;
};

#endif // SQLHIGHLIGHTER_HPP
