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
#include <QTopology.hpp>
#include <moc_QTopology.cpp>
#include <QAtom.hpp>
#include <QResidue.hpp>
#include <QSelection.hpp>
#include <TerminalStr.hpp>
#include <AmberSubTopology.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QTopologyObject::QTopologyObject(void)
{
    Topology = NULL;
}

//------------------------------------------------------------------------------

QTopologyObject::~QTopologyObject(void)
{
    if( Topology ){
        Topology->ChildObjects.removeAll(this);
        Topology->WeakObjects.removeAll(this);
    }

    Topology = NULL;
}

//------------------------------------------------------------------------------

void QTopologyObject::RegisterAsChildObject(const QScriptValue& top)
{
    if( Topology ){
        Topology->ChildObjects.removeAll(this);
    }

    JSTopology = top;
    Topology = dynamic_cast<QTopology*>(top.toQObject());
    if( Topology ){
        Topology->ChildObjects.append(this);
    }
}

//------------------------------------------------------------------------------

void QTopologyObject::RegisterAsWeakObject(const QScriptValue& top)
{
    if( Topology ){
        Topology->WeakObjects.removeAll(this);
    }

    JSTopology = top;
    Topology = dynamic_cast<QTopology*>(top.toQObject());
    if( Topology ){
        Topology->WeakObjects.append(this);
    }
}

//------------------------------------------------------------------------------

void QTopologyObject::UnlinkTopologyObject(void)
{
    Topology = NULL;
}

//------------------------------------------------------------------------------

void QTopologyObject::CleanData(void)
{
    // it does nothing here
}

//------------------------------------------------------------------------------

void QTopologyObject::UpdateData(void)
{
    // it does nothing here
}

//------------------------------------------------------------------------------

QTopology* QTopologyObject::GetQTopology(void)
{
   return(Topology);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QTopology::New(QScriptContext *context,
                            QScriptEngine *engine)
{
    QCATsScriptable scriptable("Topology");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Topology object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new Topology()" << endl;
        sout << "   new Topology(name)" << endl;
        sout << "   new Topology(selection)" << endl;
        sout << "   new Topology(topology)" << endl;
        sout << "   new Topology(topology,mask)" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0,2);
    if( value.isError() ) return(value);

// check arguments & execute ---------------------

    // new Topology()
    if( scriptable.GetArgumentCount() == 0 ){
        QTopology* p_top = new QTopology();
        value = engine->newQObject(p_top, QScriptEngine::ScriptOwnership);
        return(value);
    }

    if( scriptable.GetArgumentCount() == 1 ){
        // new Topology(name)
        if( scriptable.IsArgumentString(1) == true ){
            QString name;
            value = scriptable.GetArgAsString("name","name",1,name);
            if( value.isError() ) return(value);
            QTopology* p_top = new QTopology();
            value = engine->newQObject(p_top, QScriptEngine::ScriptOwnership);
            if( p_top->Topology.Load(name) == false ){
                CSmallString error;
                error << "unable to load topology file '" << name << "'";
                return( scriptable.ThrowError("name",error) );
            }
            p_top->Topology.InitMoleculeIndexes();
            return(value);
        }
        // new Topology(topology)       
        if( scriptable.IsArgumentObject<QTopology*>(1) == true ){
            QTopology* p_src;
            value = scriptable.GetArgAsObject<QTopology*>("topology","topology","Topology",1,p_src);
            if( value.isError() ) return(value);
            QTopology* p_top = new QTopology();
            value = engine->newQObject(p_top, QScriptEngine::ScriptOwnership);
            p_top->Topology = p_src->Topology;
            return(value);
        }
        // new Topology(selection)
        if( scriptable.IsArgumentObject<QSelection*>(1) == true ){
            QSelection* p_sel;
            value = scriptable.GetArgAsObject<QSelection*>("selection","selection","Selection",1,p_sel);
            if( value.isError() ) return(value);
            QTopology* p_top = new QTopology();
            value = engine->newQObject(p_top, QScriptEngine::ScriptOwnership);
            CAmberSubTopology sub_topology;
            if( sub_topology.InitSubTopology(&p_sel->Mask,true,true,false) == false ){
                return( scriptable.ThrowError("selection","unable to create subtopology from selection") );
            }
            p_top->Topology = sub_topology;
            return(value);
        }
        // error
        return( scriptable.ThrowError("name/topology/selection","the first argument is of unsupported type") );
    }

    // new Topology(topology,mask)
    if( scriptable.GetArgumentCount() == 2 ) {
        QTopology* p_src;
        value = scriptable.GetArgAsObject<QTopology*>("topology,mask","topology","Topology",1,p_src);
        if( value.isError() ) return(value);
        QString smask;
        value = scriptable.GetArgAsString("topology,mask","mask",2,smask);
        if( value.isError() ) return(value);

        if( p_src->Topology.AtomList.GetNumberOfAtoms() > 0 ){
            CAmberMaskAtoms mask;
            mask.AssignTopology(&p_src->Topology);
            if( mask.SetMask(smask) == false ){
                CSmallString error;
                error << "new Topology(topology,mask) - unable to init selection by '" << smask << "' mask";
                return(scriptable.ThrowError("topology,mask",error));
            }
            QTopology* p_top = new QTopology();
            value = engine->newQObject(p_top, QScriptEngine::ScriptOwnership);
            CAmberSubTopology sub_topology;
            cout << endl;
            sub_topology.InitSubTopology(&mask,true,true,true);
            p_top->Topology = sub_topology;
            return(value);
        } else {
            QTopology* p_top = new QTopology();
            value = engine->newQObject(p_top, QScriptEngine::ScriptOwnership);
            p_top->Topology = p_src->Topology;
            return(value);
        }
    }

    // this should not happen
    return(scriptable.GetUndefinedValue());
}

