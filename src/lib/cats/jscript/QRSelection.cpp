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
#include <QRSelection.hpp>
#include <QTopology.hpp>
#include <QSnapshot.hpp>
#include <QResidue.hpp>
#include <iomanip>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

#include <QRSelection.moc>
using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QRSelection::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QRSelection::New);
    QScriptValue metaObject = engine.newQMetaObject(&QRSelection::staticMetaObject, ctor);
    engine.globalObject().setProperty("RSelection", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QRSelection::New(QScriptContext *context,QScriptEngine *engine)
{
    QCATsScriptable scriptable("RSelection");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "RSelection object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new RSelection(topology)" << endl;
        sout << "   new RSelection(topology,mask)" << endl;
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
    QRSelection* p_qsel = new QRSelection(scriptable.GetArgument(1));

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

QRSelection::QRSelection(const QScriptValue& top)
    : QCATsScriptable("RSelection")
{
    RegisterAsWeakObject(top);
    QTopology* p_qtop = dynamic_cast<QTopology*>(Topology.toQObject());
    Mask.AssignTopology(&p_qtop->Topology);
}

//------------------------------------------------------------------------------

void QRSelection::CleanData(void)
{
    Mask.Reset();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QRSelection::getTopology(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: topology RSelection::getTopology()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Topology);
}

//------------------------------------------------------------------------------

QScriptValue QRSelection::setByMask(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: RSelection::setByMask(mask,[snapshot])" << endl;
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

QScriptValue QRSelection::getMask(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: topology QRSelection::getTopology()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return( QString(Mask.GetMask()) );
}

// -----------------------------------------------------------------------------

QScriptValue QRSelection::printInfo(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: RSelection::printInfo()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("[",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    cout << "=== RSelection" << endl;
    cout << "# Residue mask                : " << Mask.GetMask() << endl;
    cout << "# Number of selected residues : " << Mask.GetNumberOfSelectedResidues() << endl;
    cout << "# Total number of residues    : " << Mask.GetNumberOfTopologyResidues() << endl;

    cout << "#" << endl;
    cout << "#   ID   Name " << endl;
    cout << "# ------ ---- " << endl;

    for(int i=0; i < Mask.GetNumberOfSelectedResidues(); i++) {
        CAmberResidue* p_res = Mask.GetSelectedResidueCondensed(i);
        cout << "  " << setw(6) << right << i;
        cout << " " << setw(4) << left << p_res->GetName();
        cout << endl;
    }

    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QRSelection::getNumOfSelectedResidues(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int RSelection::getNumOfSelectedResidues()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Mask.GetNumberOfSelectedResidues());
}

//------------------------------------------------------------------------------

QScriptValue QRSelection::getResidue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: residue RSelection::getResidue(index)" << endl;
        sout << "       residue RSelection::getResidue(index,atom)" << endl;
        sout << "note:  index is counted from zero" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index[,residue]",1,2);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);
    if( (index < 0) || (index >= Mask.GetNumberOfSelectedResidues()) ){
        return( ThrowError("index","index out-of-legal range") );
    }

    if( GetArgumentCount() == 1 ){
        QResidue* p_obj = new QResidue(Topology);
        p_obj->Residue = Mask.GetSelectedResidueCondensed(index);
        value = engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership);
        return( value );
    }

    QResidue* p_qres;
    value = GetArgAsObject<QResidue*>("index,residue","residue","Residue",2,p_qres);
    if( value.isError() ) return(value);

    p_qres->RegisterAsChildObject(thisObject());
    p_qres->Residue = Mask.GetSelectedResidueCondensed(index);

    return(GetArgument(2));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
