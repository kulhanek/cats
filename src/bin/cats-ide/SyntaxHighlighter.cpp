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

#include "SyntaxHighlighter.hpp"

CSyntaxHighlighter::CSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    KeywordFormat.setForeground(Qt::darkBlue);
    KeywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.Pattern = QRegExp(pattern);
        rule.Format = KeywordFormat;
        HighlightingRules.append(rule);
    }
/*
    KeywordFormat.setForeground(Qt::darkMagenta);
    KeywordFormat.setFontWeight(QFont::Bold);
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
    }*/







    ClassFormat.setFontWeight(QFont::Bold);
       ClassFormat.setForeground(Qt::darkMagenta);
       rule.Pattern = QRegExp("\\bQ[A-Za-z]+\\b");
       rule.Format = ClassFormat;
       HighlightingRules.append(rule);



       FunctionFormat.setFontItalic(true);
       FunctionFormat.setForeground(Qt::blue);
       rule.Pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
       rule.Format = FunctionFormat;
       HighlightingRules.append(rule);





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


       QuotationFormat.setForeground(Qt::darkGreen);
       //rule.Pattern = QRegExp("\".*\"");
       rule.Pattern = QRegExp("\"[^\"]*\"");
       rule.Format = QuotationFormat;
       HighlightingRules.append(rule);


       SingleLineCommentFormat.setForeground(Qt::red);
           rule.Pattern = QRegExp("//[^\n]*");
           rule.Format = SingleLineCommentFormat;
           HighlightingRules.append(rule);

           MultiLineCommentFormat.setForeground(Qt::red);

           CommentStartExpression = QRegExp("/\\*");
           CommentEndExpression = QRegExp("\\*/");

}


void CSyntaxHighlighter::highlightBlock(const QString &text)
{
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
