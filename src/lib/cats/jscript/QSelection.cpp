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
#include <QSelection.hpp>
#include <moc_QSelection.cpp>
#include <QTopology.hpp>
#include <QSnapshot.hpp>
#include <QAtom.hpp>
#include <iomanip>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QSelection::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QSelection::New);
    QScriptValue metaObject = engine.newQMetaObject(&QSelection::staticMetaObject, ctor);
    engine.globalObject().setProperty("Selection", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QSelection::New(QScriptContext *context,QScriptEngine *engine)
{
    QCATsScriptable scriptable("Selection");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Selection object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new Selection(topology)" << endl;
        sout << "   new Selection(topology,mask)" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("topology[,mask]",1,2);
    if( value.isError() ) return(value);

    QTopology* p_qtop;
    value = scriptable.GetArgAsObject<QTopology*>("topology[,mask]","topology","Topology",1,p_qtop);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    QSelection* p_qsel = new QSelection(scriptable.GetArgument(1));

    if( context->argumentCount() == 2 ) {
        QString mask;
        value = scriptable.GetArgAsString("topology,mask","mask",2,mask);
        if( p_qsel->Mask.SetMask(mask) == false ) {
            delete p_qsel;
            return( scriptable.ThrowError("topology,mask","unable to setup mask"));
        }
    }

    return engine->newQObject(p_qsel, QScriptEngine::ScriptOwnership);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QSelection::QSelection(const QScriptValue& top)
    : QCATsScriptable("Selection")
{
    RegisterAsWeakObject(top);
    Mask.AssignTopology(&GetQTopology()->Topology);
}

//------------------------------------------------------------------------------

void QSelection::CleanData(void)
{
    Mask.Reset();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QSelection::getTopology(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: topology Selection::getTopology()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Topology);
}

//------------------------------------------------------------------------------

QScriptValue QSelection::setByMask(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Selection::setByMask(mask,[snapshot])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("mask[,snapshot]",1,2);
    if( value.isError() ) return(value);

    QString mask;
    value = GetArgAsString("mask[,snapshot]","mask",1,mask);
    if( value.isError() ) return(value);

    CAmberRestart* p_snap = NULL;
    if( GetArgumentCount() == 2 ){
        QSnapshot* p_qsnap;
        value = GetArgAsObject<QSnapshot*>("mask,snapshot","snapshot","Snapshot",2,p_qsnap);
        if( value.isError() ) return(value);
        p_snap = &p_qsnap->Restart;
    }

    Mask.AssignCoordinates(p_snap);
    value = Mask.SetMask(mask.toLatin1().constData());
    Mask.AssignCoordinates(NULL);

    return(value);
}

// -----------------------------------------------------------------------------

QScriptValue QSelection::getMask(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: topology QSelection::getTopology()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return( QString(Mask.GetMask()) );
}

// -----------------------------------------------------------------------------

QScriptValue QSelection::printInfo(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Selection::printInfo([snapshot])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("[snapshot]",0,1);
    if( value.isError() ) return(value);

    QSnapshot* p_qsnap = NULL;
    if( GetArgumentCount() == 1 ){
        value = GetArgAsObject<QSnapshot*>("snapshot","snapshot","Snapshot",1,p_qsnap);
        if( value.isError() ) return(value);

        if( Mask.GetTopology() != p_qsnap->Restart.GetTopology() ){
            return( ThrowError("snapshot","snapshot and selection do not have the same topology") );
        }
    }

// execute ---------------------------------------
    cout << "=== Selection" << endl;
    cout << "# Atom mask               : " << Mask.GetMask() << endl;
    cout << "# Number of selected atoms: " << Mask.GetNumberOfSelectedAtoms() << endl;
    cout << "# Total number of atoms   : " << Mask.GetNumberOfTopologyAtoms() << endl;

    cout << "#" << endl;
    cout << "#   ID   Name ResID  RName  Mass        X [A]           Y [A]           Z [A]     Type" << endl;
    cout << "# ------ ---- ------ ----- ------ --------------- --------------- --------------- ----" << endl;
    if( p_qsnap ) {
        CAmberTopology* p_top = p_qsnap->Restart.GetTopology();
        for(int i=0; i < Mask.GetNumberOfSelectedAtoms(); i++) {
            int index = Mask.GetSelectedAtomCondensed(i)->GetAtomIndex();
            CAmberAtom* p_atom = p_top->AtomList.GetAtom(index);
            // topology and snapshot have the same number of atoms
            double mass = p_atom->GetMass();
            CPoint p1 = p_qsnap->Restart.GetPosition(index);
            cout << "  " << setw(6) << right << index+1;
            cout << " " << setw(4) << left << p_atom->GetName();
            CAmberResidue* p_res = p_atom->GetResidue();
            if( p_res ){
            cout << " " << setw(6) << right << p_res->GetIndex()+1;
            cout << " " << setw(5) << left << p_res->GetName();
            } else {
            cout << "             ";
            }
            cout << " " << setw(6) << setprecision(3) << right << fixed << mass;
            cout << " " << setw(15) << setprecision(4) << right << fixed << p1.x;
            cout << " " << setw(15) << p1.y << " " << setw(15) << p1.z;
            cout << " " << setw(5) << left << p_atom->GetType() << endl;
        }
    } else {
        for(int i=0; i < Mask.GetNumberOfSelectedAtoms(); i++) {
            CAmberAtom* p_atom = Mask.GetSelectedAtomCondensed(i);
            double mass = p_atom->GetMass();
            cout << "  " << setw(6) << right << i;
            cout << " " << setw(4) << left << p_atom->GetName();
            CAmberResidue* p_res = p_atom->GetResidue();
            if( p_res ){
            cout << " " << setw(6) << right << p_res->GetIndex()+1;
            cout << " " << setw(5) << left << p_res->GetName();
            } else {
            cout << "             ";
            }
            cout << " " << setw(6) << setprecision(3) << right << fixed << mass;
            cout << "                                                ";
            cout << " " << setw(5) << left << p_atom->GetType() << endl;
        }
    }

    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QSelection::getNumOfSelectedAtoms(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Selection::getNumOfSelectedAtoms()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Mask.GetNumberOfSelectedAtoms());
}

//------------------------------------------------------------------------------

QScriptValue QSelection::getAtom(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: atom Selection::getAtom(index)" << endl;
        sout << "       atom Selection::getAtom(index,atom)" << endl;
        sout << "note:  index is counted from zero" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index[,atom]",1,2);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);
    if( (index < 0) || (index >= Mask.GetNumberOfSelectedAtoms()) ){
        return( ThrowError("index","index out-of-legal range") );
    }

    if( GetArgumentCount() == 1 ){
        QAtom* p_obj = new QAtom(Topology);
        p_obj->Atom = Mask.GetSelectedAtomCondensed(index);
        value = engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership);
        return( value );
    }

    QAtom* p_qatom;
    value = GetArgAsObject<QAtom*>("index,atom","atom","Atom",2,p_qatom);
    if( value.isError() ) return(value);

    p_qatom->RegisterAsChildObject(thisObject());
    p_qatom->Atom = Mask.GetSelectedAtomCondensed(index);

    return(GetArgument(2));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
