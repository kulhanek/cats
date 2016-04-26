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
 * CATs developed by: RNDr. Petr Kulhánek, PhD.
 * CATs IDE developed by: Mgr. Jaroslav Oľha
 * =====================================================================
 */

#include <QScriptEngineDebugger>
#include <fstream>
#include <iostream>
#include <QCATs.hpp>
#include <qthread.h>
#include <QPlainTextEdit>
#include <QAction>


class CJSEngineThread : public QThread
{
    Q_OBJECT

private:
    QString         JSCode;
    QScriptEngine   *JSEngine;
    QScriptValue    Result;
    virtual void run();
public:
    CJSEngineThread(QObject *parent);
    void RunCode(const QString &code);
public slots:
    void AbortEvaluation();
signals:
    void UncaughtError(QString errorMessage);
};

#endif // JSENGINETHREAD_HPP
