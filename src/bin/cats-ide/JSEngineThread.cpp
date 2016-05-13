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

#include "JSEngineThread.hpp"

CJSEngineThread::CJSEngineThread(QObject *parent)
    : QThread(parent)
{
    JSEngine = NULL;
    connect(parent, SIGNAL(AbortSignal()), this, SLOT(AbortEvaluation()));
}

void CJSEngineThread::RunCode(const QString &code)
{
    JSEngine = new QScriptEngine();

    //Import the CATs classes and methods into the engine.
    RegisterAllCATsClasses(*JSEngine);

    //If the code's first line begins with #!, disable the line (turn into a comment)
    QString firstLine = code.split(QRegExp("[\r\n]"),QString::SkipEmptyParts)[0];
    if (firstLine.contains("#!"))
    {
        JSCode = "//" + code;
    }
    else
    {
        JSCode = code;
    }

    //Run a syntax check.
    QScriptSyntaxCheckResult syntaxCheck = JSEngine->checkSyntax(JSCode);

    //If the syntax check failed, send the error message as a signal to the main window and exit.
    if (syntaxCheck.state() != QScriptSyntaxCheckResult::Valid)
    {
        QString errorMessage = QString("Syntax error at line %1: %2").arg(syntaxCheck.errorLineNumber()).arg(syntaxCheck.errorMessage());
        emit UncaughtError(errorMessage);
        delete JSEngine;
        JSEngine = NULL;
        return;
    }

    //Run the thread.
    start();
}

void CJSEngineThread::run()
{
    JSEngine->clearExceptions();

    Result = JSEngine->evaluate(JSCode);

    //If the evaluation ended with an error, send the error message as a signal to the main window and exit.
    //Otherwise, the output is redirected automatically via StdoutWatcher.
    if (JSEngine->hasUncaughtException())
    {
        int line = JSEngine->uncaughtExceptionLineNumber();
        QString err = Result.toString();
        QString msg = QString("Error at line %1: %2").arg(line).arg(err);

        emit UncaughtError(msg);
    }

    delete JSEngine;

    JSEngine = NULL;
}

void CJSEngineThread::AbortEvaluation()
{
    JSEngine->abortEvaluation("ABORTED");
}