//------------------------------------------------------------------------------

void QTopology::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QTopology::New);
    QScriptValue metaObject = engine.newQMetaObject(&QTopology::staticMetaObject, ctor);
    engine.globalObject().setProperty("Topology", metaObject);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QTopology::QTopology(void)
    : QCATsScriptable("Topology")
{

}

//------------------------------------------------------------------------------

QTopology::~QTopology(void)
{
    DestroyChildObjects();
    DestroyWeakObjects();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue  QTopology::getNumOfAtoms(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Topology::getNumOfAtoms()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Topology.AtomList.GetNumberOfAtoms());
}

//------------------------------------------------------------------------------

QScriptValue  QTopology::getNumOfResidues(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Topology::getNumOfResidues()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Topology.ResidueList.GetNumberOfResidues());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QTopology::load(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Topology::load(name)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name",1);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name","name",1,name);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    // clear previous data
    DestroyChildObjects();
    CleanWeakObjects();

    // load data
    bool result = Topology.Load(name);
    Topology.InitMoleculeIndexes();

    UpdateWeakObjects();

    return(result);
}

//------------------------------------------------------------------------------

QScriptValue QTopology::save(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Topology::save(name)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name",1);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name","name",1,name);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( Topology.IsFake() ){
        // unable to save fake topology
        return( ThrowError("name","fake topology cannot be saved") );
    }

    // process data
    return(Topology.Save(name));
}

//------------------------------------------------------------------------------

QScriptValue QTopology::loadFakeTopology(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Topology::loadFakeTopology(name,type[,key1,key2])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckMinimumNumberOfArguments("name,type",2);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name,type","name",1,name);
    if( value.isError() ) return(value);

    QString type;
    value = GetArgAsString("name,type","type",2,type);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    // clear previous data
    DestroyChildObjects();
    CleanWeakObjects();

    // load new data
    bool result = false;
    if( type == "pdb" ){
        bool mangle = IsArgumentKeySelected("mangle");
        value = CheckArgumentsUsage("name,type[,key1,key2]");
        if( value.isError() ) return(value);
        result = Topology.LoadFakeTopologyFromPDB(name,mangle);
    } else if( type == "g96" ){
        result = Topology.LoadFakeTopologyFromG96(name);
    } else {
        CSmallString error;
        error << "'" << type << "' is not supported type";
        return( ThrowError("name,type",error));
    }

    // update related objects
    UpdateWeakObjects();

    return(result);
}

