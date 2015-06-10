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
#include <QScriptEngine>
#include <QNetResults.hpp>
#include <QNetResults.moc>
#include <QTopology.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QNetResults::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QNetResults::New);
    QScriptValue metaObject = engine.newQMetaObject(&QNetResults::staticMetaObject, ctor);
    engine.globalObject().setProperty("NetResults", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QNetResults::New(QScriptContext *context,
                              QScriptEngine *engine)
{
    if( context->isCalledAsConstructor() == false ) {
        context->throwError("NetResults() can be called only as a constructor\nusage: new NetResults(topology)\n       new NetResults(topology,name)");
        return(engine->undefinedValue());
    }
    if( (context->argumentCount() > 2) || (context->argumentCount() < 1) ) {
        context->throwError("Illegal number of arguments\nusage: new NetResults(topology)\n       new NetResults(topology,name)");
        return(engine->undefinedValue());
    }
    QTopology* p_qtop =  dynamic_cast<QTopology*>(context->argument(0).toQObject());
    if( p_qtop == NULL ) {
        context->throwError("First parameter is not Topology");
        return(engine->undefinedValue());
    }
    QNetResults* p_obj = new QNetResults(p_qtop);

    if( context->argumentCount() == 2 ) {
        // TODO: load
    }
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QNetResults::QNetResults(QTopology* p_parent)
    : QCATsScriptable("NetResult")
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


