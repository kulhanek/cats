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
#include <QOFile.moc>
#include <SmallString.hpp>
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
        context->throwError("new OFile([name]) - it can be called only as a constructor");
        return(engine->undefinedValue());
    }
    if( context->argumentCount() > 1 ) {
        context->throwError("new OFile([name]) - illegal number of arguments, none or one is expected");
        return(engine->undefinedValue());
    }

    QString name;
    if( context->argumentCount() == 1 ){
        name = context->argument(0).toString();
    }

    QOFile* p_file = new QOFile();

    if( ! name.isEmpty() ){
        if( p_file->openInternal(name) == false ){
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
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool QOFile::open(const QString& name)
{
    if( argumentCount() != 1 ) {
        context()->throwError("OFile::open(name) - illegal number of arguments, one is expected");
        return(false);
    }

    vout.close();
    return( openInternal(name) );
}

//------------------------------------------------------------------------------

bool QOFile::openInternal(const QString& name)
{
    vout.open(name.toStdString().c_str());
    return( vout.good() );
}

//------------------------------------------------------------------------------

void QOFile::close(void)
{
    if( argumentCount() != 0 ) {
        context()->throwError("OFile::close() - illegal number of arguments, none is expected");
        return;
    }
    vout.close();
}

//------------------------------------------------------------------------------

bool QOFile::printf(const QString& format)
{
    if( argumentCount() < 1 ) {
        context()->throwError("OFile::printf(format[,value1,value2,..]) - illegal number of arguments, at least one is expected");
        return(false);
    }

    // prepare format
    boost::format my_format;
    try {
        my_format.parse(format.toStdString());
    } catch(...) {
        context()->throwError("OFile::printf(format[,value1,value2,..]) - unable to parse format");
        return(false);
    }

    if( my_format.expected_args() != (argumentCount() - 1) ){
        CSmallString error;
        error << "OFile::printf(format[,value1,value2,..]) - format requires different number of values (";
        error << my_format.expected_args() << ") than it is provided (" << (argumentCount() - 1) << ")";
        context()->throwError(QString(error));
        return(false);
    }

    // parse individual arguments
    for(int i=0; i < my_format.expected_args(); i++ ){
        QScriptValue val = argument(i+1);

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
            error << "OFile::printf(format[,value1,value2,..]) - unable to process argument (";
            error << i + 1 << ")";
            context()->throwError(QString(error));
            return(false);
        }
    }

    // write string to file
    vout << my_format;

    // was it OK?
    return( vout.good() );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


