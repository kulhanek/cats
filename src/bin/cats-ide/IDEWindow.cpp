#include "IDEWindow.hpp"
#include "ui_IDEWindow.h"
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include "StdoutWatcher.hpp"
#ifndef QT_NO_SCRIPTTOOLS
#include <QtScriptTools/QScriptEngineDebugger>
#endif

CIDEWindow::CIDEWindow(QWidget *parent)
    : QMainWindow(parent)
{
    JSEngineThread = new CJSEngineThread(this);

    //JSEngineThread = 0;

    ui.setupUi(this);
    setupMenu();
    //setupHelpMenu();
    setupEditor();

    workingDir = "";
    currentFile = "";

    //setCentralWidget(editor);
    setWindowTitle(tr("CATs IDE"));
}

void CIDEWindow::setupMenu()
{
    connect(ui.action_Open, SIGNAL(triggered()), this, SLOT(loadFile()));
    connect(ui.action_Save_script, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(ui.actionSave_script_as, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    connect(ui.action_Exit, SIGNAL(triggered()), this, SLOT(exitProgram()));
    connect(ui.actionChange_working_directory, SIGNAL(triggered()), this, SLOT(setWorkingDirectory()));
    connect(ui.actionRun_script, SIGNAL(triggered()), this, SLOT(runScript()));
    connect(ui.actionDebug, SIGNAL(triggered()), this, SLOT(debugScript()));
    connect(ui.actionSwitch_to_Editor, SIGNAL(triggered()), this, SLOT(switchToEditor()));
    connect(ui.actionSwitch_to_Debugger, SIGNAL(triggered()), this, SLOT(switchToDebugger()));
    connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(refreshTabs()));

    auxFilePath = "D:\\programy\\Git\\cats-build\\src\\projects\\cats\\2.0\\src\\bin\\cats-ide\\cats-output.txt";
    auxFile = new QFile(auxFilePath, this);
    auxFileStream.setDevice(auxFile);

    //connect(auxFile, SIGNAL(readyRead()), SLOT(Test()));

    connect(JSEngineThread, SIGNAL(finished()), this, SLOT(deleteAuxFile()));
}

/*void CIDEWindow::Test()
{

}*/

void CIDEWindow::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    editor = ui.plainTextEdit;
    editor->setFont(font);

    highlighter = new CSyntaxHighlighter(editor->document());
}

void CIDEWindow::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load File..."), QString::fromStdString(workingDir), tr("All Files (*.*)"));
    if (fileName == "")
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0,"info",file.errorString());

    QTextStream in(&file);

    ui.plainTextEdit->setPlainText(in.readAll());

    currentFile = fileName.toStdString();

    if (workingDir == "")
    {
        QFileInfo info(fileName);
        workingDir = info.filePath().toStdString();   

        size_t found;
        found=workingDir.find_last_of("/\\");

        QDir::setCurrent(QString::fromStdString(workingDir.substr(0,found)));
    }
}

void CIDEWindow::saveFile()
{
    QString fileName = QString::fromStdString(currentFile);

    if (fileName == "")
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File..."), QString::fromStdString(workingDir), tr("All Files (*.*)"));

        if (fileName == "")
        {
            return;
        }
        else
        {
            currentFile = fileName.toStdString();
        }
    }

    std::ofstream outFile(fileName.toStdString().c_str());

    outFile << ui.plainTextEdit->toPlainText().toStdString();
    outFile.close();
}

void CIDEWindow::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As..."), QString::fromStdString(workingDir), tr("All Files (*.*)"));

    if (fileName == "")
    {
        return;
    }
    else
    {
        currentFile = fileName.toStdString();
    }

    std::ofstream outFile(fileName.toStdString().c_str());

    outFile << ui.plainTextEdit->toPlainText().toStdString();
    outFile.close();
}

