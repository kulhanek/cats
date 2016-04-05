// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2016 Viktor Illík
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
#include <QHistogram2D.hpp>
#include <iomanip>
#include <fstream>
#include <SmallString.hpp>
#include <math.h>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

#include <QHistogram2D.moc>
using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QHistogram2D::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QHistogram2D::New);
    QScriptValue metaObject = engine.newQMetaObject(&QHistogram2D::staticMetaObject, ctor);
    engine.globalObject().setProperty("Histogram2D", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QHistogram2D::New(QScriptContext *context,QScriptEngine *engine)
{
    if( context->isCalledAsConstructor() == false ) {
        context->throwError("new Histogram2D([histogram]) - it can be called only as a constructor");
        return(engine->undefinedValue());
    }
    if( context->argumentCount() > 1 ) {
        context->throwError("new Histogram2D([histogram]) - illegal number of arguments, none or one is expected");
        return(engine->undefinedValue());
    }

    QCATsScriptable scriptable("Histogram2D");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Histogram2D object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new Histogram2D()" << endl;
        sout << "   new Histogram2D(histogram)" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("[histogram]",0,1);
    if( value.isError() ) return(value);

// create pbject
    QHistogram2D* p_src = NULL;
    if( scriptable.GetArgumentCount() > 0 ){
        value = scriptable.GetArgAsObject<>("histogram","histogram","Histogram2D",1,p_src);
        if( value.isError() ) return(value);
    }

    QHistogram2D* p_hist = new QHistogram2D();

    if( p_src != NULL ){
        // copy setup from source histogram
        // TODO
    }

    return(engine->newQObject(p_hist, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QHistogram2D::QHistogram2D(void)
    : QCATsScriptable("Histogram2D")
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================




