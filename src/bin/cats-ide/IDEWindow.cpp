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

#include "IDEWindow.hpp"
#include "ui_IDEWindow.h"
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#ifndef QT_NO_SCRIPTTOOLS
#include <QtScriptTools/QScriptEngineDebugger>
#endif

CIDEWindow::CIDEWindow(QWidget *parent)
    : QMainWindow(parent)
{
    JSEngineThread = new CJSEngineThread(this);

    AuxFile = new QTemporaryFile();
    AuxFile->open();
    AuxFilePath = AuxFile->fileName();
    AuxFileStream.setDevice(AuxFile);

    Debugger = new QScriptEngineDebugger();
    DebuggerEngine = NULL;

    StdoutWatcher = new CStdoutWatcher(this, AuxFilePath);

    connect(StdoutWatcher, SIGNAL(LineRead(QString)),this,SLOT(WriteLine(QString)));

    Ui.setupUi(this);
    SetupMenu();
    SetupEditor();

    WorkingDir = "";
    CurrentFile = "";

    Ui.workingDirLabel->setText("Working directory not selected.");

    CurrentWebPage = "";
    WebBrowser = NULL;

    setWindowTitle(tr("CATs IDE"));

    Ui.stackedWidget->addWidget(Debugger->standardWindow());

    Ui.actionAutoSet_WD_to_script_path->setChecked(true);
    AutoSetWorkingDir = true;

    Ui.stackedWidget->setCurrentIndex(0);
}

void CIDEWindow::SetupMenu()
{
    connect(Ui.action_Open, SIGNAL(triggered()), this, SLOT(LoadFile()));
    connect(Ui.action_Save_script, SIGNAL(triggered()), this, SLOT(SaveFile()));
    connect(Ui.actionSave_script_as, SIGNAL(triggered()), this, SLOT(SaveFileAs()));
    connect(Ui.action_Exit, SIGNAL(triggered()), this, SLOT(ExitProgram()));
    connect(Ui.actionChange_working_directory, SIGNAL(triggered()), this, SLOT(SetWorkingDirectory()));
    connect(Ui.actionRun_script, SIGNAL(triggered()), this, SLOT(RunScript()));
    connect(Ui.actionDebug, SIGNAL(triggered()), this, SLOT(DebugScript()));
    connect(Ui.actionSwitch_to_Editor, SIGNAL(triggered()), this, SLOT(SwitchToEditor()));
    connect(Ui.actionSwitch_to_Debugger, SIGNAL(triggered()), this, SLOT(SwitchToDebugger()));
    connect(Ui.actionJavaScript_Help, SIGNAL(triggered()), this, SLOT(SwitchToJSHelp()));
    connect(Ui.actionCATs_Help, SIGNAL(triggered()), this, SLOT(SwitchToCATsHelp()));
    connect(Ui.actionReference, SIGNAL(triggered()), this, SLOT(ShowWebBrowser()));
    connect(Ui.actionAbout, SIGNAL(triggered()), this, SLOT(SwitchToAbout()));
    connect(Ui.actionAbort, SIGNAL(triggered()), this, SLOT(AbortEvaluation()));
    connect(Ui.actionAutoSet_WD_to_script_path, SIGNAL(triggered()), this, SLOT(WorkingDirAutoSetPolicy()));

    connect(JSEngineThread, SIGNAL(UncaughtError(QString)), this, SLOT(PrintErrors(QString)));
    connect(JSEngineThread, SIGNAL(finished()), this, SLOT(TerminateStdoutWatcher()));
}

void CIDEWindow::SetupEditor()
{
    CodeEditor *CE = new CodeEditor(Ui.splitter);
    CE->setSizePolicy(Ui.plainTextEdit->sizePolicy());
    CE->resize(Ui.plainTextEdit->size());
    Ui.horizontalLayout->replaceWidget(Ui.plainTextEdit, CE);
    Ui.plainTextEdit->hide();
    Ui.splitter->insertWidget(0,CE);
    Editor = CE;

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    Editor->setFont(font);

    Highlighter = new CSyntaxHighlighter(Editor->document());

    Editor->setFocus();
}

void CIDEWindow::LoadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load File..."), QString::fromStdString(WorkingDir), tr("All Files (*.*)"));
    if (fileName == "")
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0,"info",file.errorString());

    QTextStream in(&file);

    Editor->setPlainText(in.readAll());

    CurrentFile = fileName.toStdString();

    if ((WorkingDir == "") || AutoSetWorkingDir)
    {
        SetWorkingDirectoryAtScriptLocation(CurrentFile);
    }

    this->SwitchToEditor();
}

