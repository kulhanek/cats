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
#include <QAverageSnapshot.hpp>
#include <QAverageSnapshot.moc>
#include <QTopology.hpp>
#include <QSnapshot.hpp>
#include <QSelection.hpp>
#include <SmallString.hpp>
#include <ErrorSystem.hpp>
#include <TerminalStr.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QAverageSnapshot::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QAverageSnapshot::New);
    QScriptValue metaObject = engine.newQMetaObject(&QAverageSnapshot::staticMetaObject, ctor);
    engine.globalObject().setProperty("AverageSnapshot", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QAverageSnapshot::New(QScriptContext *context,
                            QScriptEngine *engine)
{
    QCATsScriptable scriptable("AverageSnapshot");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "AverageSnapshot object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new AverageSnapshot(topology)" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("topology",1);
    if( value.isError() ) return(value);

// check arguments & execute ---------------------
    QTopology* p_top;
    value = scriptable.GetArgAsObject<QTopology*>("topology","topology","Topology",1,p_top);
    if( value.isError() ) return(value);
    QAverageSnapshot* p_snap = new QAverageSnapshot(scriptable.GetArgument(1));
    value = engine->newQObject(p_snap, QScriptEngine::ScriptOwnership);
    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QAverageSnapshot::QAverageSnapshot(const QScriptValue& top)
    : QCATsScriptable("AverageSnapshot")
{
    RegisterAsWeakObject(top);
    Restart.AssignTopology(&GetQTopology()->Topology);
    Restart.Create();
    SamplingMode = false;
    NumOfSnapshostsForCrd = 0;
    NumOfSnapshostsForVel = 0;
    NumOfSnapshostsForBox = 0;
}

//------------------------------------------------------------------------------

void QAverageSnapshot::CleanData(void)
{
    Restart.Release();
    SamplingMode = false;
    NumOfSnapshostsForCrd = 0;
    NumOfSnapshostsForVel = 0;
    NumOfSnapshostsForBox = 0;
}

//------------------------------------------------------------------------------

void QAverageSnapshot::UpdateData(void)
{
    Restart.Create();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QAverageSnapshot::begin(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: AverageSnapshot::begin()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( SamplingMode == true ){
        return( ThrowError("","already in sampling mode") );
    }
    
    BeginAccumulation();
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QAverageSnapshot::addSample(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: AverageSnapshot::addSample(snapshot[,selection])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("snapshot[,selection]",1);
    if( value.isError() ) return(value);

    QSnapshot* p_snap;
    value = GetArgAsObject<QSnapshot*>("snapshot[,selection]","snapshot","Snapshot",1,p_snap);
    if( value.isError() ) return(value);

    QSelection* p_sel = NULL;
    if( GetArgumentCount() > 1 ){
        value = GetArgAsObject<QSelection*>("snapshot,selection","selection","Selection",2,p_sel);
        if( value.isError() ) return(value);
    }
    
    if( SamplingMode == false ){
        BeginAccumulation();
    }    

// execute ---------------------------------------
    if( p_sel == NULL ){
        if( Restart.GetNumberOfAtoms() != p_snap->GetNumOfAtoms() ){
            CSmallString error;
            error << "illegal number of atoms, source (" << p_snap->GetNumOfAtoms() << "), target (" << Restart.GetNumberOfAtoms() << ")";
            return( ThrowError("snapshot[,selection]",error) );
        }
        // add snapshot positions, velocities, and box
        for(int i=0; i < Restart.GetNumberOfAtoms(); i++) {
            CPoint pos1 = Restart.GetPosition(i);
            CPoint pos2 = p_snap->Restart.GetPosition(i);
            Restart.SetPosition(i,pos1+pos2);
            if( p_snap->Restart.AreVelocitiesLoaded() ){
                CPoint vel1 = Restart.GetVelocity(i);
                CPoint vel2 = p_snap->Restart.GetVelocity(i);
                Restart.SetVelocity(i,vel1+vel2);
            }
        }
        if( Restart.IsBoxPresent() && p_snap->Restart.IsBoxPresent() ) {
            CPoint box1 = Restart.GetBox();
            CPoint box2 = p_snap->Restart.GetBox();
            Restart.SetBox(box1+box2);
            CPoint ang1 = Restart.GetAngles();
            CPoint ang2 = p_snap->Restart.GetAngles();
            Restart.SetAngles(ang1+ang2);
        }

        // update number of snapshots for individual domains
        NumOfSnapshostsForCrd++;
        if( p_snap->Restart.AreVelocitiesLoaded() ) {
            NumOfSnapshostsForVel++;
        }
        if( Restart.IsBoxPresent() && p_snap->Restart.IsBoxPresent() ) {
            NumOfSnapshostsForBox++;
        }
    } else {
        if( Restart.GetNumberOfAtoms() != p_sel->Mask.GetNumberOfSelectedAtoms() ){
            CSmallString error;
            error << "illegal number of atoms, source (" << p_sel->Mask.GetNumberOfSelectedAtoms() << "), target (" << Restart.GetNumberOfAtoms() << ")";
            return( ThrowError("snapshot[,selection]",error) );
        }
        // add snapshot positions, velocities, and box
        CAmberRestart* p_src = p_sel->Mask.GetCoordinates();
        for(int i=0; i < Restart.GetNumberOfAtoms(); i++) {
            CPoint pos1 = Restart.GetPosition(i);
            int j = p_sel->Mask.GetSelectedAtomCondensed(i)->GetAtomIndex();
            CPoint pos2 = p_src->GetPosition(j);
            Restart.SetPosition(i,pos1+pos2);
            if( p_snap->Restart.AreVelocitiesLoaded() ){
                CPoint vel1 = Restart.GetVelocity(i);
                CPoint vel2 = p_src->GetVelocity(j);
                Restart.SetVelocity(i,vel1+vel2);
            }
        }
        if( Restart.IsBoxPresent() && p_src->IsBoxPresent() ) {
            CPoint box1 = Restart.GetBox();
            CPoint box2 = p_src->GetBox();
            Restart.SetBox(box1+box2);
            CPoint ang1 = Restart.GetAngles();
            CPoint ang2 = p_src->GetAngles();
            Restart.SetAngles(ang1+ang2);
        }

        // update number of snapshots for individual domains
        NumOfSnapshostsForCrd++;
        if( p_snap->Restart.AreVelocitiesLoaded() ) {
            NumOfSnapshostsForVel++;
        }
        if( Restart.IsBoxPresent() && p_src->IsBoxPresent() ) {
            NumOfSnapshostsForBox++;
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QAverageSnapshot::finish(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: AverageSnapshot::finish()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( SamplingMode == false ){
        return( ThrowError("","not in sampling mode") );
    }

    return(FinishAccumulation());
}

//------------------------------------------------------------------------------

QScriptValue QAverageSnapshot::save(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: AverageSnapshot::save(name)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name",1);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name","name",1,name);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( SamplingMode == true ){
        FinishAccumulation();
    }

    // execute code
    Restart.SetTitle("average_structure");
    bool result = Restart.Save(name.toLatin1().constData());
    return(result);
}

//------------------------------------------------------------------------------

void  QAverageSnapshot::BeginAccumulation(void)
{
    NumOfSnapshostsForCrd = 0;
    NumOfSnapshostsForVel = 0;
    NumOfSnapshostsForBox = 0;
    Restart.Create();   // reset previous data
    SamplingMode = true;
}

//------------------------------------------------------------------------------

bool  QAverageSnapshot::FinishAccumulation(void)
{
    SamplingMode = false;
    if( NumOfSnapshostsForCrd <= 0 ) return(false); // no snapshots were accumulated

    for(int i=0; i < Restart.GetNumberOfAtoms(); i++) {
        CPoint pos = Restart.GetPosition(i);
        pos /= NumOfSnapshostsForCrd;
        Restart.SetPosition(i,pos);
        if( NumOfSnapshostsForVel > 0 ){
            CPoint vel = Restart.GetVelocity(i);
            pos /= NumOfSnapshostsForVel;
            Restart.SetVelocity(i,vel);
        }
    }
    if( NumOfSnapshostsForBox > 0 ) {
        CPoint box = Restart.GetBox();
        box /= NumOfSnapshostsForBox;
        Restart.SetBox(box);
        CPoint ang = Restart.GetAngles();
        ang /= NumOfSnapshostsForBox;
        Restart.SetAngles(ang);
    }
    NumOfSnapshostsForCrd = 0;
    NumOfSnapshostsForVel = 0;
    NumOfSnapshostsForBox = 0;
    
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



