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
#include <QPMFLib.hpp>
#include <QPMFLib.moc>
#include <QTopology.hpp>
#include <QSnapshot.hpp>
#include <TerminalStr.hpp>
#include <PMFCATsDriver.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QPMFLib::Register(QScriptEngine& engine)
{
    QObject* p_obj = new QPMFLib;
    QScriptValue objectValue = engine.newQObject(p_obj);
    engine.globalObject().setProperty("PMFLib", objectValue);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QPMFLib::QPMFLib(void)
    : QCATsScriptable("PMFLib")
{
    Initialized = false;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QPMFLib::init(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool init(snapshot,ctrlname)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("snapshot,ctrlname",2);
    if( value.isError() ) return(value);

    QSnapshot* p_snap;
    value = GetArgAsObject<QSnapshot*>("snapshot,ctrlname","snapshot","Snapshot",1,p_snap);
    if( value.isError() ) return(value);

    QString ctrlname;
    value = GetArgAsString("snapshot,ctrlname","ctrlname",2,ctrlname);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( Initialized ){
        return( ThrowError("snapshot,ctrlname","already initialized") );
    }

    if( p_snap->Restart.GetNumberOfAtoms() == 0 ){
        return( ThrowError("snapshot,ctrlname","no atoms in snapshot") );
    }

    CAmberTopology* p_top = p_snap->Restart.GetTopology();
    int box_type = 0;
    switch(p_top->BoxInfo.GetType()){
        case AMBER_BOX_NONE: box_type = 0;
            break;
        case AMBER_BOX_STANDARD: box_type = 1;
            break;
        case AMBER_BOX_OCTAHEDRAL: box_type = 2;
            break;
    }

    CPoint bdimm =  p_snap->Restart.GetBox();
    CPoint angles =  p_snap->Restart.GetAngles();

    CPMFCATsDriver::BeginInit(ctrlname,p_top->AtomList.GetNumberOfAtoms(),p_top->ResidueList.GetNumberOfResidues(),
                           box_type,bdimm.x,bdimm.y,bdimm.z,angles.x,angles.y,angles.z);

    for(int i=0; i < p_top->ResidueList.GetNumberOfResidues(); i++){
        CAmberResidue* p_res = p_top->ResidueList.GetResidue(i);
        CPMFCATsDriver::SetResidue(i,p_res->GetName(),p_res->GetFirstAtomIndex());
    }

    std::vector<double> masses;
    std::vector<double> xyz;

    masses.reserve(p_top->AtomList.GetNumberOfAtoms());
    xyz.reserve(p_top->AtomList.GetNumberOfAtoms()*3);

    for(int i=0; i < p_top->AtomList.GetNumberOfAtoms(); i++){
        CAmberAtom* p_atm = p_top->AtomList.GetAtom(i);
        CPMFCATsDriver::SetAtom(i,p_atm->GetName(),p_atm->GetType());
        masses.push_back(p_snap->Restart.GetMass(i));
        CPoint pos = p_snap->Restart.GetPosition(i);
        xyz.push_back(pos.x);
        xyz.push_back(pos.y);
        xyz.push_back(pos.z);
    }

    CPMFCATsDriver::EndInit(p_top->AtomList.GetNumberOfAtoms(),masses,xyz);

    Initialized = true;

    return( value );
}

//------------------------------------------------------------------------------

QScriptValue QPMFLib::setCoordinates(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: setCoordinates(snapshot)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("snapshot",1);
    if( value.isError() ) return(value);

    QSnapshot* p_snap;
    value = GetArgAsObject<QSnapshot*>("snapshot","snapshot","Snapshot",1,p_snap);
    if( value.isError() ) return(value);

    if( Initialized == false ){
        return( ThrowError("snapshot","PMFLIb is not initialized") );
    }

// execute ---------------------------------------
    CPoint box = p_snap->Restart.GetBox();
    CPoint angles = p_snap->Restart.GetAngles();
    CPMFCATsDriver::SetCoordinates(p_snap->Restart.GetNumberOfAtoms(),p_snap->Restart.GetCoordinatesBuffer(),
                                   box.x,box.y,box.z,angles.x,angles.y,angles.z);
    return( value );
}

//------------------------------------------------------------------------------

QScriptValue QPMFLib::getNumOfCVs(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int getNumOfCVs()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    value = CPMFCATsDriver::GetNumberOfCVs();
    return( value );
}

//------------------------------------------------------------------------------

QScriptValue QPMFLib::getCVValue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double getCVValue(name/index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name/index",1);
    if( value.isError() ) return(value);

    if( IsArgumentInt(1) ){
        int indx = 0;
        value = GetArgAsInt("index","name/index",1,indx);
        if( value.isError() ) return(value);

        if( Initialized == false ){
            return( ThrowError("name/index","PMFLib is not initialized") );
        }

    // execute ---------------------------------------
        value = CPMFCATsDriver::GetCVValue(indx);
        return( value );
    } else {
        QString cvname;
        value = GetArgAsString("name","name/index",1,cvname);
        if( value.isError() ) return(value);

        if( Initialized == false ){
            return( ThrowError("name/index","PMFLib is not initialized") );
        }

    // execute ---------------------------------------
        value = CPMFCATsDriver::GetCVValue(cvname.toStdString().c_str());
        return( value );
    }
}

//------------------------------------------------------------------------------

QScriptValue QPMFLib::getCVName(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string getCVName(index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    int indx = 0;
    value = GetArgAsInt("index","index",1,indx);
    if( value.isError() ) return(value);

    if( Initialized == false ){
        return( ThrowError("index","PMFLib is not initialized") );
    }

// execute ---------------------------------------
    value = QString(CPMFCATsDriver::GetCVName(indx));
    return( value );
}

//------------------------------------------------------------------------------

QScriptValue QPMFLib::getCVType(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string getCVType(name/index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name/index",1);
    if( value.isError() ) return(value);

    if( IsArgumentInt(1) ){
        int indx = 0;
        value = GetArgAsInt("index","name/index",1,indx);
        if( value.isError() ) return(value);

        if( Initialized == false ){
            return( ThrowError("name/index","PMFLib is not initialized") );
        }

    // execute ---------------------------------------
        value = QString(CPMFCATsDriver::GetCVType(indx));
        return( value );
    } else {
        QString cvname;
        value = GetArgAsString("name","name/index",1,cvname);
        if( value.isError() ) return(value);

        if( Initialized == false ){
            return( ThrowError("name/index","PMFLib is not initialized") );
        }

    // execute ---------------------------------------
        value = QString(CPMFCATsDriver::GetCVType(cvname.toStdString().c_str()));
        return( value );
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


