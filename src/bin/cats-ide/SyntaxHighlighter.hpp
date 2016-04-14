#ifndef SYNTAXHIGHLIGHTER_HPP
#define SYNTAXHIGHLIGHTER_HPP
/* =====================================================================
 * This file is part of CATs - Conversion and Analysis Tools.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
