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

#include "SyntaxHighlighter.hpp"

CSyntaxHighlighter::CSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    //Highlighting for the reserved JavaScript keywords.
    KeywordFormat.setForeground(Qt::darkBlue);
    KeywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\babstract\\b" << "\\barguments\\b" << "\\bboolean\\b"
                    << "\\bbreak\\b" << "\\bbyte\\b" << "\\bcase\\b"
                    << "\\bcatch\\b" << "\\bchar\\b" << "\\bclass\\b"
                    << "\\bconst\\b" << "\\bcontinue\\b" << "\\bdebugger\\b"
                    << "\\bdefault\\b" << "\\bdelete\\b" << "\\bdo\\b"
                    << "\\bdouble\\b" << "\\belse\\b" << "\\benum\\b"
                    << "\\beval\\b" << "\\bexport\\b" << "\\bextends\\b"
                    << "\\bfalse\\b" << "\\bfinal\\b" << "\\bfinally\\b"
                    << "\\bfloat\\b" << "\\bfor\\b" << "\\bfunction\\b"
                    << "\\bgoto\\b" << "\\bif\\b" << "\\bimplements\\b"
                    << "\\bimport\\b" << "\\bin\\b" << "\\binstanceof\\b"
                    << "\\bint\\b" << "\\binterface\\b" << "\\blet\\b"
                    << "\\blong\\b" << "\\bnative\\b" << "\\bnew\\b"
                    << "\\bnull\\b" << "\\bpackage\\b" << "\\bprivate\\b"
                    << "\\bprotected\\b" << "\\bpublic\\b" << "\\breturn\\b"
                    << "\\bshort\\b" << "\\bstatic\\b" << "\\bsuper\\b"
                    << "\\bswitch\\b" << "\\bsynchronized\\b" << "\\bthis\\b"
                    << "\\bthrow\\b" << "\\bthrows\\b" << "\\btransient\\b"
                    << "\\btrue\\b" << "\\btry\\b" << "\\btypeof\\b"
                    << "\\bvar\\b" << "\\bvoid\\b" << "\\bvolatile\\b"
                    << "\\bwhile\\b" << "\\bwith\\b" << "\\byield\\b";

    foreach (const QString &pattern, keywordPatterns) {
        rule.Pattern = QRegExp(pattern);
        rule.Format = KeywordFormat;
        HighlightingRules.append(rule);
    }

    //Highlighting for the functions.
    FunctionFormat.setFontItalic(true);
    FunctionFormat.setForeground(Qt::blue);
    rule.Pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.Format = FunctionFormat;
    HighlightingRules.append(rule);

    //Highlighting for the CATs class names.
    KeywordFormat.setForeground(Qt::darkMagenta);
    KeywordFormat.setFontItalic(true);
    QStringList CATsKeywords;
    CATsKeywords << "\\bAtom\\b" << "\\bAverageSnapshot\\b" << "\\bCATs\\b"
                    << "\\bCATsScriptable\\b" << "\\bCovarMatrix\\b" << "\\bGeometry\\b"
                    << "\\bHistogram\\b" << "\\bLinStat\\b" << "\\bMinMax\\b"
                    << "\\bMolSurf\\b" << "\\bNAHelper\\b" << "\\bNAStat\\b"
                    << "\\bNAStatHelper\\b" << "\\bNetResults\\b" << "\\bNetTrajectory\\b"
                    << "\\bOBMol\\b" << "\\bOFile\\b" << "\\bPMFLib\\b"
                    << "\\bPoint\\b" << "\\bPropSum\\b" << "\\bResidue\\b"
                    << "\\bRSelection\\b" << "\\bSelection\\b" << "\\bSimpleVector\\b"
                    << "\\bSnapshot\\b" << "\\bThermoIG\\b" << "\\bTopology\\b"
                    << "\\bTrajectory\\b" << "\\bTrajPool\\b" << "\\bTransformation\\b"
                    << "\\bVolumeData\\b" << "\\bx3DNA\\b";
    foreach (const QString &pattern, CATsKeywords) {
        rule.Pattern = QRegExp(pattern);
        rule.Format = KeywordFormat;
        HighlightingRules.append(rule);
    }

    //Highlighting for single line comments.
    SingleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.Pattern = QRegExp("//[^\n]*");
    rule.Format = SingleLineCommentFormat;
    HighlightingRules.append(rule);

    //Highlighting for multiple line comments.
    MultiLineCommentFormat.setForeground(Qt::darkGreen);
    CommentStartExpression = QRegExp("/\\*");
    CommentEndExpression = QRegExp("\\*/");

    //Highlighting for strings denoted by double quotes (escaped double quotes are ignored).
    QuotationFormat.setForeground(Qt::darkGreen);
    rule.Pattern = QRegExp("\"[^\"\\\\]*(\\\\.[^\"\\\\]*)*\"");
    rule.Format = QuotationFormat;
    HighlightingRules.append(rule);

    //Highlighting for strings denoted by single quotes (escaped single quotes are ignored).
    QuotationFormat.setForeground(Qt::darkGreen);
    rule.Pattern = QRegExp("\'[^\'\\\\]*(\\\\.[^\'\\\\]*)*\'");
    rule.Format = QuotationFormat;
    HighlightingRules.append(rule);
}


void CSyntaxHighlighter::highlightBlock(const QString &text)
{
    //Applies the highlighting rules which only apply for a single line (all except for multiple line comments).
    foreach (const HighlightingRule &rule, HighlightingRules) {
        QRegExp expression(rule.Pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.Format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    //Applies the highlighting rule for multiple line comments.
    int startIndex = 0;
        if (previousBlockState() != 1)
            startIndex = CommentStartExpression.indexIn(text);

        while (startIndex >= 0) {
                int endIndex = CommentEndExpression.indexIn(text, startIndex);
                int commentLength;
                if (endIndex == -1) {
                    setCurrentBlockState(1);
                    commentLength = text.length() - startIndex;
                } else {
                    commentLength = endIndex - startIndex
                                    + CommentEndExpression.matchedLength();
                }
                setFormat(startIndex, commentLength, MultiLineCommentFormat);
                startIndex = CommentStartExpression.indexIn(text, startIndex + commentLength);
            }
}