void CIDEWindow::setWorkingDirectory()
{
    QString dirName = QFileDialog::getExistingDirectory(this,tr("Choose working directory..."), QString::fromStdString(workingDir));

    if (dirName == "")
    {
        return;
    }
    else
    {
        workingDir = dirName.toStdString();
        QDir::setCurrent(dirName);
    }
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
void CIDEWindow::runScript()
{
    console_content = "";
    output_content = "";
    ui.textBrowser->clear();

    //remove(auxFilePath);

    //std::ofstream o(auxFilePath);
    //o << std::endl;

//auxFile = new QFile(auxFilePath, this);//&newFile;
//auxFileStream.setDevice(auxFile);
/*
if (st.isRunning())
    st.terminate();

while (!st.wait())
{
    sleep(0.1);
}*/

    // check if file changed
    watcher = new QFileSystemWatcher(this);
    watcher->addPath(auxFilePath);
    connect(watcher,SIGNAL(fileChanged(QString)),this,SLOT(HandleFileChange(QString)));

    JSEngineThread->RunCode(ui.plainTextEdit->toPlainText());
}

void CIDEWindow::deleteAuxFile()
{/*
    if (QCATsScriptable::CATsEngine->hasUncaughtException())
    {
        int line = QCATsScriptable::CATsEngine->uncaughtExceptionLineNumber();
        //QString err = st.getResult().toString();
        QString msg = QString("Error at line %1: %2").arg(line);//.arg(err);

        ui.textBrowser->setPlainText(msg);

        ui.tabWidget->setCurrentIndex(0);

        console_content = msg;

        refreshTabs();
    }

    QByteArray ba = auxFilePath.toLatin1();
    const char *c_str = ba.data();

    std::remove(c_str);*/
}

void CIDEWindow::printResults()
{
    if (QCATsScriptable::CATsEngine->hasUncaughtException())
    {
        int line = QCATsScriptable::CATsEngine->uncaughtExceptionLineNumber();
        QString err = JSEngineThread->GetResult().toString();
        QString msg = QString("Error at line %1: %2").arg(line).arg(err);

        ui.textBrowser->setPlainText(msg);

        //ui.tabWidget->setCurrentIndex(0);

        //console_content = msg;

        //refreshTabs();
    }
    else
    {/*ui.textBrowser->setPlainText(JSEngineThread->getOutput());
        ui.textBrowser->viewport()->update();

        output_content = ui.textBrowser->document()->toPlainText();

        size_t index = 0;
        while (true) {
            index = output_content.toStdString().find("\n\n", index);

            if (index == std::string::npos) break;

            output_content.replace(index, 3, "  \n");

            index += 3;
        }

        ui.tabWidget->setCurrentIndex(1);

        refreshTabs();*/
    }

    this->switchToEditor();
}

void CIDEWindow::debugScript()
{
    QScriptEngine* JSEngine;

    QScriptEngineDebugger* debugger = new QScriptEngineDebugger();

    if (ui.stackedWidget->count() > 2)
    {
        ui.stackedWidget->setCurrentIndex(ui.stackedWidget->count()-1);
        ui.stackedWidget->removeWidget(ui.stackedWidget->currentWidget());
    }

    ui.stackedWidget->addWidget(debugger->standardWindow());
    ui.stackedWidget->setCurrentWidget(debugger->standardWindow());

    debugger->attachTo(JSEngine);
    debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();

    this->switchToDebugger();
    JSEngine->evaluate(ui.plainTextEdit->toPlainText());
}

void CIDEWindow::debugScriptInThread()
{
    QScriptEngineDebugger* debugger = new QScriptEngineDebugger();

    if (ui.stackedWidget->count() > 2)
    {
        ui.stackedWidget->setCurrentIndex(ui.stackedWidget->count()-1);
        ui.stackedWidget->removeWidget(ui.stackedWidget->currentWidget());
    }

    ui.stackedWidget->addWidget(debugger->standardWindow());
    ui.stackedWidget->setCurrentWidget(debugger->standardWindow());

    debugger->attachTo(QCATsScriptable::CATsEngine);
    debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();

    this->switchToDebugger();

    JSEngineThread->RunCode(ui.plainTextEdit->toPlainText());
}

void CIDEWindow::switchToEditor()
{
    ui.stackedWidget->setCurrentIndex(0);
    ui.actionSwitch_to_Editor->setChecked(true);
    ui.actionSwitch_to_Debugger->setChecked(false);
}

void CIDEWindow::switchToDebugger()
{
    //If the debugger hasn't been launched, switch to "debugger not launched" message.
    //Otherwise show debugger.
    ui.stackedWidget->setCurrentIndex(ui.stackedWidget->count()-1);
    ui.actionSwitch_to_Editor->setChecked(false);
    ui.actionSwitch_to_Debugger->setChecked(true);
}

void CIDEWindow::HandleFileChange(QString change)
{/*
    QFile file(auxFilePath);

    if (!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);

    output_content = in.readAll();

    ui.tabWidget->setCurrentIndex(1);

    refreshTabs();

    qApp->processEvents();*/



/*    QString output;
    while (!auxFile->atEnd())
    {
        output = auxFileStream.readLine();
        output += "\n";
        ui.textBrowser->append(output);
        //output_content.append(output);
    }
  */




    qApp->processEvents();
/*
QString line = "";
while (!line.isNull())
{
    line = auxFileStream.readLine();
    output_content.append(line);
}*/
/*
do {
    line = auxFileStream.readLine();
} while (!line.isNull());*/

    //ui.tabWidget->setCurrentIndex(1);

    //refreshTabs();

    //ui.plainTextEdit->setPlainText(in.readAll());
}

void CIDEWindow::refreshTabs()
{
    if (ui.tabWidget->currentIndex() == 0)
    {
        ui.textBrowser->setText(console_content);
    }

    if (ui.tabWidget->currentIndex() == 1)
    {
        ui.textBrowser->setText(output_content);
    }

    if (ui.tabWidget->currentIndex() == 2)
    {
        ui.textBrowser->setText(debugger_tools_content);
    }

    if (ui.tabWidget->currentIndex() == 3)
    {
        ui.textBrowser->setText(stack_view_content);
    }

    qApp->processEvents();
}

void CIDEWindow::exitProgram()
{
    this->close();
}
