#ifndef QCATsH
#define QCATsH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//
//     This program is free software; you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation; either version 2 of the License, or
//     (at your option) any later version.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License along
//     with this program; if not, write to the Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// =============================================================================

#include <CATsMainHeader.hpp>
#include <QObject>
#include <QScriptValue>
#include <QScriptContext>
#include <QScriptable>
#include <string>
#include <SmallString.hpp>
#include <QCATsScriptable.hpp>
#include <QStringList>

//------------------------------------------------------------------------------

// main registrator handler
void CATS_PACKAGE RegisterAllCATsClasses(QScriptEngine& engine);

//------------------------------------------------------------------------------

/// expose internal methods

class CATS_PACKAGE QCATs : public QObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QCATs(void);
    static void Register(QScriptEngine& engine);
    static void SetScriptArgument(const QString& arg);

// methods ---------------------------------------------------------------------
public slots:
    /// execute system command
    /// int system(cmd)
    QScriptValue system(void);

    /// print all errors from ErrorSystem
    /// printErrors()
    QScriptValue printErrors(void);

    /// clear errors from ErrorSystem
    /// clearErrors()
    QScriptValue clearErrors(void);

    /// num of arguments
    /// int numOfArguments
    QScriptValue numOfArguments(void);

    /// get argument
    /// string argument(index)
    QScriptValue argument(void);
    
    /// include another javascript from file; files are searched 
    /// bool include(name)
    QScriptValue include(void);    

// global functions ------------------------------------------------------------
public:
    /// printf
    static QScriptValue printf(QScriptContext* p_context, QScriptEngine* p_engine);

    /// sprintf
    static QScriptValue sprintf(QScriptContext* p_context, QScriptEngine* p_engine);

    /// exit script execution
    static QScriptValue exit(QScriptContext* p_context, QScriptEngine* p_engine);

// section of private data and methods -----------------------------------------
private:
    static const std::string sprintf(const CSmallString& fname, QScriptContext* p_context, QScriptEngine* p_engine);

public:
    static int          ExitValue;
    static bool         ExitScript;
    static QStringList  ScriptArguments;
};

//------------------------------------------------------------------------------

#endif
