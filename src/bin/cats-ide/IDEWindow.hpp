#ifndef IDEWINDOW_HPP
#define IDEWINDOW_HPP
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

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include <QTemporaryFile>
#include <QDir>
#include <fstream>
#include <iostream>
#include <string>
#include <QtScript/QtScript>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptable>
#include <QWebView>
#include <unistd.h>
#include "SyntaxHighlighter.hpp"
#include "StdoutWatcher.hpp"
#include "JSEngineThread.hpp"
#include "ui_IDEWindow.h"

class CIDEWindow : public QMainWindow
{
    Q_OBJECT

public:
    CIDEWindow(QWidget *parent = 0);
    ~CIDEWindow();

public slots:
    void LoadFile(void);
    void SaveFile(void);
    void SaveFileAs(void);
    void ExitProgram(void);
    void SetWorkingDirectory(void);
    void RunScript(void);
    void DebugScript(void);
    void AbortEvaluation(void);
    void SwitchToEditor(void);
    void SwitchToDebugger(void);
    void SwitchToJSHelp();
    void SwitchToCATsHelp();
    void PrintErrors(QString errorMessage);
    void WriteLine(QString line);
    void ShowWebBrowser(void);
    void SwitchToAbout(void);
    void TerminateStdoutWatcher(void);
    void WorkingDirAutoSetPolicy(void);

signals:
    void AbortSignal(void);

private:
    void SetupEditor(void);
    void SetupMenu(void);
    void LoadWebPage(QString url);
    void BlockButtons(void);
    void UnblockButtons(void);
    Ui::IDEWindow Ui;
    std::string WorkingDir;
    std::string CurrentFile;
    CJSEngineThread *JSEngineThread;
    CStdoutWatcher *StdoutWatcher;
    QString CurrentWebPage;
    QWebView *WebBrowser;
    QScriptEngine *DebuggerEngine;
    QScriptEngineDebugger *Debugger;

    bool AutoSetWorkingDir;

    QString AuxFilePath;
    QTextStream AuxFileStream;
    QTemporaryFile *AuxFile;
    QFileSystemWatcher *Watcher;

    QPlainTextEdit *Editor;
    CSyntaxHighlighter *Highlighter;
};

#endif // IDEWINDOW_HPP
