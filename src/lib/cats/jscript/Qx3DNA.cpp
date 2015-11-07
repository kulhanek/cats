// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Qx3DNA.hpp>
#include <Qx3DNA.moc>
#include <TerminalStr.hpp>
#include <QTopology.hpp>
#include <QSnapshot.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void Qx3DNA::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(Qx3DNA::New);
    QScriptValue metaObject = engine.newQMetaObject(&Qx3DNA::staticMetaObject, ctor);
    engine.globalObject().setProperty("x3DNA", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    QCATsScriptable scriptable("x3DNA");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "x3DNA object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new x3DNA()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    Qx3DNA* p_obj = new Qx3DNA();
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

Qx3DNA::Qx3DNA(void)
    : QCATsScriptable("x3DNA")
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue Qx3DNA::analyze(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNA::analyze(topology,snapshot)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("topology,snapshot",2);
    if( value.isError() ) return(value);

    QTopology* p_qtop = NULL;
    value = GetArgAsObject<QTopology*>("topology,snapshot","topology","Topology",1,p_qtop);
    if( value.isError() ) return(value);


    QSnapshot* p_qsnap = NULL;
    value = GetArgAsObject<QSnapshot*>("topology,snapshot","snapshot","Snapshot",2,p_qsnap);
    if( value.isError() ) return(value);



// create PDB file -------------------------------


    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


