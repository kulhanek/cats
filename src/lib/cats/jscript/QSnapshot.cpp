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
#include <QSnapshot.hpp>
#include <QSnapshot.moc>
#include <QTopology.hpp>
#include <QSelection.hpp>
#include <QPoint.hpp>
#include <QAverageSnapshot.hpp>
#include <Transformation.hpp>
#include <TerminalStr.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QSnapshot::New(QScriptContext *context,
                            QScriptEngine *engine)
{
    QCATsScriptable scriptable("Snapshot");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Snapshot object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new Snapshot(topology)" << endl;
        sout << "   new Snapshot(topology,name)" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("topology[,name]",1,2);
    if( value.isError() ) return(value);

// check arguments & execute ---------------------

    // new Snapshot(topology)
    if( scriptable.GetArgumentCount() == 1 ){
        QTopology* p_top;
        value = scriptable.GetArgAsObject<QTopology*>("topology","topology","Topology",1,p_top);
        if( value.isError() ) return(value);
        QSnapshot* p_snap = new QSnapshot(scriptable.GetArgument(1));
        value = engine->newQObject(p_snap, QScriptEngine::ScriptOwnership);
        return(value);
    }

    // new Snapshot(topology,name)
    QTopology* p_top;
    value = scriptable.GetArgAsObject<QTopology*>("topology,name","topology","Topology",1,p_top);
    if( value.isError() ) return(value);
    QString name;
    value = scriptable.GetArgAsString("topology,name","name",2,name);
    if( value.isError() ) return(value);

    QSnapshot* p_snap = new QSnapshot(scriptable.GetArgument(1));
    if( p_snap->Restart.Load(name) == false ) {
        delete p_snap;
        return( scriptable.ThrowError("topology,name","unable to load restart file") );
    }
    value = engine->newQObject(p_snap, QScriptEngine::ScriptOwnership);
    return(value);
}

//------------------------------------------------------------------------------

void QSnapshot::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QSnapshot::New);
    QScriptValue metaObject = engine.newQMetaObject(&QSnapshot::staticMetaObject, ctor);
    engine.globalObject().setProperty("Snapshot", metaObject);
}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QSnapshot::QSnapshot(const QScriptValue& top)
    : QCATsScriptable("Snapshot")
{
    RegisterAsWeakObject(top);
    Restart.AssignTopology(&GetQTopology()->Topology);
    Restart.Create();
}

//------------------------------------------------------------------------------

void QSnapshot::CleanData(void)
{
    Restart.Release();
}

//------------------------------------------------------------------------------

