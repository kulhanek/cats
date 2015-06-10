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
#include <QOBMol.hpp>
#include <QOBMol.moc>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QOBMol::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QOBMol::New);
    QScriptValue metaObject = engine.newQMetaObject(&QOBMol::staticMetaObject, ctor);
    engine.globalObject().setProperty("OBMol", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QOBMol::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    if( context->isCalledAsConstructor() == false ) {
        context->throwError("OBMol() can be called only as a constructor\nusage: new OBMol(topology)\n       new OBMol(topology,name)");
        return(engine->undefinedValue());
    }
    if( (context->argumentCount() > 2) || (context->argumentCount() < 1) ) {
        context->throwError("Illegal number of arguments\nusage: new OBMol(topology)\n       new OBMol(topology,name)");
        return(engine->undefinedValue());
    }
    QOBMol* p_obj = new QOBMol();

    if( context->argumentCount() == 2 ) {
        // TODO: load
    }
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QOBMol::QOBMol(void)
    : QCATsScriptable("OBMol")
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


