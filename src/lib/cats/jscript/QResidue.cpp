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
#include <QResidue.hpp>
#include <moc_QResidue.cpp>
#include <AmberResidue.hpp>
#include <TerminalStr.hpp>
#include <QAtom.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QResidue::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QResidue::New);
    QScriptValue metaObject = engine.newQMetaObject(&QResidue::staticMetaObject, ctor);
    engine.globalObject().setProperty("Residue", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QResidue::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    QCATsScriptable scriptable("Residue");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Residue object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new Residue()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    QResidue* p_obj = new QResidue(QScriptValue());
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QResidue::QResidue(const QScriptValue& top)
    : QCATsScriptable("Residue")
{
    Residue = NULL;
    RegisterAsChildObject(top);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QResidue::getTopology(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: topology Residue::getTopology()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(JSTopology);
}

//------------------------------------------------------------------------------

QScriptValue QResidue::getIndex(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Residue::getIndex()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Residue == NULL ){
        return( ThrowError("","no residue is associated with the object") );
    }

// execute ---------------------------------------
    return(Residue->GetIndex());
}

//------------------------------------------------------------------------------

QScriptValue QResidue::getName(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string Residue::getName()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Residue == NULL ){
        return( ThrowError("","no residue is associated with the object") );
    }

// execute ---------------------------------------
    return(Residue->GetName());
}

//------------------------------------------------------------------------------

QScriptValue QResidue::getNumOfAtoms(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Residue::getNumOfAtoms()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Residue == NULL ){
        return( ThrowError("","no residue is associated with the object") );
    }

// execute ---------------------------------------
    return(Residue->GetNumberOfAtoms());
}

//------------------------------------------------------------------------------

QScriptValue QResidue::getAtom(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Residue::getAtom(index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    if( Residue == NULL ){
        return( ThrowError("","no residue is associated with the object") );
    }

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);
    if( (index < 0) || (index >= Residue->GetNumberOfAtoms()) ){
        return( ThrowError("index","index out-of-legal range") );
    }

// execute ---------------------------------------

    QAtom* p_obj = new QAtom(JSTopology);
    p_obj->Atom = Residue->GetTopology()->AtomList.GetAtom(index);
    value = engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership);

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QResidue::getFirstAtom(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Residue::getFirstAtomIndex()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Residue == NULL ){
        return( ThrowError("","no residue is associated with the object") );
    }

    if( GetQTopology() == NULL ){
        return( ThrowError("","no topology is associated with the object") );
    }

// execute ---------------------------------------
    QAtom* p_obj = new QAtom(JSTopology);
    p_obj->Atom = GetQTopology()->Topology.AtomList.GetAtom(Residue->GetFirstAtomIndex());
    return(engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//------------------------------------------------------------------------------

QScriptValue QResidue::getFirstAtomIndex(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Residue::getFirstAtomIndex()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Residue == NULL ){
        return( ThrowError("","no residue is associated with the object") );
    }

// execute ---------------------------------------
    return(Residue->GetFirstAtomIndex());
}

//------------------------------------------------------------------------------

QScriptValue QResidue::getNumOfNeighbourResidues(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Residue::getNumOfNeighbourResidues()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Residue == NULL ){
        return( ThrowError("","no residue is associated with the object") );
    }

// execute ---------------------------------------
    return(false);  // TODO
}

//------------------------------------------------------------------------------

QScriptValue QResidue::getNeighbourResidue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Residue::getAtom(index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    if( Residue == NULL ){
        return( ThrowError("","no residue is associated with the object") );
    }

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);
    if( (index < 0) || (index >= Residue->GetNumberOfAtoms()) ){
        return( ThrowError("index","index out-of-legal range") );
    }

// execute ---------------------------------------

    // FIXME
    QAtom* p_obj = new QAtom(JSTopology);
    p_obj->Atom = Residue->GetTopology()->AtomList.GetAtom(index);
    value = engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership);

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QResidue::getNeighbourResidueIndex(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Residue::getNeighbourResidueIndex()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    if( Residue == NULL ){
        return( ThrowError("","no residue is associated with the object") );
    }

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);
//    if( (index < 0) || (index >= Residue->GetNumberOfNeighbourResidues()) ){
//        return( ThrowError("index","index out-of-legal range") );
//    }

// execute ---------------------------------------
    return(false); // TODO
}

//------------------------------------------------------------------------------

QScriptValue QResidue::getNextResidue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: residue Residue::getNextResidue()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Residue == NULL ){
        return( ThrowError("","no residue is associated with the object") );
    }

    if( GetQTopology() == NULL ){
        return( ThrowError("","no topology is associated with the object") );
    }

    int index = Residue->GetIndex();
    index++;
    if( index >= GetQTopology()->Topology.ResidueList.GetNumberOfResidues() ){
        // no more residues
        return( false );
    }

// execute ---------------------------------------
    QResidue* p_obj = new QResidue(JSTopology);
    p_obj->Residue = GetQTopology()->Topology.ResidueList.GetResidue(index);
    return(engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//------------------------------------------------------------------------------

QScriptValue QResidue::isSameAs(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Residue::isSameAs(residue)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("residue",1);
    if( value.isError() ) return(value);

    QResidue* p_qres;
    value = GetArgAsObject<QResidue*>("residue","residue","Residue",1,p_qres);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return( p_qres->Residue == Residue );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


