#ifndef IDEWINDOW_HPP
#define IDEWINDOW_HPP

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include <QDir>
#include <fstream>
#include <iostream>
#include <string>
#include <QtScript/QtScript>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptable>
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

public slots:
    void loadFile();
    void saveFile();
    void saveFileAs();
    void exitProgram();
    void setWorkingDirectory();
    void runScript();
    void debugScript();
    void debugScriptInThread();
    void switchToEditor();
    void switchToDebugger();
    void refreshTabs();
    void deleteAuxFile();
    void printResults();
    void HandleFileChange(QString change);
    //void Test();
    //QScriptValue printToWindow(QScriptContext *context, QScriptEngine *engine);
    //void about();
    //void newFile();
    //void openFile(const QString &path = QString());

private:
    void setupEditor();
    void setupMenu();
    //void setupHelpMenu();
    Ui::IDEWindow ui;
    std::string workingDir;
    std::string currentFile;
    QString console_content;
    QString output_content;
    QString debugger_tools_content;
    QString stack_view_content;
    CJSEngineThread *JSEngineThread;

    QString auxFilePath;
    QTextStream auxFileStream;
    QFile *auxFile;
    QFileSystemWatcher *watcher;

    QPlainTextEdit *editor;
    CSyntaxHighlighter *highlighter;
};

#endif // IDEWINDOW_HPP