void CIDEWindow::SaveFile()
{
    QString fileName = QString::fromStdString(CurrentFile);

    if (fileName == "")
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File..."), QString::fromStdString(WorkingDir), tr("All Files (*.*)"));

        if (fileName == "")
        {
            return;
        }
        else
        {
            CurrentFile = fileName.toStdString();
        }
    }

    std::ofstream outFile(fileName.toStdString().c_str());

    outFile << Editor->toPlainText().toStdString();
    outFile.close();

    if ((WorkingDir == "") || AutoSetWorkingDir)
    {
        SetWorkingDirectoryAtScriptLocation(CurrentFile);
    }
}

void CIDEWindow::SaveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As..."), QString::fromStdString(WorkingDir), tr("All Files (*.*)"));

    if (fileName == "")
    {
        return;
    }
    else
    {
        CurrentFile = fileName.toStdString();
    }

    std::ofstream outFile(fileName.toStdString().c_str());

    outFile << Editor->toPlainText().toStdString();
    outFile.close();

    if ((WorkingDir == "") || AutoSetWorkingDir)
    {
        SetWorkingDirectoryAtScriptLocation(CurrentFile);
    }
}

void CIDEWindow::SetWorkingDirectory()
{
    QString dirName = QFileDialog::getExistingDirectory(this,tr("Choose working directory..."), QString::fromStdString(WorkingDir));

    if (dirName == "")
    {
        return;
    }
    else
    {
        WorkingDir = dirName.toStdString();
        QDir::setCurrent(dirName);
    }

    Ui.workingDirLabel->setText("Working directory: "+QString::fromStdString(WorkingDir));
}

void CIDEWindow::RunScript()
{
    BlockButtons();

    Ui.textBrowser->clear();

    StdoutWatcher->StartOutputRedirection();

    this->SwitchToEditor();

    JSEngineThread->RunCode(Editor->toPlainText());
}

void CIDEWindow::DebugScript()
{
    DebuggerEngine = new QScriptEngine();

    BlockButtons();

    this->SwitchToDebugger();

    RegisterAllCATsClasses(*DebuggerEngine);

    QString code = Editor->toPlainText();
    QString JSCode;

    if (code.size() > 0)
    {
        QString firstLine = code.split(QRegExp("[\r\n]"),QString::SkipEmptyParts)[0];
        if (firstLine.contains("#!"))
        {
            JSCode = "//" + code;
        }
        else
        {
            JSCode = code;
        }
    }

    DebuggerEngine->setProcessEventsInterval(10);

    Debugger->attachTo(DebuggerEngine);
    Debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();

    DebuggerEngine->evaluate(JSCode);
/*
    Ui.textBrowser->append("\n=======================\n DEBUGGER HAS FINISHED \n=======================\n\n");
    QTextCursor cursor = Ui.textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    Ui.textBrowser->ensureCursorVisible();

    Ui.stackedWidget->removeWidget(Debugger->standardWindow());
    this->SwitchToEditor();
*/
    Debugger->detach();
    delete DebuggerEngine;
    DebuggerEngine = NULL;

    UnblockButtons();
}

void CIDEWindow::AbortEvaluation()
{
    if (DebuggerEngine != NULL)
    {
        DebuggerEngine->abortEvaluation();
        Debugger->action(QScriptEngineDebugger::ContinueAction)->trigger();
    }
    else
    {
        emit AbortSignal();
    }
}

void CIDEWindow::PrintErrors(QString errorMessage)
{
    QTextCursor cursor = Ui.textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    Ui.textBrowser->append("\n======================================\nEVALUATION ABORTED:");
    Ui.textBrowser->append(errorMessage);
    Ui.textBrowser->append("======================================\n");
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    Ui.textBrowser->ensureCursorVisible();
    this->SwitchToEditor();

    UnblockButtons();
}

void CIDEWindow::SwitchToEditor()
{
    Ui.stackedWidget->setCurrentIndex(0);
    Ui.actionSwitch_to_Editor->setChecked(true);
    Ui.actionSwitch_to_Debugger->setChecked(false);
    Ui.actionReference->setChecked(false);
}

void CIDEWindow::SwitchToDebugger()
{
    Ui.stackedWidget->setCurrentWidget(Debugger->standardWindow());

    Ui.actionSwitch_to_Editor->setChecked(false);
    Ui.actionSwitch_to_Debugger->setChecked(true);
    Ui.actionReference->setChecked(false);
}