//------------------------------------------------------------------------------

QScriptValue QTopology::isFakeTopology(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Topology::isFakeTopology()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return( Topology.IsFake() );
}

//------------------------------------------------------------------------------

QScriptValue QTopology::copyFrom(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Topology::copyFrom(topology)" << endl;
        sout << "       bool Topology::copyFrom(selection[,key1,key2,..])" << endl;
        sout << "            Keys:        box          - copy box" << endl;
        sout << "                         verbose      - be verbose" << endl;
        sout << "                         noerrors     - do not stop on errors" << endl;
        sout << "                         noexceptions - do not throw exceptions on errors" << endl;
        return(false);
    }

// check arguments & execute ---------------------

    // bool Topology::copyFrom(topology)
    if( IsArgumentObject<QTopology*>(1) ){
        value = CheckNumberOfArguments("topology",1);
        if( value.isError() ) return(value);

        QTopology* p_top;
        value = GetArgAsObject<QTopology*>("topology","topology","Topology",1,p_top);
        if( value.isError() ) return(value);

        // clear previous data
        DestroyChildObjects();
        CleanWeakObjects();

        // deep copy
        Topology = p_top->Topology;

        // update related objects
        UpdateWeakObjects();

        return(true);
    }

    // bool Topology::copyFrom(selection[,key1,key2,..])
    if( IsArgumentObject<QSelection*>(1) ){

        QSelection* p_sel;
        value = GetArgAsObject<QSelection*>("selection","selection","Selection",1,p_sel);
        if( value.isError() ) return(value);

        bool copy_box = IsArgumentKeySelected("box");
        bool verbose = IsArgumentKeySelected("verbose");
        bool ignore_errors = IsArgumentKeySelected("noerrors");
        bool throw_on_errors = ! IsArgumentKeySelected("noexceptions");

        value = CheckArgumentsUsage("selection[,key1,key2,..]");
        if( value.isError() ) return(value);

        if( p_sel->Mask.GetNumberOfSelectedAtoms() > 0 ){
            CAmberSubTopology sub_topology;
            if( verbose ) cout << endl;
            if( sub_topology.InitSubTopology(&p_sel->Mask,copy_box,ignore_errors,verbose) == false ){
                if( throw_on_errors ){
                    return(ThrowError("selection[,key1,..]","exception due to errors"));
                }
                return(false);
            }
            // clear previous data
            DestroyChildObjects();
            CleanWeakObjects();

            Topology = sub_topology;

            // update related objects
            UpdateWeakObjects();
        }
        return(true);
    }

    return( ThrowError("topology/selection","first argument must be either topology or selection") );
}

//------------------------------------------------------------------------------

