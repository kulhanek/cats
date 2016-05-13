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
 *
 * CATs developed by: Petr Kulhánek, kulhanek@chemi.muni.cz
 * CATs IDE developed by: Jaroslav Oľha, jaroslav.olha@gmail.com
 * =====================================================================
 */

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include <QTemporaryFile>
#include <QDir>
#include <fstream>
#include <iostream>
#include <string>
#include <QtScript>
#include <QScriptable>
#include <QScriptValue>
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QWebView>
#include "SyntaxHighlighter.hpp"
#include "StdoutWatcher.hpp"
#include "JSEngineThread.hpp"
#include "CodeEditor.hpp"
#include "ui_IDEWindow.h"

class CIDEWindow : public QMainWindow
{
    Q_OBJECT

public:
    //Initializes the main window object.
    CIDEWindow(QWidget *parent = 0);

    //Saves the layout of the debugger widget.
    ~CIDEWindow();

public slots:
    //Slots connected to the GUI buttons:
    void LoadFile(void);                //"Load script..."
    void SaveFile(void);                //"Save script..."
    void SaveFileAs(void);              //"Save script as..."
    void ExitProgram(void);             //"Exit"
    void SetWorkingDirectory(void);     //"Change working directory..."
    void RunScript(void);               //"Run"
    void DebugScript(void);             //"Debug"
    void AbortEvaluation(void);         //"Abort"
    void SwitchToEditor(void);          //"Editor"
    void SwitchToDebugger(void);        //"Debugger"
    void SwitchToJSHelp();              //"JavaScript help"
    void SwitchToCATsHelp();            //"CATs help"
    void SwitchToAbout(void);           //"About"
    void ShowWebBrowser(void);          //"Web browser for CATs and JavaScript reference"
    void WorkingDirAutoSetPolicy(void); //"Script location is the working directory"
    void DeleteLayout(void);            //"Restore default debugger layout"

    //Slot reacting to JSEngineThread's finished signal.
    //Aborts output redirection by stopping the StdoutWatcher object's thread.
    void TerminateStdoutWatcher(void);

    //Slot reacting to JSEngineThread's UncaughtError signal.
    //Prints the error message to the output widget.
    void PrintErrors(QString errorMessage);

    //Slot reacting to StdoutWatcher's LineRead signal.
    //Prints the line to the output widget.
    void WriteLine(QString line);

signals:
    //Sent to the JSEngineThread to abort its evaluation in progress.
    void AbortSignal(void);

private:
    //The method called automatically when the main window is closed.
    //If there are unsaved changes, a prompt asks the user whether they want to save.
    void closeEvent(QCloseEvent *event);

    //Creates the code editor and its syntax highlighter.
    void SetupEditor(void);

    //Connects all the GUI signals to their slot methods.
    void SetupMenu(void);

    //Loads a web page and displays it in the web browser widget.
    void LoadWebPage(QString url);

    //Disables some of the GUI buttons while the script is running.
    void BlockButtons(void);

    //Reenables the GUI buttons.
    void UnblockButtons(void);

    //Sets the working directory of the script to the path where the script is located.
    void SetWorkingDirectoryAtScriptLocation(std::string scriptPath);

    //The main UI window.
    Ui::IDEWindow Ui;

    //The path to of the current working directory.
    std::string WorkingDir;

    //The path to the currently loaded script.
    std::string CurrentFile;

    //The object responsible for evaluating scripts in a thread.
    CJSEngineThread *JSEngineThread;

    //The object responsible for output redirection.
    CStdoutWatcher *StdoutWatcher;

    //The URL of the currently loaded web page.
    QString CurrentWebPage;

    //The browser widget.
    QWebView *WebBrowser;

    //The engine used for debugging.
    QScriptEngine *DebuggerEngine;

    //The debugger.
    QScriptEngineDebugger *Debugger;

    //The variables that hold the initial state of the debugger widget.
    //Only used if the user wants to restore the default debugger layout.
    QByteArray DefaultGeometry;
    QByteArray DefaultState;

    //Holds the value of the "Script location is the working directory" setting.
    bool AutoSetWorkingDir;

    //The temporary file used for output redirection.
    QTemporaryFile *AuxFile;

    //The path to the temporary file.
    QString AuxFilePath;

    //The file stream of the temporary file.
    QTextStream AuxFileStream;

    //The code editor widget.
    QPlainTextEdit *Editor;

    //The highlighter attached to the editor widget.
    CSyntaxHighlighter *Highlighter;
};

#endif // IDEWINDOW_HPP
