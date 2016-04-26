#ifndef CODEEDITOR_HPP
#define CODEEDITOR_HPP
/* =====================================================================
 * This file is part of CATs - Conversion and Analysis Tools.
 *
 * Most of this file is taken from the official Qt documentation, at:
 *
 * http://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html
 *
 * As such, it is licensed under the terms of the GNU Free Documentation
 * License version 1.3 as published by the Free Software Foundation.
 *
 * Certain parts were slightly edited as part of the CATs IDE development.
 *
 * CATs developed by: RNDr. Petr Kulhánek, PhD.
 * CATs IDE developed by: Mgr. Jaroslav Oľha
 *
 * =====================================================================
 */

#include <QPlainTextEdit>
#include <QTextBlock>
#include <QPainter>

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const Q_DECL_OVERRIDE {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

#endif // CODEEDITOR_HPP
