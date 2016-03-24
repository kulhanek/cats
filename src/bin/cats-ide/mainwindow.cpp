#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include "q_debugstream.h"
#ifndef QT_NO_SCRIPTTOOLS
#include <QtScriptTools/QScriptEngineDebugger>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupMenu();
    //setupHelpMenu();
    setupEditor();

    workingDir = "";
    currentFile = "";

    //setCentralWidget(editor);
    //setWindowTitle(tr("CATs"));
}

/*
MainWindow::~MainWindow()
{
    delete ui;
}*/

void MainWindow::setupMenu()
{
    connect(ui->action_Open, SIGNAL(triggered()), this, SLOT(loadFile()));
    connect(ui->action_Save_script, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(ui->actionSave_script_as, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    connect(ui->action_Exit, SIGNAL(triggered()), this, SLOT(exitProgram()));
    connect(ui->actionChange_working_directory, SIGNAL(triggered()), this, SLOT(setWorkingDirectory()));
    connect(ui->actionRun_script, SIGNAL(triggered()), this, SLOT(runScript()));
    connect(ui->actionDebug, SIGNAL(triggered()), this, SLOT(debugScript()));
    connect(ui->actionSwitch_to_Editor, SIGNAL(triggered()), this, SLOT(switchToEditor()));
    connect(ui->actionSwitch_to_Debugger, SIGNAL(triggered()), this, SLOT(switchToDebugger()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(refreshTabs()));

    connect(&st, SIGNAL(finished()), this, SLOT(printResults()));

    //ui->tab_3->setVisible(0);
    //ui->tab_4->setVisible(0);
}

void MainWindow::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    editor = ui->plainTextEdit;
    editor->setFont(font);

    highlighter = new Highlighter(editor->document());

    //ui->plainTextEdit->sizeHint()
}

void MainWindow::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load File..."), QString::fromStdString(workingDir), tr("All Files (*.*)"));
    if (fileName == "")
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0,"info",file.errorString());

    QTextStream in(&file);

    ui->plainTextEdit->setPlainText(in.readAll());

    currentFile = fileName.toStdString();

    if (workingDir == "")
    {
        QFileInfo info(fileName);
        workingDir = info.filePath().toStdString();
    }
}

void MainWindow::saveFile()
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

    outFile << ui->plainTextEdit->toPlainText().toStdString();
    outFile.close();
}

void MainWindow::saveFileAs()
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

    outFile << ui->plainTextEdit->toPlainText().toStdString();
    outFile.close();
}

void MainWindow::setWorkingDirectory()
{
    QString dirName = QFileDialog::getExistingDirectory(this,tr("Choose working directory..."), QString::fromStdString(workingDir));

    if (dirName == "")
    {
        return;
    }
    else
    {
        workingDir = dirName.toStdString();
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
void MainWindow::runScript()
{
    console_content = "";
    output_content = "";
    ui->textBrowser->clear();

    st.setCode(ui->plainTextEdit->toPlainText());

    st.registerCATs();

    st.start();
}

void MainWindow::printResults()
{
    if (st.getEngine()->hasUncaughtException())
    {
        int line = st.getEngine()->uncaughtExceptionLineNumber();
        QString err = st.getResult().toString();
        QString msg = QString("Error at line %1: %2").arg(line).arg(err);

        ui->textBrowser->setPlainText(msg);

        ui->tabWidget->setCurrentIndex(0);

        console_content = msg;

        refreshTabs();
    }
    else
    {ui->textBrowser->setPlainText(st.getOutput());
        ui->textBrowser->viewport()->update();

        output_content = ui->textBrowser->document()->toPlainText();

        size_t index = 0;
        while (true) {
            index = output_content.toStdString().find("\n\n", index);

            if (index == std::string::npos) break;

            output_content.replace(index, 3, "  \n");

            index += 3;
        }

        ui->tabWidget->setCurrentIndex(1);

        refreshTabs();
    }

    this->switchToEditor();
}

void MainWindow::debugScript()
{
    QScriptEngine JSEngine;

    QScriptEngineDebugger* debugger = new QScriptEngineDebugger();

    if (ui->stackedWidget->count() > 2)
    {
        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->count()-1);
        ui->stackedWidget->removeWidget(ui->stackedWidget->currentWidget());
    }

    ui->stackedWidget->addWidget(debugger->standardWindow());
    ui->stackedWidget->setCurrentWidget(debugger->standardWindow());

    RegisterAllCATsClasses(JSEngine); // ?? added by kulhanek

    debugger->attachTo(&JSEngine);
    debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();

    this->switchToDebugger();
    JSEngine.evaluate(ui->plainTextEdit->toPlainText());
}

void MainWindow::debugScriptInThread()
{
    QScriptEngineDebugger* debugger = new QScriptEngineDebugger();

    if (ui->stackedWidget->count() > 2)
    {
        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->count()-1);
        ui->stackedWidget->removeWidget(ui->stackedWidget->currentWidget());
    }

    ui->stackedWidget->addWidget(debugger->standardWindow());
    ui->stackedWidget->setCurrentWidget(debugger->standardWindow());

    debugger->attachTo(st.getEngine());
    debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();

    this->switchToDebugger();

    st.setCode(ui->plainTextEdit->toPlainText());
    st.start();
}

void MainWindow::switchToEditor()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->actionSwitch_to_Editor->setChecked(true);
    ui->actionSwitch_to_Debugger->setChecked(false);
}

void MainWindow::switchToDebugger()
{
    //If the debugger hasn't been launched, switch to "debugger not launched" message.
    //Otherwise show debugger.
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->count()-1);
    ui->actionSwitch_to_Editor->setChecked(false);
    ui->actionSwitch_to_Debugger->setChecked(true);
}

void MainWindow::refreshTabs()
{
    if (ui->tabWidget->currentIndex() == 0)
    {
        ui->textBrowser->setText(console_content);
    }

    if (ui->tabWidget->currentIndex() == 1)
    {
        ui->textBrowser->setText(output_content);
    }

    if (ui->tabWidget->currentIndex() == 2)
    {
        ui->textBrowser->setText(debugger_tools_content);
    }

    if (ui->tabWidget->currentIndex() == 3)
    {
        ui->textBrowser->setText(stack_view_content);
    }
}

void MainWindow::exitProgram()
{
    this->close();
}
