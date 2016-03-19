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
#include <iomanip>
#include <errno.h>
#include <QNAStat.hpp>
#include <TerminalStr.hpp>
#include <Qx3DNA.hpp>
#include <ErrorSystem.hpp>

#include <QNAStat.moc>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QNAStat::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QNAStat::New);
    QScriptValue metaObject = engine.newQMetaObject(&QNAStat::staticMetaObject, ctor);
    engine.globalObject().setProperty("NAStat", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QNAStat::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    QCATsScriptable scriptable("NAStat");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "NAStat object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new NAStat()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    QNAStat* p_obj = new QNAStat();
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QNAStat::QNAStat(void)
    : QCATsScriptable("NAStat")
{
    NumOfSnapshots = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QNAStat::addSample(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool NAStat::addSample(x3dna)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("x3dna",1);
    if( value.isError() ) return(value);

    Qx3DNA* p_x3dna;
    value = GetArgAsObject<Qx3DNA*>("x3dna","x3dna","x3DNA",1,p_x3dna);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    NumOfSnapshots++;

    RegisterBPData(p_x3dna);
    RegisterBPStepData(p_x3dna);
    RegisterBPHelData(p_x3dna);

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QNAStat::clear(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: NAStat::clear()" << endl;
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

QScriptValue QNAStat::printResults(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool NAStat::printResults(name)" << endl;
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

    PrintBPParams(ofs);
    PrintBPStepParams(ofs);
    PrintBPHelParams(ofs);

    ofs.close();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QNAStat::ClearAll(void)
{
    BPIDs.clear();
    BPStepIDs.clear();

    NumOfSnapshots = 0;
    LocalBPStat.clear();
    LocalBPStepStat.clear();
    LocalBPHelStat.clear();
}

//------------------------------------------------------------------------------

void QNAStat::PrintBPParams(ofstream& vout)
{
    vout << "# Local Base Pair Parameters" << endl;
    vout << "# index ResIDA ResIDB BasePair Abundance <shear> s(shear) <stretch> s(stretch) <stagger> s(stagger) <buckle> s(buckle) <propeller> s(propeller) <opening> s(opening)" << endl;
    vout << "#------ ------ ------ -------- --------- ------- -------- --------- ---------- --------- ---------- -------- --------- ----------- ------------ --------- ----------" << endl;

    std::map<CNABPID,CNALocalBPStatPtr>::iterator  it = LocalBPStat.begin();
    std::map<CNABPID,CNALocalBPStatPtr>::iterator  ie = LocalBPStat.end();

    int i = 1;
    while( it != ie ){
        CNABPID  bp_id = it->first;
        CNALocalBPStatPtr bp_stat = it->second;
        vout << right << setw(7) << i << " ";
        // ResIDA ResIDB BasePair
        vout << right << setw(6) << bp_id.ResIDA + 1 << " " << right << setw(6) << bp_id.ResIDB + 1 << " " << right << setw(8) << bp_id.Name << " ";
        // Abundance
        double abundance = (double)bp_stat->NumOfSamples * 100.0 /  (double)NumOfSnapshots;
        vout << right << fixed << setprecision(2) << setw(9) << abundance  << " ";
        // Shear
        vout << right << fixed << setprecision(2) << setw(7) << bp_stat->Sum.Shear / bp_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(8) << sqrt( bp_stat->NumOfSamples * bp_stat->Sum2.Shear - bp_stat->Sum.Shear * bp_stat->Sum.Shear )  / bp_stat->NumOfSamples << " "; // sigma
        // Stretch
        vout << right << fixed << setprecision(2) << setw(9) << bp_stat->Sum.Stretch / bp_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(10) << sqrt( bp_stat->NumOfSamples * bp_stat->Sum2.Stretch - bp_stat->Sum.Stretch * bp_stat->Sum.Stretch )  / bp_stat->NumOfSamples << " "; // sigma
        // Stagger
        vout << right << fixed << setprecision(2) << setw(9) << bp_stat->Sum.Stagger / bp_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(10) << sqrt( bp_stat->NumOfSamples * bp_stat->Sum2.Stagger - bp_stat->Sum.Stagger * bp_stat->Sum.Stagger )  / bp_stat->NumOfSamples << " "; // sigma
        // Buckle
        vout << right << fixed << setprecision(2) << setw(8) << bp_stat->Sum.Buckle / bp_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(9) << sqrt( bp_stat->NumOfSamples * bp_stat->Sum2.Buckle - bp_stat->Sum.Buckle * bp_stat->Sum.Buckle )  / bp_stat->NumOfSamples << " "; // sigma
        // Propeller
        vout << right << fixed << setprecision(2) << setw(11) << bp_stat->Sum.Propeller / bp_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(12) << sqrt( bp_stat->NumOfSamples * bp_stat->Sum2.Propeller - bp_stat->Sum.Propeller * bp_stat->Sum.Propeller )  / bp_stat->NumOfSamples << " "; // sigma
        // Opening
        vout << right << fixed << setprecision(2) << setw(9) << bp_stat->Sum.Opening / bp_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(10) << sqrt( bp_stat->NumOfSamples * bp_stat->Sum2.Opening - bp_stat->Sum.Opening * bp_stat->Sum.Opening )  / bp_stat->NumOfSamples << " "; // sigma
        vout << endl;
        i++;
        it++;
    }
}

//------------------------------------------------------------------------------

void QNAStat::PrintBPStepParams(ofstream& vout)
{
    vout << "# Local Base Pair Step Parameters" << endl;
    vout << "# index ResIDA ResIDB ResIDC ResIDD  BPStep  Abundance " << endl;
    vout << "#------ ------ ------ ------ ------ -------- --------- " << endl;

    std::map<CNABPStepID,CNALocalBPStepStatPtr>::iterator  it = LocalBPStepStat.begin();
    std::map<CNABPStepID,CNALocalBPStepStatPtr>::iterator  ie = LocalBPStepStat.end();

    int i = 1;
    while( it != ie ){
        CNABPStepID  bpstep_id = it->first;
        CNALocalBPStepStatPtr bpstep_stat = it->second;
        vout << right << setw(7) << i << " ";
        // ResIDA ResIDB BasePair
        vout << right << setw(6) << bpstep_id.ResIDA + 1 << " "
             << right << setw(6) << bpstep_id.ResIDB + 1 << " "
             << right << setw(6) << bpstep_id.ResIDC + 1 << " "
             << right << setw(6) << bpstep_id.ResIDD + 1 << right << setw(8) << bpstep_id.Step << " ";
        // Abundance
        double abundance = (double)bpstep_stat->NumOfSamples * 100.0 /  (double)NumOfSnapshots;
        vout << right << fixed << setprecision(2) << setw(9) << abundance  << " ";
        vout << endl;
        i++;
        it++;
    }
}

//------------------------------------------------------------------------------

void QNAStat::PrintBPHelParams(ofstream& vout)
{
    // TODO
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QNAStat::RegisterBPData(Qx3DNA* p_data)
{
    if( p_data == NULL ) return;    // no valid input

// local bp parameters
    std::vector<CNALocalBPPar>::iterator  it = p_data->LocalBPPar.begin();
    std::vector<CNALocalBPPar>::iterator  ie = p_data->LocalBPPar.end();

    while( it != ie ){
        CNALocalBPPar local_bp = *it;
        if( local_bp.Valid ){
            CNABPID bp_id(p_data->BPIDs[local_bp.ID]);
            if (LocalBPStat.find(bp_id) == LocalBPStat.end() ){
                // new data
                CNALocalBPStatPtr data(new CNALocalBPStat);
                LocalBPStat[bp_id] = data;
            }
            BPIDs.insert(bp_id);
            CNALocalBPStatPtr data = LocalBPStat[bp_id];
            if( data ){
                data->RegisterData(local_bp);
            }
        }
        it++;
    }
}

//------------------------------------------------------------------------------

void QNAStat::RegisterBPStepData(Qx3DNA* p_data)
{
    if( p_data == NULL ) return;    // no valid input

// local step parameters
    std::vector<CNALocalBPStepPar>::iterator  it = p_data->LocalBPStepPar.begin();
    std::vector<CNALocalBPStepPar>::iterator  ie = p_data->LocalBPStepPar.end();

    while( it != ie ){
        CNALocalBPStepPar local_bpstep = *it;
        if( local_bpstep.Valid ){
            CNABPStepID bpstep_id(p_data->BPStepIDs[local_bpstep.ID]);
            if (LocalBPStepStat.find(bpstep_id) == LocalBPStepStat.end() ){
                // new data
                CNALocalBPStepStatPtr data(new CNALocalBPStepStat);
                LocalBPStepStat[bpstep_id] = data;
            }
            BPStepIDs.insert(bpstep_id);
            CNALocalBPStepStatPtr data = LocalBPStepStat[bpstep_id];
            if( data ){
                data->RegisterData(local_bpstep);
            }
        }
        it++;
    }
}

//------------------------------------------------------------------------------

void QNAStat::RegisterBPHelData(Qx3DNA* p_data)
{
    if( p_data == NULL ) return;    // no valid input

// local helical parameters
    std::vector<CNALocalBPHelPar>::iterator  it = p_data->LocalBPHelPar.begin();
    std::vector<CNALocalBPHelPar>::iterator  ie = p_data->LocalBPHelPar.end();

    while( it != ie ){
        CNALocalBPHelPar local_bphel = *it;
        if( local_bphel.Valid ){
            CNABPStepID bpstep_id(p_data->BPStepIDs[local_bphel.ID]);
            if (LocalBPHelStat.find(bpstep_id) == LocalBPHelStat.end() ){
                // new data
                CNALocalBPHelStatPtr data(new CNALocalBPHelStat);
                LocalBPHelStat[bpstep_id] = data;
            }
            BPStepIDs.insert(bpstep_id);
            CNALocalBPHelStatPtr data = LocalBPHelStat[bpstep_id];
            if( data ){
                data->RegisterData(local_bphel);
            }
        }
        it++;
    }
}

//------------------------------------------------------------------------------

