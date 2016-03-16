// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2015 Michal Ruzicka, michalruz@mail.muni.cz
//    Copyright (C) 2015 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <fstream>
#include <errno.h>

#include <Qx3DNAStatistics.hpp>
#include <TerminalStr.hpp>
#include <Qx3DNA.hpp>
#include <ErrorSystem.hpp>

#include <Qx3DNAStatistics.moc>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void Qx3DNAStatistics::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(Qx3DNAStatistics::New);
    QScriptValue metaObject = engine.newQMetaObject(&Qx3DNAStatistics::staticMetaObject, ctor);
    engine.globalObject().setProperty("x3DNADatabase", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNAStatistics::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    QCATsScriptable scriptable("x3DNADatabase");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "x3DNADatabase object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new x3DNADatabase()" << endl;
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

Qx3DNAStatistics::Qx3DNAStatistics(void)
    : QCATsScriptable("x3DNADatabase")
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue Qx3DNAStatistics::registerData(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNADatabase::registerData(x3DNA)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("set",1);
    if( value.isError() ) return(value);

    bool set = false;
    value = GetArgAsBool("set","set",1,set);
    if( value.isError() ) return(value);

// execute ---------------------------------------


    return(true);
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNAStatistics::clear(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: x3DNADatabase::clear()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    ClearAll();

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNAStatistics::printResults(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNADatabase::printResults(name[,options])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name",1);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name","name",1,name);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    // open file
    CFileName fileName = name;
    ofstream ofs;

    ofs.open( fileName );
    if( ofs.fail() ) {
        CSmallString error;
        error << "unable to open analysis output file " << fileName;
        error << " (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }

    PrintLocalBPParams(ofs);
    PrintLocalBPStepParams(ofs);
    PrintLocalBPHelParams(ofs);

    ofs.close();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void Qx3DNAStatistics::ClearAll(void)
{
    // clear all local data
}

//------------------------------------------------------------------------------

void Qx3DNAStatistics::PrintLocalBPParams(ofstream& vout)
{
    // TODO

    vout << "# ResIDA ResIDB BasePair Abundance Shear<> s(Shear) Stretch<> s(Stretch) Stagger<> s(Stagger) Buckle<> s(Buckle) Propeller<> s(Propeller) Opening<> s(Opening)" << endl;
    vout << "#------- ------ -------- --------- ------- -------- --------- ---------- --------- ---------- -------- --------- ----------- ------------ --------- ----------" << endl;

}

//------------------------------------------------------------------------------

void Qx3DNAStatistics::PrintLocalBPStepParams(ofstream& vout)
{
    // TODO
}

//------------------------------------------------------------------------------

void Qx3DNAStatistics::PrintLocalBPHelParams(ofstream& vout)
{
    // TODO
}

//------------------------------------------------------------------------------

void Qx3DNAStatistics::RegisterData(const CLocalBP& data)
{
//    // register key
//    BasePairIDs.insert(data.ID);
//    // add new data to key
//    LocalBPSnapshots[data.ID].push_back(data.Data);
}

//------------------------------------------------------------------------------

void Qx3DNAStatistics::RegisterData(const CLocalBPStep& data)
{

}

//------------------------------------------------------------------------------

void Qx3DNAStatistics::RegisterData(const CLocalBPHel& data)
{

}

//------------------------------------------------------------------------------

