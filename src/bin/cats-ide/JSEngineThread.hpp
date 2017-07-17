#ifndef JSENGINETHREAD_HPP
#define JSENGINETHREAD_HPP
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

#include <QCATs.hpp>
#include <qthread.h>
#include <QAction>

class CJSEngineThread : public QThread
{
    Q_OBJECT

private:
    //The script code to be evaluated.
    QString         JSCode;

    //The Qt Script / CATs engine.
    QScriptEngine   *JSEngine;

    //The engine's final result (only used if there was an error).
    QScriptValue    Result;

    //The part of the execution that runs in a thread.
    virtual void run();
public:
    //Connects the Abort signal from the main window to the slot in this class.
    CJSEngineThread(QObject *parent);

    //Prepares the engine and the source code and launches the thread.
    void RunCode(const QString &code);
public slots:
    //Attempts to abort the execution of the script.
    void AbortEvaluation();
signals:
    //Sends an error message to the main window.
    void UncaughtError(QString errorMessage);
};

#endif // JSENGINETHREAD_HPP
