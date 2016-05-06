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
 * CATs developed by: RNDr. Petr Kulhánek, PhD.
 * CATs IDE developed by: Mgr. Jaroslav Oľha
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
    //Main objects are initialized.
    JSEngineThread = new CJSEngineThread(this);

    AuxFile = new QTemporaryFile();
    AuxFile->open();
    AuxFilePath = AuxFile->fileName();
    AuxFileStream.setDevice(AuxFile);

    Debugger = new QScriptEngineDebugger();
    DebuggerEngine = NULL;

    StdoutWatcher = new CStdoutWatcher(this, AuxFilePath);

    //Whenever the StdoutWatcher reads a line from the temporary file, it is sent
    //as a signal to the WriteLine slot (so that it can be written to the output window).
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

    //In the IDEWindow.ui file, the debugger window's place is occupied by the label - now the label
    //is hidden and replaced by the window.
    Ui.label->hide();
    Ui.splitter_2->insertWidget(0,Debugger->standardWindow());

    Ui.actionAutoSet_WD_to_script_path->setChecked(true);
    AutoSetWorkingDir = true;

    //On startup, always show the Editor view.
    Ui.stackedWidget->setCurrentIndex(0);
}

void CIDEWindow::SetupMenu()
{
    //GUI buttons are associated with corresponding actions.
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

    //JSEngineThread's signals are associated with the corresponding reactions by the GUI.
    connect(JSEngineThread, SIGNAL(UncaughtError(QString)), this, SLOT(PrintErrors(QString)));
    connect(JSEngineThread, SIGNAL(finished()), this, SLOT(TerminateStdoutWatcher()));
}

void CIDEWindow::SetupEditor()
{
    //In the IDEWindow.ui file, the CodeEditor's place is occupied by a placeholder QPlainTextEdit
    //object - now the placeholder's properties are copied into the newly-created CodeEditor, which
    //replaces it in the layout.
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

    //The custom syntax highlighter is attached to the CodeEditor.
    Highlighter = new CSyntaxHighlighter(Editor->document());

    //Ensures that the CodeEditor widget has the keyboard focus on startup.
    Editor->setFocus();
}

void CIDEWindow::LoadFile()
{
    //Opens the "Load File..." prompt.
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load File..."), QString::fromStdString(WorkingDir), tr("All Files (*.*)"));
    if (fileName == "")
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0,"info",file.errorString());

    QTextStream in(&file);

    //Prints the file's contents into the CodeEditor.
    Editor->setPlainText(in.readAll());

    CurrentFile = fileName.toStdString();

    //If the working directory has not been set yet, it is set to the loaded file's folder.
    //If the working directory has already been set before, it will only be changed if the
    //"Script location is the working directory" option is enabled.
    if ((WorkingDir == "") || AutoSetWorkingDir)
    {
        SetWorkingDirectoryAtScriptLocation(CurrentFile);
    }

    this->SwitchToEditor();
}

void CIDEWindow::SaveFile()
{
    QString fileName = QString::fromStdString(CurrentFile);

    //If the file does not already exist (i.e. it hasn't previously been loaded or saved), a file prompt is
    //opened so the file location can be chosen.
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

    //A file is opened and the entire content of the CodeEditor's text area is written.
    std::ofstream outFile(fileName.toStdString().c_str());

    outFile << Editor->toPlainText().toStdString();
    outFile.close();

    //If the working directory has not been set yet, it is set to the saved file's folder.
    //If the working directory has already been set before, it will only be changed if the
    //"Script location is the working directory" option is enabled.
    if ((WorkingDir == "") || AutoSetWorkingDir)
    {
        SetWorkingDirectoryAtScriptLocation(CurrentFile);
    }
}

void CIDEWindow::SaveFileAs()
{
    //Opens a file prompt so the file location can be chosen.
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

    //If the working directory has not been set yet, it is set to the saved file's folder.
    //If the working directory has already been set before, it will only be changed if the
    //"Script location is the working directory" option is enabled.
    if ((WorkingDir == "") || AutoSetWorkingDir)
    {
        SetWorkingDirectoryAtScriptLocation(CurrentFile);
    }
}

void CIDEWindow::SetWorkingDirectory()
{
    //Opens a directory prompt and sets the working directory to the selected path.
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

    //Changes the label above the output field, so that it displays the current working directory.
    Ui.workingDirLabel->setText("Working directory: "+QString::fromStdString(WorkingDir));
}

void CIDEWindow::RunScript()
{
    BlockButtons();

    Ui.textBrowser->clear();
    Ui.textBrowser_2->clear();

    StdoutWatcher->StartOutputRedirection();

    this->SwitchToEditor();

    JSEngineThread->RunCode(Editor->toPlainText());
}

void CIDEWindow::DebugScript()
{
    //The debugger has its own Qt Script engine.
    DebuggerEngine = new QScriptEngine();

    BlockButtons();

    Ui.textBrowser->clear();
    Ui.textBrowser_2->clear();

    StdoutWatcher->StartOutputRedirection();

    this->SwitchToDebugger();

    RegisterAllCATsClasses(*DebuggerEngine);

    QString code = Editor->toPlainText();
    QString JSCode;

    //If the code's first line begins with #!, the line is disabled (turned into a comment)
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

    //Allows the GUI to remain responsive.
    DebuggerEngine->setProcessEventsInterval(10);

    //Attaches the debugger to the engine and triggers an interrupt action (so that the engine's evaluation
    //will immediately stop at the first line).
    Debugger->attachTo(DebuggerEngine);
    Debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();

    //Launches the engine (it will be stopped immediately).
    DebuggerEngine->evaluate(JSCode);

    //Once the evaluate method is finished (the debugger has reached the end of code or it was aborted
    //by the Abort button), it is detached and the engine is deleted.
    Debugger->detach();
    delete DebuggerEngine;
    DebuggerEngine = NULL;

    Ui.textBrowser_2->append("\n=======================\n DEBUGGER HAS FINISHED \n=======================\n\n");
    QTextCursor cursor = Ui.textBrowser_2->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    Ui.textBrowser_2->ensureCursorVisible();

    //End of output redirection.
    TerminateStdoutWatcher();
}

