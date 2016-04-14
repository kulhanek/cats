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

    RegisterAllCATsClasses(*JSEngine);

    QString firstLine = code.split(QRegExp("[\r\n]"),QString::SkipEmptyParts)[0];
    if (firstLine.contains("#!"))
    {
        JSCode = "//" + code;
    }
    else
    {
        JSCode = code;
    }

    QScriptSyntaxCheckResult syntaxCheck = JSEngine->checkSyntax(JSCode);

    if (syntaxCheck.state() != QScriptSyntaxCheckResult::Valid)
    {
        QString errorMessage = QString("Syntax error at line %1: %2").arg(syntaxCheck.errorLineNumber()).arg(syntaxCheck.errorMessage());
        emit UncaughtError(errorMessage);
        delete JSEngine;
        JSEngine = NULL;
        return;
    }

    start();
}

void CJSEngineThread::run()
{
    JSEngine->clearExceptions();

    Result = JSEngine->evaluate(JSCode);

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
