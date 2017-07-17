#ifndef SYNTAXHIGHLIGHTER_HPP
#define SYNTAXHIGHLIGHTER_HPP
/* =====================================================================
 * This file is part of CATs - Conversion and Analysis Tools.
 *
 * Based on the official Qt documentation, at:
 *
 * http://doc.qt.io/qt-5/qtwidgets-richtext-syntaxhighlighter-example.html
 *
 * As such, it is licensed under the terms of the GNU Free Documentation
 * License version 1.3 as published by the Free Software Foundation.
 *
 * Adapted for CATs/JavaScript as part of the CATs IDE development.
 *
 * CATs developed by: Petr Kulhánek, kulhanek@chemi.muni.cz
 * CATs IDE developed by: Jaroslav Oľha, jaroslav.olha@gmail.com
 *
 * =====================================================================
 */

#include <QSyntaxHighlighter>

class CSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    CSyntaxHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    struct HighlightingRule
    {
        //The regular expression that a string needs to match in order to be highlighted.
        QRegExp Pattern;

        //The highlighting font.
        QTextCharFormat Format;
    };
    QVector<HighlightingRule> HighlightingRules;

    QRegExp CommentStartExpression;
    QRegExp CommentEndExpression;

    QTextCharFormat KeywordFormat;
    QTextCharFormat ClassFormat;
    QTextCharFormat SingleLineCommentFormat;
    QTextCharFormat MultiLineCommentFormat;
    QTextCharFormat QuotationFormat;
    QTextCharFormat FunctionFormat;
};

#endif // SYNTAXHIGHLIGHTER_HPP