QScriptValue QTopology::clear(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Topology::clear()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    DestroyChildObjects();
    CleanWeakObjects();
    Topology.Clean();
    UpdateWeakObjects();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QTopology::printInfo(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Topology::printInfo()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Topology.PrintInfo();
    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QTopology::printAtoms(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Topology::printAtoms()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Topology.PrintAtoms();
    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QTopology::buidListOfNeighbourAtoms(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Topology::buidListOfNeighbourAtoms()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Topology.BuidListOfNeighbourAtoms();
    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QTopology::getAtom(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: atom Topology::getAtom(index)" << endl;
        sout << "       atom Topology::getAtom(index,atom)" << endl;
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
    if( (index < 0) || (index >= Topology.AtomList.GetNumberOfAtoms()) ){
        return( ThrowError("index","index out-of-legal range") );
    }

    if( GetArgumentCount() == 1 ){
        QAtom* p_obj = new QAtom(thisObject());
        p_obj->Atom = Topology.AtomList.GetAtom(index);
        value = engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership);
        return( value );
    }

    QAtom* p_qatom;
    value = GetArgAsObject<QAtom*>("index,atom","atom","Atom",2,p_qatom);
    if( value.isError() ) return(value);

    p_qatom->RegisterAsChildObject(thisObject());
    p_qatom->Atom = Topology.AtomList.GetAtom(index);

    return(GetArgument(2));
}

//------------------------------------------------------------------------------

QScriptValue QTopology::getFirstAtom(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: atom Topology::getFirstAtom()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);


    if( Topology.AtomList.GetNumberOfAtoms() == 0 ){
        // no more atoms
        return( false );
    }

// execute ---------------------------------------
    QAtom* p_obj = new QAtom(thisObject());
    p_obj->Atom = Topology.AtomList.GetAtom(0);
    return(engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//------------------------------------------------------------------------------

QScriptValue QTopology::getResidue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: residue Topology::getResidue(index)" << endl;
        sout << "       residue Topology::getResidue(index,residue)" << endl;
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
    if( (index < 0) || (index >= Topology.ResidueList.GetNumberOfResidues()) ){
        return( ThrowError("index","index out-of-legal range") );
    }

    if( GetArgumentCount() == 1 ){
        QResidue* p_obj = new QResidue(thisObject());
        p_obj->Residue = Topology.ResidueList.GetResidue(index);
        value = engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership);
        return( value );
    }

    QResidue* p_qobj;
    value = GetArgAsObject<QResidue*>("index,residue","residue","Residue",2,p_qobj);
    if( value.isError() ) return(value);

    p_qobj->RegisterAsChildObject(thisObject());
    p_qobj->Residue = Topology.ResidueList.GetResidue(index);

    return(GetArgument(2));
}

//------------------------------------------------------------------------------

QScriptValue QTopology::getFirstResidue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: residue Topology::getFirstResidue()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

    if( Topology.ResidueList.GetNumberOfResidues() == 0 ){
        // no residues
        return( false );
    }

// execute ---------------------------------------
    QResidue* p_obj = new QResidue(thisObject());
    p_obj->Residue = Topology.ResidueList.GetResidue(0);
    return(engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QTopology::DestroyWeakObjects(void)
{
    while( ! WeakObjects.isEmpty() ){
        QObject* p_obj = WeakObjects.takeFirst();
        QTopologyObject* p_topobj = dynamic_cast<QTopologyObject*>(p_obj);
        if( p_topobj != NULL ){
            p_topobj->UnlinkTopologyObject();
        }
        //p_obj->deleteLater();
    }
}

//------------------------------------------------------------------------------

void QTopology::DestroyChildObjects(void)
{
    while( ! ChildObjects.isEmpty() ){
        QObject* p_obj = ChildObjects.takeFirst();
        QTopologyObject* p_topobj = dynamic_cast<QTopologyObject*>(p_obj);
        if( p_topobj != NULL ){
            p_topobj->UnlinkTopologyObject();
        }
        //p_obj->deleteLater();
    }
}

//------------------------------------------------------------------------------

void QTopology::CleanWeakObjects(void)
{
    foreach(QObject* p_obj,WeakObjects){
        QTopologyObject* p_topobj = dynamic_cast<QTopologyObject*>(p_obj);
        if( p_topobj != NULL ){
            p_topobj->CleanData();
        }
    }
}

//------------------------------------------------------------------------------

void QTopology::UpdateWeakObjects(void)
{
    foreach(QObject* p_obj,WeakObjects){
        QTopologyObject* p_topobj = dynamic_cast<QTopologyObject*>(p_obj);
        if( p_topobj != NULL ){
            p_topobj->UpdateData();
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