void CIDEWindow::AbortEvaluation()
{
    //If the debugger engine exists, the debug is running, otherwise the evaluation is running in a thread.
    if (DebuggerEngine != NULL)
    {
        //Abort the evaluation, and trigger the Continue action on the debugger (so that the control
        //will return to the engine and it can abort).
        DebuggerEngine->abortEvaluation();
        Debugger->action(QScriptEngineDebugger::ContinueAction)->trigger();
    }
    else
    {
        //Send an Abort signal to the thread.
        emit AbortSignal();
    }
}

void CIDEWindow::PrintErrors(QString errorMessage)
{
    //If the JSEngineThread's run has ended with an error (either syntax error detected
    //before the evaluation, or an exception thrown during the evaluation), it sends a signal
    //with the message - this method is the slot that prints the message to the GUI.
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
    //Switches to the Editor view.
    Ui.stackedWidget->setCurrentIndex(0);

    Ui.actionSwitch_to_Editor->setChecked(true);
    Ui.actionSwitch_to_Debugger->setChecked(false);
    Ui.actionReference->setChecked(false);
}

void CIDEWindow::SwitchToDebugger()
{
    //Switches to the Debugger view.
    Ui.stackedWidget->setCurrentIndex(1);

    Ui.actionSwitch_to_Editor->setChecked(false);
    Ui.actionSwitch_to_Debugger->setChecked(true);
    Ui.actionReference->setChecked(false);
}

void CIDEWindow::ShowWebBrowser()
{
    //If the web browser has not been opened yet, it is created and the JavaScript reference page is loaded.
    //Otherwise, the already existing web browser is shown, with the page that had been loaded before.
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
    //Switches to the "About" screen.
    Ui.stackedWidget->setCurrentIndex(2);

    Ui.actionReference->setChecked(false);
    Ui.actionSwitch_to_Debugger->setChecked(false);
    Ui.actionSwitch_to_Editor->setChecked(false);
}

void CIDEWindow::LoadWebPage(QString url)
{
    //If the desired url matches the one already loaded, the web browser is shown
    //and nothing else happens.
    if (CurrentWebPage == url)
    {
        Ui.stackedWidget->setCurrentWidget(WebBrowser);
        Ui.actionReference->setChecked(true);
        Ui.actionSwitch_to_Debugger->setChecked(false);
        Ui.actionSwitch_to_Editor->setChecked(false);
        return;
    }

    CurrentWebPage = url;

    //If the web browser does not exist already, create it and insert it into the stacked widget.
    if (WebBrowser == NULL)
    {
        WebBrowser = new QWebView(this);
        Ui.stackedWidget->insertWidget(3,WebBrowser);
        WebBrowser->show();
    }

    //Display the web browser and load the new url.
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
    //Writes the given line to both text output widgets - the one in the Editor view,
    //and the one in the Debugger view. The cursor is moved to the end of the widget
    //twice - the first time to ensure that the line will be written to the end (in case
    //the user has moved the cursor elsewhere), the second time to ensure that the output
    //widget will scroll down to this new line.
    QTextCursor cursor = Ui.textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    Ui.textBrowser->setTextCursor(cursor);
    Ui.textBrowser->insertPlainText(line);
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    Ui.textBrowser->setTextCursor(cursor);
    Ui.textBrowser->ensureCursorVisible();

    cursor = Ui.textBrowser_2->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    Ui.textBrowser_2->setTextCursor(cursor);
    Ui.textBrowser_2->insertPlainText(line);
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    Ui.textBrowser_2->setTextCursor(cursor);
    Ui.textBrowser->ensureCursorVisible();
}

void CIDEWindow::BlockButtons()
{
    //Disables the main action buttons in the GUI, enables the Abort button.
    Ui.actionAbort->setEnabled(true);
    Ui.actionChange_working_directory->setEnabled(false);
    Ui.actionDebug->setEnabled(false);
    Ui.actionRun_script->setEnabled(false);
    Ui.action_Open->setEnabled(false);

    //If the debugger is running, the script cannot be edited.
    if (DebuggerEngine != NULL)
        Editor->setReadOnly(true);
}

void CIDEWindow::UnblockButtons()
{
    //Enables the main action buttons in the GUI, disables the Abort button.
    Ui.actionAbort->setEnabled(false);
    Ui.actionChange_working_directory->setEnabled(true);
    Ui.actionDebug->setEnabled(true);
    Ui.actionRun_script->setEnabled(true);
    Ui.action_Open->setEnabled(true);

    Editor->setReadOnly(false);
}

void CIDEWindow::WorkingDirAutoSetPolicy()
{
    //Reacts to the "Script location is the working directory" setting.
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
    //Change the application's working directory to the location of the script.
    QFileInfo info(QString::fromStdString(scriptPath));
    WorkingDir = info.filePath().toStdString();

    //Ignore the script file name (remove the part after the last slash).
    size_t found;
    found=WorkingDir.find_last_of("/\\");

    WorkingDir = WorkingDir.substr(0,found);

    //Set the working directory.
    QDir::setCurrent(QString::fromStdString(WorkingDir));

    //Change the label above the output widget.
    Ui.workingDirLabel->setText("Working directory: "+QString::fromStdString(WorkingDir));
}

void CIDEWindow::ExitProgram()
{
    this->close();
}