void QSnapshot::UpdateData(void)
{
    Restart.Create();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int QSnapshot::GetNumOfAtoms(void)
{
    return(Restart.GetNumberOfAtoms());
}

//------------------------------------------------------------------------------

const CPoint QSnapshot::GetCOMPoint(bool nomass)
{
    CPoint com;
    CAmberTopology* p_top = Restart.GetTopology();
    double totmass = 0.0;
    for(int i=0; i < GetNumOfAtoms(); i++) {
        CAmberAtom* p_atom = p_top->AtomList.GetAtom(i);
        // topology and snapshot have the same number of atoms
        double mass;
        if( nomass ){
            mass = 1.0;
        } else {
            mass = p_atom->GetMass();
        }
        CPoint p1 = Restart.GetPosition(i);
        com += p1*mass;
        totmass += mass;
    }
    if( totmass > 0.0 ) com /= totmass;
    return(com);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QSnapshot::getTopology(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: topology Snapshot::getTopology()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Topology);
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::load(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Snapshot::load(name)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name",1);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name","name",1,name);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    bool result = Restart.Load(name);
    return(result);
}

// -----------------------------------------------------------------------------

QScriptValue QSnapshot::save(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Snapshot::save(name)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name",1);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name","name",1,name);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    bool result = Restart.Save(name.toLatin1().constData());
    return(result);
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::copyFrom(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Snapshot::copyFrom(snapshot[,selection])" << endl;
        sout << "       Snapshot::copyFrom(averageSnapshot[,selection])" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("snapshot/averageSnapshot[,selection]",1,2);
    if( value.isError() ) return(value);

    if( IsArgumentObject<QSnapshot*>(1) ){
        QSnapshot* p_snap;
        GetArgAsObject("snapshot/averageSnapshot[,selection]","snapshot","Snapshot",1,p_snap);

        if( GetArgumentCount() == 1 ){
            if( GetNumOfAtoms() != p_snap->GetNumOfAtoms() ){
                CSmallString error;
                error << "inconsistent number of atoms, source (" << p_snap->GetNumOfAtoms() << "), target (" << GetNumOfAtoms() << ")";
                return( ThrowError("snapshot/averageSnapshot[,selection]",error) );
            }
            Restart = p_snap->Restart;
            return(value);
        } else {

            QSelection* p_sel;
            GetArgAsObject("snapshot/averageSnapshot,selection","selection","Selection",2,p_sel);

            if( GetNumOfAtoms() != p_sel->Mask.GetNumberOfSelectedAtoms() ){
                CSmallString error;
                error << "inconsistent number of atoms, selection (" << p_sel->Mask.GetNumberOfSelectedAtoms() << "), target (" << GetNumOfAtoms() << ")";
                return(ThrowError("snapshot/averageSnapshot,selection",error));
            }

            CAmberRestart* p_src = &p_snap->Restart;
            for(int i=0; i < GetNumOfAtoms(); i++ ){
                int j = p_sel->Mask.GetSelectedAtomCondensed(i)->GetAtomIndex();
                CPoint pos = p_src->GetPosition(j);
                Restart.SetPosition(i,pos);
                if( p_src->AreVelocitiesLoaded() ){
                    CPoint vel = p_src->GetVelocity(j);
                    Restart.SetVelocity(i,vel);
                }
            }
            if( p_src->IsBoxPresent() ){
                Restart.SetBox(p_src->GetBox());
                Restart.SetAngles(p_src->GetAngles());
            }
            return(value);
        }
    }

    if( IsArgumentObject<QAverageSnapshot*>(1) ){

        QAverageSnapshot* p_asnap;
        GetArgAsObject("snapshot/averageSnapshot[,selection]","averageSnapshot","AverageSnapshot",1,p_asnap);

        if( GetArgumentCount() == 1 ){
            if( GetNumOfAtoms() != p_asnap->Restart.GetNumberOfAtoms() ){
                CSmallString error;
                error << "inconsistent number of atoms, source (" << p_asnap->Restart.GetNumberOfAtoms() << "), target (" << GetNumOfAtoms() << ")";
                return( ThrowError("snapshot/averageSnapshot[,selection]",error) );
            }
            if( p_asnap->SamplingMode == true ){
                return( ThrowError("snapshot/averageSnapshot[,selection]","averageSnapshot is still in sampling mode") );
            }
            Restart = p_asnap->Restart;
            return(value);
        } else {
            QSelection* p_sel;
            GetArgAsObject("snapshot/averageSnapshot,selection","selection","Selection",2,p_sel);

            if( GetNumOfAtoms() != p_sel->Mask.GetNumberOfSelectedAtoms() ){
                CSmallString error;
                error << "inconsistent number of atoms, selection (" << p_sel->Mask.GetNumberOfSelectedAtoms() << "), target (" << GetNumOfAtoms() << ")";
                return(ThrowError("snapshot/averageSnapshot,selection",error));
            }

            CAmberRestart* p_src = &p_asnap->Restart;
            for(int i=0; i < GetNumOfAtoms(); i++ ){
                int j = p_sel->Mask.GetSelectedAtomCondensed(i)->GetAtomIndex();
                CPoint pos = p_src->GetPosition(j);
                Restart.SetPosition(i,pos);
                if( p_src->AreVelocitiesLoaded() ){
                    CPoint vel = p_src->GetVelocity(j);
                    Restart.SetVelocity(i,vel);
                }
            }
            if( p_src->IsBoxPresent() ){
                Restart.SetBox(p_src->GetBox());
                Restart.SetAngles(p_src->GetAngles());
            }
            return(value);
        }
    }

    return( ThrowError("snapshot/averageSnapshot[,selection]","first argument must be either snapshot or averageSnapshot") );
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::clear(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Snapshot::clear()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    CPoint zero;

    for(int i=0; i < Restart.GetNumberOfAtoms(); i++){
        Restart.SetPosition(i,zero);
        if( Restart.AreVelocitiesLoaded() ){
            Restart.SetVelocity(i,zero);
        }
    }
    if( Restart.IsBoxPresent() ){
        Restart.SetBox(zero);
        Restart.SetAngles(zero);
    }
    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QSnapshot::setTitle(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Snapshot::setTitle(title)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("title",1);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    QString title;
    value = GetArgAsString("title","title",1,title);
    if( value.isError() ) return(value);

    Restart.SetTitle(title);
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::getTitle(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Snapshot::getTitle()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    return( QString(Restart.GetTitle()) );
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::setTime(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Snapshot::setTime(time)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("time",1);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    double time;
    value = GetArgAsRNumber("time","time",1,time);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    Restart.SetTime(time);
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::getTime(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Snapshot::getTime()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    return(Restart.GetTime());
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::areVelocitiesAvailable(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Snapshot::areVelocitiesAvailable()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    return(Restart.AreVelocitiesLoaded());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QSnapshot::isBoxPresent(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Snapshot::isBoxPresent()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    return(Restart.IsBoxPresent());
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::getBoxASize(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Snapshot::getBoxASize()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    if( Restart.IsBoxPresent() == false ) return(0.0);
    CPoint box = Restart.GetBox();
    return(box.x);
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::getBoxBSize(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Snapshot::getBoxBSize()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    if( Restart.IsBoxPresent() == false ) return(0.0);
    CPoint box = Restart.GetBox();
    return(box.y);
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::getBoxCSize(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Snapshot::getBoxCSize()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    if( Restart.IsBoxPresent() == false ) return(0.0);
    CPoint box = Restart.GetBox();
    return(box.z);
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::getLargestSphereRadius(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Snapshot::getLargestSphereRadius()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    if( Restart.IsBoxPresent() == false ) return(0.0);
    Restart.GetTopology()->BoxInfo.SetBoxDimmensions(Restart.GetBox());
    Restart.GetTopology()->BoxInfo.SetBoxAngles(Restart.GetAngles());
    Restart.GetTopology()->BoxInfo.UpdateBoxMatrices();
    return( Restart.GetTopology()->BoxInfo.GetLargestSphereRadius());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QSnapshot::center(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Snapshot::center([selection,key1,key2,...])" << endl;
        return(false);
    }

// options ---------------------------------------
    bool origin = IsArgumentKeySelected("origin");
    bool nomass = IsArgumentKeySelected("nomass");
    QSelection* p_qsel = NULL;
    FindArgAsObject<QSelection*>("[selection,key1,key2,...]","Selection",p_qsel,false);

    value = CheckArgumentsUsage("[selection,key1,key2,...]");
    if( value.isError() ) return(value);

// execute code ----------------------------------
    // update topology box info
    Restart.GetTopology()->BoxInfo.SetBoxDimmensions(Restart.GetBox());
    Restart.GetTopology()->BoxInfo.SetBoxAngles(Restart.GetAngles());
    Restart.GetTopology()->BoxInfo.UpdateBoxMatrices();

    // shift vector
    CPoint mov;

    if( p_qsel ){
  // FIXME      mov = - p_qsel->GetCOMPoint(nomass);
    } else {
        mov = - GetCOMPoint(nomass);
    }

    if( ! origin ){
        mov += Restart.GetTopology()->BoxInfo.GetBoxCenter();
    }

    // move all atoms
    for(int i=0; i < Restart.GetNumberOfAtoms(); i++){
        CPoint pos = Restart.GetPosition(i);
        pos += mov;
        Restart.SetPosition(i,pos);
    }

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::image(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Snapshot::image([key1,key2,...])" << endl;
        return(false);
    }

// options ---------------------------------------
    bool origin = IsArgumentKeySelected("origin");
    bool familiar = IsArgumentKeySelected("familiar");
    bool bymol = IsArgumentKeySelected("bymol");
    bool byres = IsArgumentKeySelected("byres");
    bool byatom = IsArgumentKeySelected("byatom");

    value = CheckArgumentsUsage("[key1,key2,...]");
    if( value.isError() ) return(value);

    if( (byres && bymol) || (byres && byatom) || (bymol && byatom) ){
        return( ThrowError("[key1,key2,...]","bymol, byres, and byatom keys are mutually exclusive") );
    }
    if( ! (bymol || byres || byatom) ){
        bymol = true;
    }

// execute code ----------------------------------
    // update topology box info
    Restart.GetTopology()->BoxInfo.SetBoxDimmensions(Restart.GetBox());
    Restart.GetTopology()->BoxInfo.SetBoxAngles(Restart.GetAngles());
    Restart.GetTopology()->BoxInfo.UpdateBoxMatrices();

    // by molecule imaging ---------------------
    if( bymol ){
        int start_index = 0;
        while( start_index < Restart.GetNumberOfAtoms() ) {
            int mol_id = Restart.GetTopology()->AtomList.GetAtom(start_index)->GetMoleculeIndex();

            // calculate molecule COM
            CPoint  com;
            double  tmass = 0.0;
            int     count = 0;
            for(int i=start_index; i < Restart.GetNumberOfAtoms(); i++) {
                if( mol_id != Restart.GetTopology()->AtomList.GetAtom(i)->GetMoleculeIndex() ) break;
                double mass = Restart.GetTopology()->AtomList.GetAtom(i)->GetMass();
                com += Restart.GetPosition(i)*mass;
                tmass += mass;
                count++;
            }
            if( tmass != 0.0 ) com /= tmass;

            // image com
            CPoint  icom;
            icom = Restart.GetTopology()->BoxInfo.ImagePoint(com,0,0,0,origin,familiar);
            icom = icom - com;

            // move molecule
            for(int i=start_index; i < start_index+count; i++) {
                CPoint pos = Restart.GetPosition(i);
                pos += icom;
                Restart.SetPosition(i,pos);
            }
            start_index += count;
        }
        return(value);
    }

    // by residue imaging ---------------------
    if( byres ){
        int start_index = 0;
        while( start_index < Restart.GetNumberOfAtoms() ) {
            CAmberResidue* res_id = Restart.GetTopology()->AtomList.GetAtom(start_index)->GetResidue();

            // calculate residue COM
            CPoint  com;
            double  tmass = 0.0;
            int     count = 0;
            for(int i=start_index; i < Restart.GetNumberOfAtoms(); i++) {
                if( res_id != Restart.GetTopology()->AtomList.GetAtom(i)->GetResidue() ) break;
                double mass = Restart.GetTopology()->AtomList.GetAtom(i)->GetMass();
                com += Restart.GetPosition(i)*mass;
                tmass += mass;
                count++;
            }
            if( tmass != 0.0 ) com /= tmass;

            // image com
            CPoint  icom;
            icom = Restart.GetTopology()->BoxInfo.ImagePoint(com,0,0,0,origin,familiar);
            icom = icom - com;

            // move molecule
            for(int i=start_index; i < start_index+count; i++) {
                CPoint pos = Restart.GetPosition(i);
                pos += icom;
                Restart.SetPosition(i,pos);
            }
            start_index += count;
        }
        return(value);
    }

    // by atom imaging ----------------------
    if( byatom ){
        cout << "byatom" << endl;
        for(int i=0; i < Restart.GetNumberOfAtoms(); i++) {
            CPoint pos = Restart.GetPosition(i);
            pos = Restart.GetTopology()->BoxInfo.ImagePoint(pos,0,0,0,origin,familiar);
            Restart.SetPosition(i,pos);
        }
        return(value);
    }

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::rmsdFit(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Snapshot::rmsdFit(snapshot[,selection,key1,key2,...])" << endl;
        return(false);
    }

// arguments -------------------------------------
    value = CheckMinimumNumberOfArguments("snapshot[selection,key1,key2,...]",1);
    if( value.isError() ) return(value);

    QSnapshot* p_qref;
    value = GetArgAsObject<QSnapshot*>("snapshot[selection,key1,key2,...]","snapshot","Snapshot",1,p_qref);
    if( value.isError() ) return(value);

// options ---------------------------------------
    bool nomass = IsArgumentKeySelected("nomass");

    QSelection* p_qsel = NULL;
    FindArgAsObject<QSelection*>("snapshot[selection,key1,key2,...]","Selection",p_qsel,false);

    value = CheckArgumentsUsage("[key1,key2,...]");
    if( value.isError() ) return(value);

// other checks ----------------------------------
    if( Restart.IsBoxPresent() == true ){
        return( ThrowError("snapshot[selection,key1,key2,...]","target snapshot cannot contain box") );
    }

    if( Restart.GetNumberOfAtoms() != p_qref->Restart.GetNumberOfAtoms() ){
        CSmallString error;
        error << "number of atoms does not match, reference ("
              << p_qref->Restart.GetNumberOfAtoms() << "), target (" << Restart.GetNumberOfAtoms() << ")";
        return( ThrowError("snapshot[selection,key1,key2,...]",error) );
    }

    if( p_qsel ){
        if( p_qsel->Mask.GetCoordinates()->GetNumberOfAtoms() != Restart.GetNumberOfAtoms() ){
        CSmallString error;
        error << "number of atoms does not match, snapshot associated with selection ("
              << p_qref->Restart.GetNumberOfAtoms() << "), target (" << Restart.GetNumberOfAtoms() << ")";
        return( ThrowError("snapshot[selection,key1,key2,...]",error) );
        }
    }

// execute code ----------------------------------
    CTransformation trans;

    // get COMs
    CPoint com1;
    CPoint com2;
    double  totalmass1 = 0.0;
    double  totalmass2 = 0.0;

    if( p_qsel ){
        for(int s=0; s < p_qsel->Mask.GetNumberOfSelectedAtoms(); s++) {
            int i = p_qsel->Mask.GetSelectedAtomCondensed(s)->GetAtomIndex();
            CPoint tpos = Restart.GetPosition(i);
            CPoint rpos = p_qref->Restart.GetPosition(i);
            double tmass;
            double rmass;
            if( nomass ){
                tmass = 1.0;
                rmass = 1.0;
            } else {
                tmass = Restart.GetMass(i);
                rmass = p_qref->Restart.GetMass(i);
            }
            com1 += rpos*rmass;
            totalmass1 += rmass;

            com2 += tpos*tmass;
            totalmass2 += tmass;
        }
    } else {
        for(int i=0; i < Restart.GetNumberOfAtoms(); i++) {
            CPoint tpos = Restart.GetPosition(i);
            CPoint rpos = p_qref->Restart.GetPosition(i);
            double tmass;
            double rmass;
            if( nomass ){
                tmass = 1.0;
                rmass = 1.0;
            } else {
                tmass = Restart.GetMass(i);
                rmass = p_qref->Restart.GetMass(i);
            }
            com1 += rpos*rmass;
            totalmass1 += rmass;

            com2 += tpos*tmass;
            totalmass2 += tmass;
        }
    }

    if( fabs(totalmass1-totalmass2) > 0.1 ){
        CSmallString error;
        error << "masses do not match, reference ("
              << CSmallString().DoubleToStr(totalmass2) << "), target (" << CSmallString().DoubleToStr(totalmass1) << ")";
        return( ThrowError("snapshot[selection,key1,key2,...]",error) );
    }

    if( totalmass1 > 0 ){
        com1 = com1 / totalmass1;
    }
    if( totalmass2 > 0 ){
        com2 = com2 / totalmass2;
    }

    if( totalmass1 < 0.01 ){
        CSmallString error;
        error << "no atoms to fit, total mass is zero";
        return( ThrowError("snapshot[selection,key1,key2,...]",error) );
    }

    // get the transformation matrix
    CSimpleSquareMatrix<double,4>   helper;
    double       rx,ry,rz;
    double       fx,fy,fz;
    double       xxyx, xxyy, xxyz;
    double       xyyx, xyyy, xyyz;
    double       xzyx, xzyy, xzyz;
    double       q[4];

// generate the upper triangle of the quadratic form matrix
    xxyx = 0.0;
    xxyy = 0.0;
    xxyz = 0.0;
    xyyx = 0.0;
    xyyy = 0.0;
    xyyz = 0.0;
    xzyx = 0.0;
    xzyy = 0.0;
    xzyz = 0.0;

    if( p_qsel ){
        for(int s=0; s < p_qsel->Mask.GetNumberOfSelectedAtoms(); s++) {
            int i = p_qsel->Mask.GetSelectedAtomCondensed(s)->GetAtomIndex();
            CPoint tpos = Restart.GetPosition(i);
            CPoint rpos = p_qref->Restart.GetPosition(i);
            double mass = 1.0;
            if( nomass ){
                mass = 1.0;
            } else {
                mass = Restart.GetMass(i);
            }
            fx = mass*(rpos.x - com1.x);
            fy = mass*(rpos.y - com1.y);
            fz = mass*(rpos.z - com1.z);

            rx = mass*(tpos.x - com2.x);
            ry = mass*(tpos.y - com2.y);
            rz = mass*(tpos.z - com2.z);

            xxyx += fx*rx;
            xxyy += fx*ry;
            xxyz += fx*rz;
            xyyx += fy*rx;
            xyyy += fy*ry;
            xyyz += fy*rz;
            xzyx += fz*rx;
            xzyy += fz*ry;
            xzyz += fz*rz;
        }
    } else {
        for(int i=0; i < Restart.GetNumberOfAtoms(); i++) {
            CPoint tpos = Restart.GetPosition(i);
            CPoint rpos = p_qref->Restart.GetPosition(i);
            double mass = 1.0;
            if( nomass ){
                mass = 1.0;
            } else {
                mass = Restart.GetMass(i);
            }
            fx = mass*(rpos.x - com1.x);
            fy = mass*(rpos.y - com1.y);
            fz = mass*(rpos.z - com1.z);

            rx = mass*(tpos.x - com2.x);
            ry = mass*(tpos.y - com2.y);
            rz = mass*(tpos.z - com2.z);

            xxyx += fx*rx;
            xxyy += fx*ry;
            xxyz += fx*rz;
            xyyx += fy*rx;
            xyyy += fy*ry;
            xyyz += fy*rz;
            xzyx += fz*rx;
            xzyy += fz*ry;
            xzyz += fz*rz;
        }
    }

    helper.Field[0][0] = xxyx + xyyy + xzyz;

    helper.Field[0][1] = xzyy - xyyz;
    helper.Field[1][1] = xxyx - xyyy - xzyz;

    helper.Field[0][2] = xxyz - xzyx;
    helper.Field[1][2] = xxyy + xyyx;
    helper.Field[2][2] = xyyy - xzyz - xxyx;

    helper.Field[0][3] = xyyx - xxyy;
    helper.Field[1][3] = xzyx + xxyz;
    helper.Field[2][3] = xyyz + xzyy;
    helper.Field[3][3] = xzyz - xxyx - xyyy;

// complete matrix
    helper.Field[1][0] = helper.Field[0][1];

    helper.Field[2][0] = helper.Field[0][2];
    helper.Field[2][1] = helper.Field[1][2];

    helper.Field[3][0] = helper.Field[0][3];
    helper.Field[3][1] = helper.Field[1][3];
    helper.Field[3][2] = helper.Field[2][3];

// diagonalize helper matrix
    helper.EigenProblem(q);

// extract the desired quaternion
    q[0] = helper.Field[0][3];
    q[1] = helper.Field[1][3];
    q[2] = helper.Field[2][3];
    q[3] = helper.Field[3][3];

// generate the rotation matrix
    helper.SetUnit();

    helper.Field[0][0] = q[0]*q[0] + q[1]*q[1] - q[2]*q[2] - q[3]*q[3];
    helper.Field[1][0] = 2.0 * (q[1] * q[2] - q[0] * q[3]);
    helper.Field[2][0] = 2.0 * (q[1] * q[3] + q[0] * q[2]);

    helper.Field[0][1] = 2.0 * (q[2] * q[1] + q[0] * q[3]);
    helper.Field[1][1] = q[0]*q[0] - q[1]*q[1] + q[2]*q[2] - q[3]*q[3];
    helper.Field[2][1] = 2.0 * (q[2] * q[3] - q[0] * q[1]);

    helper.Field[0][2] = 2.0 * (q[3] * q[1] - q[0] * q[2]);
    helper.Field[1][2] = 2.0 * (q[3] * q[2] + q[0] * q[1]);
    helper.Field[2][2] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];

    trans.Translate(-com2);
    trans.MultFromRightWith(helper);
    trans.Translate(com1);

    // transform snapshot
    double rms = 0.0;
    double totalmass = 0.0;
    for(int i=0; i < Restart.GetNumberOfAtoms(); i++) {
        bool selected = true;
        if( p_qsel ){
            selected = p_qsel->Mask.IsAtomSelected(i);
        }
        CPoint tpos = Restart.GetPosition(i);
        trans.Apply(tpos);
        Restart.SetPosition(i,tpos);
        if( Restart.AreVelocitiesLoaded() ){
            CPoint vel;
            Restart.SetVelocity(i,vel);
        }
        if( selected ){
            CPoint rpos = p_qref->Restart.GetPosition(i);
            double mass;
            if( nomass ){
                mass = 1.0;
            } else {
                mass = Restart.GetMass(i);
            }
            rms += mass*Square(tpos-rpos);
            totalmass += mass;
        }
    }
    // finalize
    rms = sqrt(rms / totalmass);

    return(rms);
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::getRMSD(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Snapshot::getRMSD(snapshot[,selection,key1,key2,...])" << endl;
        return(false);
    }

// arguments -------------------------------------
    value = CheckMinimumNumberOfArguments("snapshot[selection,key1,key2,...]",1);
    if( value.isError() ) return(value);

    QSnapshot* p_qref;
    value = GetArgAsObject<QSnapshot*>("snapshot[selection,key1,key2,...]","snapshot","Snapshot",1,p_qref);
    if( value.isError() ) return(value);

// options ---------------------------------------
    bool nomass = IsArgumentKeySelected("nomass");

    QSelection* p_qsel = NULL;
    FindArgAsObject<QSelection*>("snapshot[selection,key1,key2,...]","Selection",p_qsel);

    value = CheckArgumentsUsage("[key1,key2,...]");
    if( value.isError() ) return(value);

// other checks ----------------------------------
    if( Restart.IsBoxPresent() == true ){
        return( ThrowError("snapshot[selection,key1,key2,...]","target snapshot cannot contain box") );
    }

    if( Restart.GetNumberOfAtoms() != p_qref->Restart.GetNumberOfAtoms() ){
        CSmallString error;
        error << "number of atoms does not match, reference ("
              << p_qref->Restart.GetNumberOfAtoms() << "), target (" << Restart.GetNumberOfAtoms() << ")";
        return( ThrowError("snapshot[selection,key1,key2,...]",error) );
    }

    if( p_qsel ){
        if( p_qsel->Mask.GetCoordinates()->GetNumberOfAtoms() != Restart.GetNumberOfAtoms() ){
        CSmallString error;
        error << "number of atoms does not match, snapshot associated with selection ("
              << p_qref->Restart.GetNumberOfAtoms() << "), target (" << Restart.GetNumberOfAtoms() << ")";
        return( ThrowError("snapshot[selection,key1,key2,...]",error) );
        }
    }

// execute code ----------------------------------
    double  totalmass1 = 0.0;
    double  totalmass2 = 0.0;
    double  rms = 0.0;

    if( p_qsel ){
        for(int s=0; s < p_qsel->Mask.GetNumberOfSelectedAtoms(); s++) {
            int i = p_qsel->Mask.GetSelectedAtomCondensed(s)->GetAtomIndex();
            CPoint tpos = Restart.GetPosition(i);
            CPoint rpos = p_qref->Restart.GetPosition(i);
            double tmass;
            double rmass;
            if( nomass ){
                tmass = 1.0;
                rmass = 1.0;
            } else {
                tmass = Restart.GetMass(i);
                rmass = p_qref->Restart.GetMass(i);
            }
            rms += rmass*Square(tpos-rpos);
            totalmass1 += rmass;
            totalmass2 += tmass;
        }
    } else {
        for(int i=0; i < Restart.GetNumberOfAtoms(); i++) {
            CPoint tpos = Restart.GetPosition(i);
            CPoint rpos = p_qref->Restart.GetPosition(i);
            double tmass;
            double rmass;
            if( nomass ){
                tmass = 1.0;
                rmass = 1.0;
            } else {
                tmass = Restart.GetMass(i);
                rmass = p_qref->Restart.GetMass(i);
            }
            rms += rmass*Square(tpos-rpos);
            totalmass1 += rmass;
            totalmass2 += tmass;
        }
    }

    if( fabs(totalmass1-totalmass2) > 0.1 ){
        CSmallString error;
        error << "Snapshot::getRMSD(snapshot) - masses do not match, reference ("
              << CSmallString().DoubleToStr(totalmass2) << "), target (" << CSmallString().DoubleToStr(totalmass1) << ")";
        context()->throwError(QString(error));
        return(0.0);
    }

    if( totalmass1 < 0.01 ){
        CSmallString error;
        error << "Snapshot::getRMSD(snapshot) - no atoms for RMS calculation, total mass is zero";
        context()->throwError(QString(error));
        return(0.0);
    }

    // finalize
    rms = sqrt(rms / totalmass1);

    return(rms);
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::getCOM(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: point Snapshot::getCOM([selection][,point][,key1,...])" << endl;
        return(false);
    }

// arguments -------------------------------------
    QSelection* p_qsel = NULL;
    FindArgAsObject<QSelection*>("[selection][,point][,key1,...]","Selection",p_qsel);

    QPoint* p_qpts = NULL;
    FindArgAsObject<QPoint*>("[selection][,point][,key1,...]","Point",p_qpts);

    bool nomass = IsArgumentKeySelected("nomass");

    value = CheckArgumentsUsage("[key1,key2,...]");
    if( value.isError() ) return(value);

// execute code ----------------------------------
    CPoint com;
    if( p_qsel != NULL){
        CAmberTopology* p_top = Restart.GetTopology();
        double totmass = 0.0;
        for(int s=0; s < p_qsel->Mask.GetNumberOfSelectedAtoms(); s++) {
            int i = p_qsel->Mask.GetSelectedAtomCondensed(s)->GetAtomIndex();
            CAmberAtom* p_atom = p_top->AtomList.GetAtom(i);
            // topology and snapshot have the same number of atoms
            double mass;
            if( nomass ){
                mass = 1.0;
            } else {
                mass = p_atom->GetMass();
            }
            CPoint p1 = Restart.GetPosition(i);
            com += p1*mass;
            totmass += mass;
        }
    } else {
        CAmberTopology* p_top = Restart.GetTopology();
        double totmass = 0.0;
        for(int i=0; i < GetNumOfAtoms(); i++) {
            CAmberAtom* p_atom = p_top->AtomList.GetAtom(i);
            // topology and snapshot have the same number of atoms
            double mass;
            if( nomass ){
                mass = 1.0;
            } else {
                mass = p_atom->GetMass();
            }
            CPoint p1 = Restart.GetPosition(i);
            com += p1*mass;
            totmass += mass;
        }
        if( totmass > 0.0 ) com /= totmass;
    }

    if( p_qpts == NULL ){
        QPoint* p_obj = new QPoint();
        p_obj->Point = com;
        QScriptValue obj = engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership);
        return(obj);
    } else {
        p_qpts->Point = com;
        for(int i=1;i<=GetArgumentCount();i++){
            if( IsArgumentObject<QPoint*>(i) ) return(GetArgument(i));
        }
        return(false);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QSnapshot::getNumOfAtoms(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Snapshot::getNumOfAtoms()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    return(Restart.GetNumberOfAtoms());
}

//------------------------------------------------------------------------------

QScriptValue QSnapshot::getPosition(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: point Snapshot::getPosition(index)" << endl;
        sout << "       point Snapshot::getPosition(index,point)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index[,point]",1,2);
    if( value.isError() ) return(value);

// execute code ----------------------------------
    int index;

    if( GetArgumentCount() == 1 ){
        value = GetArgAsInt("index","index",1,index);
        if( value.isError() ) return(value);

        if( (index < 0) || (index >= Restart.GetNumberOfAtoms()) ){
            return( ThrowError("index", "index is out-of-range") );
        }

        QPoint* p_obj = new QPoint();
        p_obj->Point = Restart.GetPosition(index);
        QScriptValue obj = engine()->newQObject(p_obj, QScriptEngine::ScriptOwnership);
        return(obj);
    } else {
        value = GetArgAsInt("index,point","index",1,index);
        if( value.isError() ) return(value);

        if( (index < 0) || (index >= Restart.GetNumberOfAtoms()) ){
            return( ThrowError("index", "index is out-of-range") );
        }

        QPoint* p_obj = NULL;
        value = GetArgAsObject<QPoint*>("index,point","point","Point",2,p_obj);
        if( value.isError() ) return(value);

        p_obj->Point = Restart.GetPosition(index);
        return( GetArgument(2) );
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



