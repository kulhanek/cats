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
    FlipHelAxisEnabled = true;
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
    RegisterPData(p_x3dna);

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
    PrintPParams(ofs);

    ofs.close();

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QNAStat::enableFlipHelAxis(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: NAStat::enableFlipHelAxis(set)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("set",1);
    if( value.isError() ) return(value);

    bool set;
    value = GetArgAsBool("set","set",1,set);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    FlipHelAxisEnabled = set;
    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QNAStat::isFlipHelAxisEnabled(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool NAStat::isFlipHelAxisEnabled()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(FlipHelAxisEnabled);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QNAStat::ClearAll(void)
{
    BPIDs.clear();
    BPStepIDs.clear();

    NumOfSnapshots = 0;
    BPStat.clear();
    BPStepStat.clear();
    BPHelStat.clear();
    PStat.clear();
}

//------------------------------------------------------------------------------

void QNAStat::PrintBPParams(ofstream& vout)
{
    vout << "#  Base Pair Parameters" << endl;
    vout << "# index ResIDA ResIDB BasePair Abundance <shear> s(shear) <stretch> s(stretch) <stagger> s(stagger) <buckle> s(buckle) <propeller> s(propeller) <opening> s(opening)" << endl;
    vout << "#   1      2      3      4         5        6       7         8         9         10         11        12        13        14           15          16        17    " << endl;
    vout << "#------ ------ ------ -------- --------- ------- -------- --------- ---------- --------- ---------- -------- --------- ----------- ------------ --------- ----------" << endl;

    std::map<CNABPID,CNABPStatPtr>::iterator  it = BPStat.begin();
    std::map<CNABPID,CNABPStatPtr>::iterator  ie = BPStat.end();

    int i = 1;
    while( it != ie ){
        CNABPID  bp_id = it->first;
        CNABPStatPtr bp_stat = it->second;
        // index
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
    vout << endl;
}

//------------------------------------------------------------------------------

void QNAStat::PrintBPStepParams(ofstream& vout)
{
    vout << "#  Base Pair Step Parameters" << endl;
    vout << "# index ResIDA ResIDB ResIDC ResIDD  BPStep  Abundance <shift> s(shift) <slide> s(slide)  <rise> s(rise)  <tilt> s(tilt)  <roll> s(roll) <twist> s(twist)" << endl;
    vout << "#   1      2      3      4      5       6        7        8        9       10      11       12     13       14     15       16     17       18      19   " << endl;
    vout << "#------ ------ ------ ------ ------ -------- --------- ------- -------- ------- -------- ------- ------- ------- ------- ------- ------- ------- --------" << endl;

    std::map<CNABPStepID,CNABPStepStatPtr>::iterator  it = BPStepStat.begin();
    std::map<CNABPStepID,CNABPStepStatPtr>::iterator  ie = BPStepStat.end();

    int i = 1;
    while( it != ie ){
        CNABPStepID  bpstep_id = it->first;

        CNABPStepStatPtr bpstep_stat = it->second;

        vout << right << setw(7) << i << " ";
        // ResIDA ResIDB ResIDC ResIDD  BPStep
        vout << right << setw(6) << bpstep_id.ResIDA + 1 << " "
             << right << setw(6) << bpstep_id.ResIDB + 1 << " "
             << right << setw(6) << bpstep_id.ResIDC + 1 << " "
             << right << setw(6) << bpstep_id.ResIDD + 1 << " " << right << setw(8) << bpstep_id.Step << " ";
        // Abundance
        double abundance = (double)bpstep_stat->NumOfSamples * 100.0 /  (double)NumOfSnapshots;
        vout << right << fixed << setprecision(2) << setw(9) << abundance  << " ";
        // Shift
        vout << right << fixed << setprecision(2) << setw(7) << bpstep_stat->Sum.Shift / bpstep_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(8) << sqrt( bpstep_stat->NumOfSamples * bpstep_stat->Sum2.Shift - bpstep_stat->Sum.Shift * bpstep_stat->Sum.Shift )  / bpstep_stat->NumOfSamples << " "; // sigma
        // Slide
        vout << right << fixed << setprecision(2) << setw(7) << bpstep_stat->Sum.Slide / bpstep_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(8) << sqrt( bpstep_stat->NumOfSamples * bpstep_stat->Sum2.Slide - bpstep_stat->Sum.Slide * bpstep_stat->Sum.Slide )  / bpstep_stat->NumOfSamples << " "; // sigma
        // Rise
        vout << right << fixed << setprecision(2) << setw(7) << bpstep_stat->Sum.Rise / bpstep_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(7) << sqrt( bpstep_stat->NumOfSamples * bpstep_stat->Sum2.Rise - bpstep_stat->Sum.Rise * bpstep_stat->Sum.Rise )  / bpstep_stat->NumOfSamples << " "; // sigma
        // Tilt
        vout << right << fixed << setprecision(2) << setw(7) << bpstep_stat->Sum.Tilt / bpstep_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(7) << sqrt( bpstep_stat->NumOfSamples * bpstep_stat->Sum2.Tilt - bpstep_stat->Sum.Tilt * bpstep_stat->Sum.Tilt )  / bpstep_stat->NumOfSamples << " "; // sigma
        // Roll
        vout << right << fixed << setprecision(2) << setw(7) << bpstep_stat->Sum.Roll / bpstep_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(7) << sqrt( bpstep_stat->NumOfSamples * bpstep_stat->Sum2.Roll - bpstep_stat->Sum.Roll * bpstep_stat->Sum.Roll )  / bpstep_stat->NumOfSamples << " "; // sigma
        // Twist
        vout << right << fixed << setprecision(2) << setw(7) << bpstep_stat->Sum.Twist / bpstep_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(8) << sqrt( bpstep_stat->NumOfSamples * bpstep_stat->Sum2.Twist - bpstep_stat->Sum.Twist * bpstep_stat->Sum.Twist )  / bpstep_stat->NumOfSamples << " "; // sigma
        vout << endl;
        i++;
        it++;
    }
    vout << endl;
}

//------------------------------------------------------------------------------

void QNAStat::PrintBPHelParams(ofstream& vout)
{
    vout << "#  Base Pair Helical Parameters" << endl;
    vout << "# index ResIDA ResIDB ResIDC ResIDD  BPStep  Abundance <xdisp> s(xdisp) <ydisp> s(ydisp) <hrise> s(hrise)  <incl> s(incl)  <tip>   s(tip) <htwist> s(htwist)" << endl;
    vout << "#   1      2      3      4      5       6        7        8        9       10      11       12      13       14      15      16      17      18        19   " << endl;
    vout << "#------ ------ ------ ------ ------ -------- --------- ------- -------- ------- -------- ------- -------- ------- ------- ------- ------- -------- ---------" << endl;

    std::map<CNABPStepID,CNABPHelStatPtr>::iterator  it = BPHelStat.begin();
    std::map<CNABPStepID,CNABPHelStatPtr>::iterator  ie = BPHelStat.end();

    int i = 1;
    while( it != ie ){
        CNABPStepID  bpstep_id = it->first;
        CNABPHelStatPtr bphel_stat = it->second;
        // index
        vout << right << setw(7) << i << " ";
        // ResIDA ResIDB ResIDC ResIDD  BPStep
        vout << right << setw(6) << bpstep_id.ResIDA + 1 << " "
             << right << setw(6) << bpstep_id.ResIDB + 1 << " "
             << right << setw(6) << bpstep_id.ResIDC + 1 << " "
             << right << setw(6) << bpstep_id.ResIDD + 1 << " " << right << setw(8) << bpstep_id.Step << " ";
        // Abundance
        double abundance = (double)bphel_stat->NumOfSamples * 100.0 /  (double)NumOfSnapshots;
        vout << right << fixed << setprecision(2) << setw(9) << abundance  << " ";
        // Xdisp
        vout << right << fixed << setprecision(2) << setw(7) << bphel_stat->Sum.Xdisp / bphel_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(8) << sqrt( bphel_stat->NumOfSamples * bphel_stat->Sum2.Xdisp - bphel_stat->Sum.Xdisp * bphel_stat->Sum.Xdisp )  / bphel_stat->NumOfSamples << " "; // sigma
        // Ydisp
        vout << right << fixed << setprecision(2) << setw(7) << bphel_stat->Sum.Ydisp / bphel_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(8) << sqrt( bphel_stat->NumOfSamples * bphel_stat->Sum2.Ydisp - bphel_stat->Sum.Ydisp * bphel_stat->Sum.Ydisp )  / bphel_stat->NumOfSamples << " "; // sigma
        // Hrise
        vout << right << fixed << setprecision(2) << setw(7) << bphel_stat->Sum.Hrise / bphel_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(8) << sqrt( bphel_stat->NumOfSamples * bphel_stat->Sum2.Hrise - bphel_stat->Sum.Hrise * bphel_stat->Sum.Hrise )  / bphel_stat->NumOfSamples << " "; // sigma
        // Incl
        vout << right << fixed << setprecision(2) << setw(7) << bphel_stat->Sum.Incl / bphel_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(7) << sqrt( bphel_stat->NumOfSamples * bphel_stat->Sum2.Incl - bphel_stat->Sum.Incl * bphel_stat->Sum.Incl )  / bphel_stat->NumOfSamples << " "; // sigma
        // Tip
        vout << right << fixed << setprecision(2) << setw(7) << bphel_stat->Sum.Tip / bphel_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(7) << sqrt( bphel_stat->NumOfSamples * bphel_stat->Sum2.Tip - bphel_stat->Sum.Tip * bphel_stat->Sum.Tip )  / bphel_stat->NumOfSamples << " "; // sigma
        // Htwist
        vout << right << fixed << setprecision(2) << setw(8) << bphel_stat->Sum.Htwist / bphel_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(9) << sqrt( bphel_stat->NumOfSamples * bphel_stat->Sum2.Htwist - bphel_stat->Sum.Htwist * bphel_stat->Sum.Htwist )  / bphel_stat->NumOfSamples << " "; // sigma
        vout << endl;
        i++;
        it++;
    }
    vout << endl;
}

//------------------------------------------------------------------------------

void QNAStat::PrintPParams(ofstream& vout)
{
    vout << "#  P Parameters" << endl;
    vout << "#                                                     |  Mean projection of the two phosphorus atoms onto the x/y/z-axis of the dimer ‘middle frame’  |   Abundance of the duplex form    " << endl;
    vout << "# index ResIDA ResIDB ResIDC ResIDD  BPStep  Abundance|  <Xp>   s(Xp)    <Yp>   s(Yp)    <Zp>   s(Zp)   <XpH>   s(XpH)  <YpH>   s(YpH)  <ZpH>   s(ZpH)|A-form B-form A-like B-like TA-like" << endl;
    vout << "#   1      2      3      4      5       6        7    |   8       9       10      11      12     13      14      15       16      17      18      19  |  20     21     22     23      24  " << endl;
    vout << "#------ ------ ------ ------ ------ -------- --------- ------- ------- ------- ------- ------- ------- ------- ------- ------- ------- ------- ------- ------ ------ ------ ------ -------" << endl;

    std::map<CNABPStepID,CNAPStatPtr>::iterator  it = PStat.begin();
    std::map<CNABPStepID,CNAPStatPtr>::iterator  ie = PStat.end();

    int i = 1;
    while( it != ie ){
        CNABPStepID  bpstep_id = it->first;

        CNAPStatPtr bpstep_stat = it->second;

        vout << right << setw(7) << i << " ";
        // ResIDA ResIDB ResIDC ResIDD  BPStep
        vout << right << setw(6) << bpstep_id.ResIDA + 1 << " "
             << right << setw(6) << bpstep_id.ResIDB + 1 << " "
             << right << setw(6) << bpstep_id.ResIDC + 1 << " "
             << right << setw(6) << bpstep_id.ResIDD + 1 << " " << right << setw(8) << bpstep_id.Step << " ";
        // Abundance
        double abundance = (double)bpstep_stat->NumOfSamples * 100.0 /  (double)NumOfSnapshots;
        vout << right << fixed << setprecision(2) << setw(9) << abundance  << " ";
        // Xp
        vout << right << fixed << setprecision(2) << setw(7) << bpstep_stat->Sum.Xp / bpstep_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(7) << sqrt( bpstep_stat->NumOfSamples * bpstep_stat->Sum2.Xp - bpstep_stat->Sum.Xp * bpstep_stat->Sum.Xp )  / bpstep_stat->NumOfSamples << " "; // sigma
        // Yp
        vout << right << fixed << setprecision(2) << setw(7) << bpstep_stat->Sum.Yp / bpstep_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(7) << sqrt( bpstep_stat->NumOfSamples * bpstep_stat->Sum2.Yp - bpstep_stat->Sum.Yp * bpstep_stat->Sum.Yp )  / bpstep_stat->NumOfSamples << " "; // sigma
        // Zp
        vout << right << fixed << setprecision(2) << setw(7) << bpstep_stat->Sum.Zp / bpstep_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(7) << sqrt( bpstep_stat->NumOfSamples * bpstep_stat->Sum2.Zp - bpstep_stat->Sum.Zp * bpstep_stat->Sum.Zp )  / bpstep_stat->NumOfSamples << " "; // sigma
        // XpH
        vout << right << fixed << setprecision(2) << setw(7) << bpstep_stat->Sum.XpH / bpstep_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(7) << sqrt( bpstep_stat->NumOfSamples * bpstep_stat->Sum2.XpH - bpstep_stat->Sum.XpH * bpstep_stat->Sum.XpH )  / bpstep_stat->NumOfSamples << " "; // sigma
        // YpH
        vout << right << fixed << setprecision(2) << setw(7) << bpstep_stat->Sum.YpH / bpstep_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(7) << sqrt( bpstep_stat->NumOfSamples * bpstep_stat->Sum2.YpH - bpstep_stat->Sum.YpH * bpstep_stat->Sum.YpH )  / bpstep_stat->NumOfSamples << " "; // sigma
        // ZpH
        vout << right << fixed << setprecision(2) << setw(7) << bpstep_stat->Sum.ZpH / bpstep_stat->NumOfSamples << " "; // average
        vout << right << fixed << setprecision(2) << setw(7) << sqrt( bpstep_stat->NumOfSamples * bpstep_stat->Sum2.ZpH - bpstep_stat->Sum.ZpH * bpstep_stat->Sum.ZpH )  / bpstep_stat->NumOfSamples << " "; // sigma
        // A-form_Abun
        vout << right << fixed << setprecision(2) << setw(6) << bpstep_stat->NumOfAForm * 100.0 / bpstep_stat->NumOfSamples << " "; // abundance
        // B-form_Abun
        vout << right << fixed << setprecision(2) << setw(6) << bpstep_stat->NumOfBForm * 100.0 / bpstep_stat->NumOfSamples << " "; // abundance
        // A-like_Abun
        vout << right << fixed << setprecision(2) << setw(6) << bpstep_stat->NumOfAlikeForm * 100.0 / bpstep_stat->NumOfSamples << " "; // abundance
        // B-like_Abun
        vout << right << fixed << setprecision(2) << setw(6) << bpstep_stat->NumOfBlikeForm * 100.0 / bpstep_stat->NumOfSamples << " "; // abundance
        // TA-like_Abun
        vout << right << fixed << setprecision(2) << setw(7) << bpstep_stat->NumOfTAlikeForm * 100.0 / bpstep_stat->NumOfSamples << " "; // abundance
        vout << endl;
        i++;
        it++;
    }
    vout << endl;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QNAStat::RegisterBPData(Qx3DNA* p_data)
{
    if( p_data == NULL ) return;    // no valid input

// local bp parameters
    std::vector<CNABPPar>::iterator  it = p_data->BPPar.begin();
    std::vector<CNABPPar>::iterator  ie = p_data->BPPar.end();

    while( it != ie ){
        CNABPPar local_bp = *it;
        if( local_bp.Valid ){
            CNABPID bp_id(p_data->BPIDs[local_bp.ID]);
            bool flipped = false;
            if( FlipHelAxisEnabled ){
                flipped = bp_id.MakeCanonical();
            }
            if (BPStat.find(bp_id) == BPStat.end() ){
                // new data
                CNABPStatPtr data(new CNABPStat);
                BPStat[bp_id] = data;
            }
            BPIDs.insert(bp_id);
            CNABPStatPtr data = BPStat[bp_id];
            if( data ){
                data->RegisterData(local_bp,flipped);
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
    std::vector<CNABPStepPar>::iterator  it = p_data->BPStepPar.begin();
    std::vector<CNABPStepPar>::iterator  ie = p_data->BPStepPar.end();

    while( it != ie ){
        CNABPStepPar local_bpstep = *it;
        if( local_bpstep.Valid ){
            CNABPStepID bpstep_id(p_data->BPStepIDs[local_bpstep.ID]);
            bool flipped = false;
            if( FlipHelAxisEnabled ){
                flipped = bpstep_id.MakeCanonical();
            }
            if (BPStepStat.find(bpstep_id) == BPStepStat.end() ){
                // new data
                CNABPStepStatPtr data(new CNABPStepStat);
                BPStepStat[bpstep_id] = data;
            }
            BPStepIDs.insert(bpstep_id);
            CNABPStepStatPtr data = BPStepStat[bpstep_id];
            if( data ){
                data->RegisterData(local_bpstep,flipped);
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
    std::vector<CNABPHelPar>::iterator  it = p_data->BPHelPar.begin();
    std::vector<CNABPHelPar>::iterator  ie = p_data->BPHelPar.end();

    while( it != ie ){
        CNABPHelPar local_bphel = *it;
        if( local_bphel.Valid ){
            CNABPStepID bpstep_id(p_data->BPStepIDs[local_bphel.ID]);
            bool flipped = false;
            if( FlipHelAxisEnabled ){
                flipped = bpstep_id.MakeCanonical();
            }
            if (BPHelStat.find(bpstep_id) == BPHelStat.end() ){
                // new data
                CNABPHelStatPtr data(new CNABPHelStat);
                BPHelStat[bpstep_id] = data;
            }
            BPStepIDs.insert(bpstep_id);
            CNABPHelStatPtr data = BPHelStat[bpstep_id];
            if( data ){
                data->RegisterData(local_bphel,flipped);
            }
        }
        it++;
    }
}

//------------------------------------------------------------------------------

void QNAStat::RegisterPData(Qx3DNA* p_data)
{
    if( p_data == NULL ) return;    // no valid input

// P parameters
    std::vector<CNAPPar>::iterator  it = p_data->PPar.begin();
    std::vector<CNAPPar>::iterator  ie = p_data->PPar.end();

    while( it != ie ){
        CNAPPar local_p = *it;
        if( local_p.Valid ){
            CNABPStepID bpstep_id(p_data->BPStepIDs[local_p.ID]);
            bool flipped = false;
            if( FlipHelAxisEnabled ){
                flipped = bpstep_id.MakeCanonical();
            }
            if (PStat.find(bpstep_id) == PStat.end() ){
                // new data
                CNAPStatPtr data(new CNAPStat);
                PStat[bpstep_id] = data;
            }
            BPStepIDs.insert(bpstep_id);
            CNAPStatPtr data = PStat[bpstep_id];
            if( data ){
                data->RegisterData(local_p,flipped);
            }
        }
        it++;
    }
}

//------------------------------------------------------------------------------