void CIDEWindow::ShowWebBrowser()
{
    Ui.actionReference->setChecked(true);
    Ui.actionSwitch_to_Debugger->setChecked(false);
    Ui.actionSwitch_to_Editor->setChecked(false);

    if (WebBrowser == NULL)
    {
        SwitchToJSHelp();
    }
    else
    {
        Ui.stackedWidget->setCurrentWidget(WebBrowser);
    }
}

void CIDEWindow::SwitchToJSHelp()
{
    LoadWebPage("https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference");
}

void CIDEWindow::SwitchToCATsHelp()
{
    LoadWebPage("https://lcc.ncbr.muni.cz/whitezone/development/cats/wiki/index.php/Documentation");
}

void CIDEWindow::SwitchToAbout()
{
    if (WebBrowser == NULL)
    {
        Ui.stackedWidget->setCurrentIndex(1);
    }
    else
    {
        Ui.stackedWidget->setCurrentIndex(2);
    }

    Ui.actionReference->setChecked(false);
    Ui.actionSwitch_to_Debugger->setChecked(false);
    Ui.actionSwitch_to_Editor->setChecked(false);
}

void CIDEWindow::LoadWebPage(QString url)
{
    if (CurrentWebPage == url)
    {
        Ui.stackedWidget->setCurrentWidget(WebBrowser);
        return;
    }

    CurrentWebPage = url;

    if (WebBrowser == NULL)
    {
        WebBrowser = new QWebView(this);
        Ui.stackedWidget->insertWidget(1,WebBrowser);
        WebBrowser->show();
    }

    Ui.stackedWidget->setCurrentWidget(WebBrowser);
    Ui.actionReference->setChecked(true);
    Ui.actionSwitch_to_Debugger->setChecked(false);
    Ui.actionSwitch_to_Editor->setChecked(false);
    WebBrowser->load(QUrl(url));
}

void CIDEWindow::TerminateStdoutWatcher()
{
    StdoutWatcher->Terminated = true;

    UnblockButtons();
}

CIDEWindow::~CIDEWindow()
{
    StdoutWatcher->Terminated = true;
    StdoutWatcher->wait();
}

void CIDEWindow::WriteLine(QString line)
{
    QTextCursor cursor = Ui.textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    Ui.textBrowser->setTextCursor(cursor);
    Ui.textBrowser->insertPlainText(line);
    qApp->processEvents();
}

void CIDEWindow::BlockButtons()
{
    Ui.actionAbort->setEnabled(true);
    Ui.actionChange_working_directory->setEnabled(false);
    Ui.actionDebug->setEnabled(false);
    Ui.actionRun_script->setEnabled(false);
    Ui.action_Open->setEnabled(false);

    if (DebuggerEngine != NULL)
        Editor->setReadOnly(true);
}

void CIDEWindow::UnblockButtons()
{
    Ui.actionAbort->setEnabled(false);
    Ui.actionChange_working_directory->setEnabled(true);
    Ui.actionDebug->setEnabled(true);
    Ui.actionRun_script->setEnabled(true);
    Ui.action_Open->setEnabled(true);

    Editor->setReadOnly(false);
}

void CIDEWindow::WorkingDirAutoSetPolicy()
{
    if (Ui.actionAutoSet_WD_to_script_path->isChecked())
    {
        AutoSetWorkingDir = true;

        SetWorkingDirectoryAtScriptLocation(CurrentFile);
    }
    else
    {
        AutoSetWorkingDir = false;
    }
}

void CIDEWindow::SetWorkingDirectoryAtScriptLocation(std::string scriptPath)
{
    QFileInfo info(QString::fromStdString(scriptPath));
    WorkingDir = info.filePath().toStdString();

    size_t found;
    found=WorkingDir.find_last_of("/\\");

    WorkingDir = WorkingDir.substr(0,found);

    QDir::setCurrent(QString::fromStdString(WorkingDir));

    Ui.workingDirLabel->setText("Working directory: "+QString::fromStdString(WorkingDir));
}

void CIDEWindow::ExitProgram()
{
    this->close();
}

/*
QScriptValue QtPrintFunction(QScriptContext *context, QScriptEngine *engine)
{
    QString result;
    for (int i = 0; i < context->argumentCount(); ++i) {
        if (i > 0)
            result.append(" ");
        result.append(context->argument(i).toString());
    }

    QScriptValue calleeData = context->callee().data();
    QTextBrowser *edit = qobject_cast<QTextBrowser*>(calleeData.toQObject());
    edit->append(result);

    return engine->undefinedValue();
}
*/
