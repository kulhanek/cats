// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QAtom.hpp>
#include <QResidue.hpp>
#include <AmberResidue.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

#include <QAtom.moc>
using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QAtom::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QAtom::New);
    QScriptValue metaObject = engine.newQMetaObject(&QAtom::staticMetaObject, ctor);
    engine.globalObject().setProperty("Atom", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QAtom::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    QCATsScriptable scriptable("Atom");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Atom object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new Atom()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    QAtom* p_obj = new QAtom(QScriptValue());
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QAtom::QAtom(const QScriptValue& top)
    : QCATsScriptable("Atom")
{
    Atom = NULL;
    RegisterAsChildObject(top);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QAtom::getTopology(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: topology Atom::getTopology()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Topology);
}

//------------------------------------------------------------------------------

QScriptValue QAtom::getIndex(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Atom::getIndex()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Atom == NULL ){
        return( ThrowError("","no atom is associated with the object") );
    }

// execute ---------------------------------------
    return(Atom->GetAtomIndex());
}

//------------------------------------------------------------------------------

QScriptValue QAtom::getName(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string Atom::getName()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Atom == NULL ){
        return( ThrowError("","no atom is associated with the object") );
    }

// execute ---------------------------------------
    return(Atom->GetName());
}

//------------------------------------------------------------------------------

QScriptValue QAtom::getResidue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: residue Atom::getResidue()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Atom == NULL ){
        return( ThrowError("","no atom is associated with the object") );
    }

    if( Atom->GetResidue() == NULL ){
        return( ThrowError("","no residue assocciated with the atom") );
    }

// execute ---------------------------------------
    QResidue* p_obj = new QResidue(Topology);
    p_obj->Residue = Atom->GetResidue();
    return(engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//------------------------------------------------------------------------------

QScriptValue QAtom::getResIndex(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Atom::getResIndex()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Atom == NULL ){
        return( ThrowError("","no atom is associated with the object") );
    }

    if( Atom->GetResidue() == NULL ){
        return( ThrowError("","no residue assocciated with the atom") );
    }

// execute ---------------------------------------
    return(Atom->GetResidue()->GetIndex());
}

//------------------------------------------------------------------------------

QScriptValue QAtom::getResName(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string Atom::getResName()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Atom == NULL ){
        return( ThrowError("","no atom is associated with the object") );
    }

    if( Atom->GetResidue() == NULL ){
        return( ThrowError("","no residue assocciated with the atom") );
    }

// execute ---------------------------------------
    return(Atom->GetResidue()->GetName());
}

//------------------------------------------------------------------------------

QScriptValue QAtom::getNumOfNeighbourAtoms(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Atom::getNumOfNeighbourAtoms()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Atom == NULL ){
        return( ThrowError("","no atom is associated with the object") );
    }

// execute ---------------------------------------
    return(Atom->GetNumberOfNeighbourAtoms());
}

//------------------------------------------------------------------------------

QScriptValue QAtom::getNeighbourAtom(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Atom::getNeighbourAtom()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    if( Atom == NULL ){
        return( ThrowError("","no atom is associated with the object") );
    }

    if( GetQTopology() == NULL ){
        return( ThrowError("","no topology is associated with the object") );
    }

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);
    if( (index < 0) || (index >= Atom->GetNumberOfNeighbourAtoms()) ){
        return( ThrowError("index","index out-of-legal range") );
    }

// execute ---------------------------------------
    QAtom* p_obj = new QAtom(Topology);
    p_obj->Atom = GetQTopology()->Topology.AtomList.GetAtom(index);
    return(engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//------------------------------------------------------------------------------

QScriptValue QAtom::getNeighbourAtomIndex(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Atom::getNeighbourAtomIndex()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    if( Atom == NULL ){
        return( ThrowError("","no atom is associated with the object") );
    }

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);
    if( (index < 0) || (index >= Atom->GetNumberOfNeighbourAtoms()) ){
        return( ThrowError("index","index out-of-legal range") );
    }

// execute ---------------------------------------
    return(Atom->GetNeighbourAtomIndex(index));
}

//------------------------------------------------------------------------------

QScriptValue QAtom::isBondedWith(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Atom::isBondedWith(atom)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("atom",1);
    if( value.isError() ) return(value);

    QAtom* p_qobj;
    value = GetArgAsObject<QAtom*>("atom","atom","Atom",1,p_qobj);
    if( value.isError() ) return(value);

// execute ----------------------------------------
    // different topologies
    if( GetQTopology()  != p_qobj->GetQTopology() ) return(false);

    // check in the list of neighbour atoms
    for(int i=0; i < Atom->GetNumberOfNeighbourAtoms(); i++){
        if( Atom->GetNeighbourAtomIndex(i) == p_qobj->Atom->GetAtomIndex() ) return(true);
    }

    return(false);
}

//------------------------------------------------------------------------------

QScriptValue QAtom::getNextAtom(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: atom Atom::getNextAtom()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Atom == NULL ){
        return( ThrowError("","no atom is associated with the object") );
    }

    if( GetQTopology() == NULL ){
        return( ThrowError("","no topology is associated with the object") );
    }
    int index = Atom->GetAtomIndex();
    index++;
    if( index >= GetQTopology()->Topology.AtomList.GetNumberOfAtoms() ){
        // no more atoms
        return( false );
    }

// execute ---------------------------------------
    QAtom* p_obj = new QAtom(Topology);
    p_obj->Atom = GetQTopology()->Topology.AtomList.GetAtom(index);
    return(engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//------------------------------------------------------------------------------

QScriptValue QAtom::isSameAs(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Atom::isSameAs(atom)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("atom",1);
    if( value.isError() ) return(value);

    QAtom* p_qatom;
    value = GetArgAsObject<QAtom*>("atom","atom","Atom",1,p_qatom);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return( p_qatom->Atom == Atom );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


