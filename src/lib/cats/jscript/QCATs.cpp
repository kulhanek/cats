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

#include <iostream>
#include <stdlib.h>
#include <QScriptEngine>
#include <QCATs.hpp>
#include <QCATs.moc>
#include <boost/format.hpp>
#include <ErrorSystem.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

using namespace std;

//------------------------------------------------------------------------------

int         QCATs::ExitValue = 0;
bool        QCATs::ExitScript = false;
QStringList QCATs::ScriptArguments;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QCATs::Register(QScriptEngine& engine)
{
    QObject* p_obj = new QCATs;
    QScriptValue objectValue = engine.newQObject(p_obj);
    engine.globalObject().setProperty("CATs", objectValue);

    QScriptValue fun;
    fun = engine.newFunction(QCATs::printf);
    engine.globalObject().setProperty("printf", fun);

    fun = engine.newFunction(QCATs::sprintf);
    engine.globalObject().setProperty("sprintf", fun);

    fun = engine.newFunction(QCATs::exit);
    engine.globalObject().setProperty("exit", fun);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QCATs::QCATs(void)
    : QCATsScriptable("cats")
{

}

//------------------------------------------------------------------------------

void QCATs::SetScriptArgument(const QString& arg)
{
    ScriptArguments << arg;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QCATs::system(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int system(cmd)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("cmd",1);
    if( value.isError() ) return(value);

    QString cmd;
    value = GetArgAsString("cmd","cmd",1,cmd);
    if( value.isError() ) return(value);


// execute ---------------------------------------
    return( ::system(cmd.toLatin1().constData()) );
}

//------------------------------------------------------------------------------

QScriptValue QCATs::printErrors(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: printErrors()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    ErrorSystem.PrintErrors(stdout);
    fprintf(stdout,"\n");

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QCATs::clearErrors(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: clearErrors()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    ErrorSystem.RemoveAllErrors();

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QCATs::numOfArguments(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int numOfArguments()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(ScriptArguments.count());
}

//------------------------------------------------------------------------------

QScriptValue QCATs::argument(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string argument(index)" << endl;
        sout << "note:  index is counted from one" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    int index = 0;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( (index < 1) || (index > ScriptArguments.count() ) ){
        value = ThrowError("index","index is out of legal range");
        return(value);
    }

    return(ScriptArguments.at(index-1));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QCATs::sprintf(QScriptContext* p_context, QScriptEngine* p_engine)
{
    return( sprintf("sprintf",p_context,p_engine).c_str() );
}

//------------------------------------------------------------------------------

QScriptValue QCATs::printf(QScriptContext* p_context, QScriptEngine* p_engine)
{
    std:: string str = sprintf("printf",p_context,p_engine);
    cout << str;
    return(QScriptValue());
}

//------------------------------------------------------------------------------

QScriptValue QCATs::exit(QScriptContext* p_context, QScriptEngine* p_engine)
{
    if( p_context->argumentCount() != 1 ) {
        p_context->throwError("exit(retcode) - illegal number of arguments, only one is expected");
        return(QScriptValue());
    }

    ExitScript = true;

    if( p_context->argument(0).isNumber() ){
        ExitValue = p_context->argument(0).toNumber();
    }

    p_engine->abortEvaluation( p_context->argument(0) );
    return(QScriptValue());
}

//------------------------------------------------------------------------------

const std::string QCATs::sprintf(const CSmallString& fname, QScriptContext* p_context,
                                QScriptEngine* p_engine)
{
    if( p_context->argumentCount() < 1 ) {
        CSmallString error;
        error << fname << "(format[,value1,value2,..]) - illegal number of arguments, at least one is expected";
        p_context->throwError(QString(error));
        return("");
    }

    QString format;
    format = p_context->argument(0).toString();

    // prepare format
    boost::format my_format;
    try {
        my_format.parse(format.toStdString());
    } catch(...) {
        CSmallString error;
        error << fname << "(format[,value1,value2,..]) - unable to parse format";
        p_context->throwError(QString(error));
        return("");
    }

    if( my_format.expected_args() != (p_context->argumentCount() - 1) ){
        CSmallString error;
        error << fname << "(format[,value1,value2,..]) - format requires different number of values (";
        error << my_format.expected_args() << ") than it is provided (" << (p_context->argumentCount() - 1) << ")";
        p_context->throwError(QString(error));
        return("");
    }

    // parse individual arguments
    for(int i=0; i < my_format.expected_args(); i++ ){
        QScriptValue val = p_context->argument(i+1);

        try {
            if( val.isNumber() ){
                double sval = val.toNumber();
                my_format % sval;
            } else {
                QString sval = val.toString();
                my_format % sval.toStdString();
            }
        } catch(...){
            CSmallString error;
            error << fname << "(format[,value1,value2,..]) - unable to process argument (";
            error << i + 1 << ")";
            p_context->throwError(QString(error));
            return("");
        }
    }

    // return string
    return( my_format.str() );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


