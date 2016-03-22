#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include <fstream>
#include <QtScript/QtScript>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptable>
#include <unistd.h>
#include "highlighter.h"
#include "q_debugstream.h"
#include "scriptthread.hpp"

namespace Ui {
class MainWindow;
}
/*
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

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
    void printResults();
    //QScriptValue printToWindow(QScriptContext *context, QScriptEngine *engine);
    //void about();
    //void newFile();
    //void openFile(const QString &path = QString());

private:
    void setupEditor();
    void setupMenu();
    //void setupHelpMenu();
    Ui::MainWindow *ui;
    std::string workingDir;
    std::string currentFile;
    QString console_content;
    QString output_content;
    QString debugger_tools_content;
    QString stack_view_content;
    ScriptThread st;

    QPlainTextEdit *editor;
    Highlighter *highlighter;
};

#endif // MAINWINDOW_H
