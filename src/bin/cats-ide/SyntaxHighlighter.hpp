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
 * CATs developed by: RNDr. Petr Kulhánek, PhD.
 * CATs IDE developed by: Mgr. Jaroslav Oľha
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
        QRegExp Pattern;
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
