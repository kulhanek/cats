// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QScriptEngine>
#include <QOFile.hpp>
#include <moc_QOFile.cpp>
#include <SmallString.hpp>
#include <TerminalStr.hpp>
#include <iomanip>
#include <boost/format.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QOFile::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QOFile::New);
    QScriptValue metaObject = engine.newQMetaObject(&QOFile::staticMetaObject, ctor);
    engine.globalObject().setProperty("OFile", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QOFile::New(QScriptContext *context,QScriptEngine *engine)
{
    if( context->isCalledAsConstructor() == false ) {
        context->throwError("new OFile([name[,key1,key2,...]]) - it can be called only as a constructor");
        return(engine->undefinedValue());
    }
    if( context->argumentCount() == 0 ) {
        context->throwError("new OFile([name[,key1,key2,...]]) - illegal number of arguments, none or one is expected");
        return(engine->undefinedValue());
    }

    QString name;
    if( context->argumentCount() > 0 ){
        name = context->argument(0).toString();
    }

    std::ios_base::openmode mode = std::ios_base::out;
    for(int i=1; i < context->argumentCount(); i++){
        QScriptValue value = context->argument(i);
        if( value.isString() ){
            if( value.toString() == "append" ){
                mode = std::ios_base::out | std::ios_base::app;
                break;
            }
        }
    }

    QOFile* p_file = new QOFile();
    p_file->name = name;
    p_file->mode = mode;

    if( ! name.isEmpty() ){
        if( p_file->openInternal() == false ){
            delete p_file;
            context->throwError("new OFile(name) - unable to open file");
            return(engine->undefinedValue());
        }
    }

    return(engine->newQObject(p_file, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QOFile::QOFile(void)
    : QCATsScriptable("OFile")
{
    mode = std::ios_base::out;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QOFile::open(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: OFile::open(name[,key1,key2,...])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckMinimumNumberOfArguments("name",1);
    if( value.isError() ) return(value);

    value = GetArgAsString("name[,key1,key2,...]","name",1,name);
    if( value.isError() ) return(value);

    mode = std::ios_base::out;
    if( IsArgumentKeySelected("append") == true ){
        mode = std::ios_base::out | std::ios_base::app;
    }

// execute ---------------------------------------
    vout.close();
    if( openInternal() == false ){
        CSmallString error;
        error << "unable to open specified file '" << name << "'";
        return(ThrowError("name",error));
    }
    return(value);
}

//------------------------------------------------------------------------------

bool QOFile::openInternal(void)
{
    vout.open(name.toStdString().c_str(),mode);
    return( vout.is_open() );
}

//------------------------------------------------------------------------------

QScriptValue QOFile::close(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: OFile::close()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    vout.close();
    name.clear();
    mode = std::ios_base::out;
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QOFile::printf(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: OFile::printf(format,...)" << endl;
        return(false);
    }

    value = CheckMinimumNumberOfArguments("format,...",1);
    if( value.isError() ) return(value);

    QString format;
    value = GetArgAsString("format,...","format",1,format);
    if( value.isError() ) return(value);

// execute ---------------------------------------

    // prepare format
    boost::format my_format;
    try {
        my_format.parse(format.toStdString());
    } catch(...) {
        return(ThrowError("format,...","unable to parse format"));
    }

    if( my_format.expected_args() != (GetArgumentCount() - 1) ){
        CSmallString error;
        error << "format requires different number of values (";
        error << my_format.expected_args() << ") than it is provided (" << (GetArgumentCount() - 1) << ")";
        return(ThrowError("format,...",error));
    }

    // parse individual arguments
    for(int i=1; i <= my_format.expected_args(); i++ ){
        QScriptValue val = GetArgument(i+1);

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
            error << "unable to process argument (";
            error << i + 1 << ")";
            return(ThrowError("format,...",error));
        }
    }

    // write string to file
    vout << my_format;

    if( ! vout ){
        return(ThrowError("format,...","unable to write formated data to the file"));
    }

    return( value );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


