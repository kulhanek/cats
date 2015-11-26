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

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <ErrorSystem.hpp>
#include <QScriptEngine>
#include <Qx3DNA.hpp>
#include <Qx3DNA.moc>
#include <TerminalStr.hpp>
#include <PeriodicTable.hpp>


using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CLocalBP::CLocalBP(void)
{
    Valid = false;
    ID = -1;
    Name = "nd";
    Shear = 0.0;
    Stretch = 0.0;
    Stagger = 0.0;
    Buckle = 0.0;
    Propeller = 0.0;
    Opening = 0.0;
}


CLocalBPStep::CLocalBPStep(void)
{
    Valid = false;
    ID = -1;
    Step = "nd";
    Shift = 0.0;
    Slide = 0.0;
    Rise = 0.0;
    Tilt = 0.0;
    Roll = 0.0;
    Twist = 0.0;
}

CLocalBPHel::CLocalBPHel(void)
{
    Valid = false;
    ID = -1;
    Step = "nd";
    Xdisp = 0.0;
    Ydisp = 0.0;
    Hrise = 0.0;
    Incl = 0.0;
    Tip = 0.0;
    Htwist = 0.0;
}

//------------------------------------------------------------------------------

CDNABasePair::CDNABasePair(void)
{
    ID = -1;
    Name = "nd";
    ResIDA = 0;
    ResIDB = 0;
}

//------------------------------------------------------------------------------

CDNABasePairStep::CDNABasePairStep(void)
{
    ID = -1;
    Step = "nd";
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void Qx3DNA::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(Qx3DNA::New);
    QScriptValue metaObject = engine.newQMetaObject(&Qx3DNA::staticMetaObject, ctor);
    engine.globalObject().setProperty("x3DNA", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    QCATsScriptable scriptable("x3DNA");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "x3DNA object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new x3DNA()" << endl;
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

Qx3DNA::Qx3DNA(void)
    : QCATsScriptable("x3DNA")
{
    // set and create working dir in /tmp
    ostringstream str_pid;
    str_pid << getpid();
    string my_pid(str_pid.str());
    CFileName fn_pid = my_pid.c_str();

    WorkDir = "/tmp" / fn_pid;
    mkdir(WorkDir, S_IRWXU);

    // default value
    AutoReferenceMode = true;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue Qx3DNA::setAutoReferenceMode(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNA::setAutoReferenceMode(set)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("set",1);
    if( value.isError() ) return(value);

    bool set;
    value = GetArgAsBool("set","set",1,set);
    if( value.isError() ) return(value);

// update value of AutoReferenceMode -------------
    AutoReferenceMode = set;

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::analyzeReference(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNA::analyzeReference(snapshot[,selection])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("snapshot[,selection]",1,2);
    if( value.isError() ) return(value);

    QSnapshot* p_qsnap = NULL;
    value = GetArgAsObject<QSnapshot*>("snapshot[,selection]","snapshot","Snapshot",1,p_qsnap);
    if( value.isError() ) return(value);

    QSelection* p_qsel = NULL;
    if( GetArgumentCount() > 1 ){
        value = GetArgAsObject<QSelection*>("snapshot,selection","selection","Selection",2,p_qsel);
        if( value.isError() ) return(value);
    }

// do analysis -----------------------------------
    // clear previous data
    ReferenceBasePairs.clear();
    ReferenceBasePairSteps.clear();
    AutoReferenceMode = false;

    // write input data
    if( WriteInputData(p_qsnap,p_qsel) == false ){
        return( ThrowError("snapshot[,selection]","unable to write input data") );
    }

    // start analysis
    if( RunAnalysis() == false ){
        return( ThrowError("snapshot[,selection]","unable to run analysis") );
    }

    // parse output data to set ReferenceBasePairs and ReferenceBasePairSteps
    if( ParseReferenceData() == false ){
        return( ThrowError("snapshot[,selection]","unable to parse output reference data") );
    }

    // perform standard analysis
    return(analyze()); // to avoid infinite recursion - AutoReferenceMode MUST be set to false in this method
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::analyze(void)
{
    // in autoreference mode - call analyzeReference
    if( AutoReferenceMode == true ){
        return(analyzeReference());
    }

    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNA::analyze(snapshot[,selection])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("snapshot[,selection]",1,2);
    if( value.isError() ) return(value);

    QSnapshot* p_qsnap = NULL;
    value = GetArgAsObject<QSnapshot*>("snapshot[,selection]","snapshot","Snapshot",1,p_qsnap);
    if( value.isError() ) return(value);

    QSelection* p_qsel = NULL;
    if( GetArgumentCount() > 1 ){
        value = GetArgAsObject<QSelection*>("snapshot,selection","selection","Selection",2,p_qsel);
        if( value.isError() ) return(value);
    }

// do 3DNA analysis -------------------------------
    // clear previous data
    ClearAll();

    // write input data
    if( WriteInputData(p_qsnap,p_qsel) == false ){
        return( ThrowError("snapshot[,selection]","unable to write input data") );
    }

    // start analysis
    if( RunAnalysis() == false ){
        return( ThrowError("snapshot[,selection]","unable to run analysis") );
    }

    // parse output data
    if( ParseOutputData() == false ){
        return( ThrowError("snapshot[,selection]","unable to parse output") );
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

// getters
QScriptValue Qx3DNA::getNumOfBasePairs(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int x3DNA::getNumOfBasePairs()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( ReferenceBasePairs.size() > 0 ){
        return((int)ReferenceBasePairs.size());
    } else {
        return((int)LocalBP.size());
    }
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::getNumOfSteps(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int x3DNA::getNumOfSteps()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( ReferenceBasePairSteps.size() > 0 ){
        return((int)ReferenceBasePairSteps.size());
    } else {
        return((int)LocalBPStep.size());
    }
}

//------------------------------------------------------------------------------

#define get(what,param) \
QScriptValue Qx3DNA::get##what##param(void)\
{\
    QScriptValue value;\
\
    if( IsHelpRequested() ){\
        CTerminalStr sout;\
        sout << "usage: double x3DNA::get##what##param(index)" << endl;\
        return(false);\
    }\
\
    value = CheckNumberOfArguments("index",1);\
    if( value.isError() ) return(value);\
\
    int index;\
    value = GetArgAsInt("index","index",1,index);\
    if( value.isError() ) return(value);\
\
    if( (index < 0) || (index >= (int)what.size()) ){\
        return( ThrowError("index", "index is out-of-range") );\
    }\
\
    double num = what[index].param; \
    return(num);\
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::areLocalBPParamsValid(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNA::areLocalBPParamsValid(index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);

    if( (index < 0) || (index >= (int)LocalBP.size()) ){
        return( ThrowError("index", "index is out-of-range") );
    }

// execute ---------------------------------------
    bool rvalue = LocalBP[index].Valid;
    return(rvalue);
}

get(LocalBP,Shear)
get(LocalBP,Stretch)
get(LocalBP,Stagger)
get(LocalBP,Buckle)
get(LocalBP,Propeller)
get(LocalBP,Opening)

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::areLocalBPStepParamsValid(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNA::areLocalBPStepParamsValid(index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);

    if( (index < 0) || (index >= (int)LocalBP.size()) ){
        return( ThrowError("index", "index is out-of-range") );
    }

// execute ---------------------------------------
    bool rvalue = LocalBPStep[index].Valid;
    return(rvalue);
}

get(LocalBPStep,Shift)
get(LocalBPStep,Slide)
get(LocalBPStep,Rise)
get(LocalBPStep,Tilt)
get(LocalBPStep,Roll)
get(LocalBPStep,Twist)

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::areLocalBPHelParamsValid(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNA::areLocalBPHelParamsValid(index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);

    if( (index < 0) || (index >= (int)LocalBP.size()) ){
        return( ThrowError("index", "index is out-of-range") );
    }

// execute ---------------------------------------
    bool rvalue = LocalBPHel[index].Valid;
    return(rvalue);
}

get(LocalBPHel,Xdisp)
get(LocalBPHel,Ydisp)
get(LocalBPHel,Hrise)
get(LocalBPHel,Incl)
get(LocalBPHel,Tip)
get(LocalBPHel,Htwist)



//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void Qx3DNA::ClearAll(void)
{
    // destroy all previous data
    LocalBP.clear();
    LocalBPStep.clear();
    LocalBPHel.clear();

    // remove all old tmp files
    remove ( WorkDir / "Qx3DNA.pdb" );
    remove ( WorkDir / "Qx3DNA.out" );
    remove ( WorkDir / "auxiliary.par" );
    remove ( WorkDir / "bestpairs.pdb" );
    remove ( WorkDir / "bp_helical.par" );
    remove ( WorkDir / "bp_order.dat" );
    remove ( WorkDir / "bp_step.par" );

    remove ( WorkDir / "cf_7methods.par" );
    remove ( WorkDir / "col_chains.scr" );
    remove ( WorkDir / "col_helices.scr" );
    remove ( WorkDir / "hel_regions.pdb" );
    remove ( WorkDir / "hstacking.pdb" );
    remove ( WorkDir / "ref_frames.dat" );
    remove ( WorkDir / "stacking.pdb" );

}

//------------------------------------------------------------------------------

bool Qx3DNA::WriteInputData(QSnapshot* p_qsnap,QSelection* p_qsel)
{
// create PDB file -------------------------------
    // open output file
    FILE* p_fout;
    CFileName fileName = WorkDir / "Qx3DNA.pdb";   // / - is overloaded operator - it merges two strings by path delimiter (/)
    if( (p_fout = fopen(fileName,"w")) == NULL ) {
        CSmallString error;
        error << "unable to open output file " << fileName;
        error << " (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }

    if( p_qsel != NULL ){
        // write to output file
        WritePDB(p_qsnap->Restart.GetTopology(),&p_qsnap->Restart,&p_qsel->Mask,p_fout);
    } else {
        CAmberMaskAtoms fake_mask;
        fake_mask.AssignTopology(p_qsnap->Restart.GetTopology());
        fake_mask.SelectAllAtoms();
        // write to output file
        WritePDB(p_qsnap->Restart.GetTopology(),&p_qsnap->Restart,&fake_mask,p_fout);
    }

    // is everything OK?
    int ret = ferror(p_fout);

    // close output file
    fclose(p_fout);

    return(ret == 0);
}

//------------------------------------------------------------------------------

bool Qx3DNA::RunAnalysis(void)
{
// run 3DNA program -------------------------------
    // create input file for analysis first & run analyze program to create output files after
    int status = system( "cd " + WorkDir + " > /dev/null 2>&1 && " +
                         "find_pair " + WorkDir / "Qx3DNA.pdb Qx3DNA.inp > /dev/null 2>&1 && " +
                         "analyze Qx3DNA.inp > /dev/null 2>&1" );

    if ( status < 0 ){
        CSmallString error;
        error << "running 3DNA program failed - " << strerror(errno);
        ES_ERROR(error);
        return(false);
    }
    if ( status > 0 ){
        int exitCode = WEXITSTATUS(status);
        CSmallString error;
        if ( exitCode == 126 ){
            error << "running 3DNA program failed - command invoked cannot execute (permission problem or command is not an executable)";
        } else if ( exitCode == 127 ){
            error << "running 3DNA program failed - \"command not found\" (possible problem with $PATH)";
        } else {
            error << "running 3DNA program failed - exit code (" << exitCode << ")";
        }
        ES_ERROR(error);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ParseReferenceData(void)
{
    ifstream ifs;

// parse BP  -----------------------------------
    // open file
    CFileName fileName = WorkDir / "Qx3DNA.inp";
    ifs.open( fileName );
    if( ifs.fail() ) {
        CSmallString error;
        error << "unable to open analysis output file " << fileName;
        error << " (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }

    string lbuf;

    getline(ifs,lbuf);
    while( !ifs.eof() ){
        if( lbuf.find("explicit bp numbering/hetero atoms") != string::npos ){
            if( ReadSectionReferenceBP(ifs) == false ) return(false);
        }

        getline(ifs,lbuf);
    }

    ifs.close();

// parse BP Step -------------------------------
    fileName = WorkDir / "Qx3DNA.out";
    ifs.open( fileName );
    if( ifs.fail() ) {
        CSmallString error;
        error << "unable to open analysis output file " << fileName;
        error << " (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }

    getline(ifs,lbuf);
    while( !ifs.eof() ){
        if( lbuf.find("step      i1-i2        i1-j2        j1-i2        j1-j2        sum") != string::npos ){
            if( ReadSectionReferenceBPStep(ifs) == false ) return(false);
        }

        getline(ifs,lbuf);
    }

    ifs.close();

    return(true);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ParseOutputData(void)
{
    ifstream ifs;
    // open file
    CFileName fileName = WorkDir / "Qx3DNA.out";
    ifs.open( fileName );
    if( ifs.fail() ) {
        CSmallString error;
        error << "unable to open analysis output file " << fileName;
        error << " (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }

    string lbuf;

    getline(ifs,lbuf);
    while( !ifs.eof() ){
//      Local base-pair parameters
//      bp        Shear    Stretch   Stagger    Buckle  Propeller  Opening
        if( lbuf.find("Local base-pair parameters") != string::npos ){
            getline(ifs,lbuf); // skip heading
            if( ReadSectionLocalBP(ifs) == false ) return(false);
        }
//      Local base-pair step parameters
//      step       Shift     Slide      Rise      Tilt      Roll     Twist
        if( lbuf.find("Local base-pair step parameters") != string::npos ){
            getline(ifs,lbuf); // skip heading
            if( ReadSectionLocalBPStep(ifs) == false ) return(false);
        }
//      Local base-pair helical parameters
//      step       X-disp    Y-disp   h-Rise     Incl.       Tip   h-Twist
        if( lbuf.find("Local base-pair helical parameters") != string::npos ){
            getline(ifs,lbuf); // skip heading
            if( ReadSectionLocalBPHel(ifs) == false ) return(false);
        }
        /// ....
        /// ....
        getline(ifs,lbuf);
    }

// cross-check number of BP and BP Pairs (steps)
    if( ReferenceBasePairs.size() > 0 ){
        if( ReferenceBasePairs.size() != LocalBP.size() ){
            CSmallString error;
            error << "number of base pairs is not the same as in reference structure";
            ES_ERROR(error);
            return(false);
        }
        if( ReferenceBasePairSteps.size() != LocalBPStep.size() ){
            CSmallString error;
            error << "number of base pair steps is not the same as in reference structure";
            ES_ERROR(error);
            return(false);
        }
        if( ReferenceBasePairSteps.size() != LocalBPHel.size() ){
            CSmallString error;
            error << "number of base pair steps is not the same as in reference structure";
            ES_ERROR(error);
            return(false);
        }
    }

    ifs.close();

    return(true);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ReadSectionReferenceBP(std::ifstream& ifs)
{
    string lbuf;
    getline(ifs,lbuf);
    while( ifs ){
        if( lbuf.find("##### Base-pair criteria used:") != string::npos ){
            return(true); // end of ReferenceBP
        }

        stringstream    str(lbuf);
        CDNABasePair    params;
        string          dummy;

        str >> params.ResIDA >> params.ResIDB;
        str >> dummy >> dummy;
        str >> params.ID;
        str >> dummy >> dummy;
        params.Name = dummy.substr(18,1) + "-" + dummy.substr(24,1);
        if( ! str ){
            CSmallString error;
            error << "unable to read Reference base-pairs in:\n" << lbuf;
            ES_ERROR(error);
            return(false);
        }

        ReferenceBasePairs[params.ID] = params;

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read Reference base-pairs";
    ES_ERROR(error);
    return(false);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ReadSectionReferenceBPStep(std::ifstream& ifs)
{
    string lbuf;
    getline(ifs,lbuf);
    while( ifs ){
        if( lbuf.find("****************************************************************************") != string::npos ){
            return(true); // end of ReferenceBPStep
        }

        stringstream        str(lbuf);
        CDNABasePairStep    params;
        string              dummy;

        str >> params.ID >> params.Step;

        if( ! str ){
            CSmallString error;
            error << "unable to read Reference base-pair step in:\n" << lbuf;
            ES_ERROR(error);
            return(false);
        }

        ReferenceBasePairSteps[params.ID] = params;

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read Reference base-pair step";
    ES_ERROR(error);
    return(false);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ReadSectionLocalBP(std::ifstream& ifs)
{
    string lbuf;
    getline(ifs,lbuf);
    while( ifs ){
        if( lbuf.find("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~") != string::npos ){
            return(true); // end of LocalBP
        }

        CLocalBP        params;
        stringstream    str(lbuf);
        // bp
        str >> params.ID >> params.Name;        // this should not fail - but you can test success as well, code is commented below
//        if( ! str ){
//            CSmallString error;
//            error << "unable to read Local base-pair parameters in:\n" << lbuf;
//            ES_ERROR(error);
//            return(false);
//        }
        if( ! ( lbuf.find("----") != string::npos ) ){
            // read the rest of parameters
            //        Shear    Stretch   Stagger    Buckle  Propeller  Opening
            str >> params.Shear >> params.Stretch >> params.Stagger >> params.Buckle >> params.Propeller >> params.Opening;
            if( ! str ){
                CSmallString error;
                error << "unable to read Local base-pair parameters in:\n" << lbuf;
                ES_ERROR(error);
                return(false);
            }
            params.Valid = true;  // data are valid
        }

        // check if the current structure has the same pairing as reference structure
        if( ReferenceBasePairs.size() > 0 ){  // reference pairing is available -> perform test
            if( ReferenceBasePairs[params.ID].Name != params.Name ){
                CSmallString error;
                error << "base pair (" << params.Name << ") with number (" << params.ID << ") is not the same as in reference structure";
                ES_ERROR(error);
                return(false);
            }
        }

        LocalBP.push_back(params);

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read Local base-pair parameters section";
    ES_ERROR(error);
    return(false);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ReadSectionLocalBPStep(std::ifstream& ifs)
{
    string lbuf;
    getline(ifs,lbuf);
    while( ifs ){
        if( lbuf.find("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~") != string::npos ){
            return(true); // end of LocalBPStep
        }

        stringstream    str(lbuf);
        CLocalBPStep    params;
        // step
        str >> params.ID >> params.Step;        // this should not fail - but you can test success as well, code is commented below
        //        if( ! str ){
        //            CSmallString error;
        //            error << "unable to read Local base-pair parameters in:\n" << lbuf;
        //            ES_ERROR(error);
        //            return(false);
        //        }
        if( ! ( lbuf.find("----") != string::npos ) ){
            // read the rest of parameters
            // step       Shift     Slide      Rise      Tilt      Roll     Twist
            str >> params.Shift >> params.Slide >> params.Rise >> params.Tilt >> params.Roll >> params.Twist;
            if( ! str ){
                CSmallString error;
                error << "unable to read Local base-pair step parameters in:\n" << lbuf;
                ES_ERROR(error);
                return(false);
            }
            params.Valid = true;  // data are valid
        }

        // check if the current structure has the same pairing as reference structure
        if( ReferenceBasePairSteps.size() > 0 ){  // reference pairing is available -> perform test
            if( ReferenceBasePairSteps[params.ID].Step != params.Step ){
                CSmallString error;
                error << "base pair step (" << params.Step << ") with number (" << params.ID << ") is not the same as in reference structure";
                ES_ERROR(error);
                return(false);
            }
        }

        LocalBPStep.push_back(params);

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read Local base-pair step parameters section";
    ES_ERROR(error);
    return(false);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ReadSectionLocalBPHel(std::ifstream& ifs)
{
    string lbuf;
    getline(ifs,lbuf);
    while( ifs ){
        if( lbuf.find("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~") != string::npos ){
            return(true); // end of LocalBPHel
        }

        stringstream    str(lbuf);
        CLocalBPHel     params;
        // step
        str >> params.ID >> params.Step;        // this should not fail - but you can test success as well, code is commented below
        //        if( ! str ){
        //            CSmallString error;
        //            error << "unable to read Local base-pair parameters in:\n" << lbuf;
        //            ES_ERROR(error);
        //            return(false);
        //        }
        if( ! ( lbuf.find("----") != string::npos ) ){
            // step       X-disp    Y-disp   h-Rise     Incl.       Tip   h-Twist
            str >> params.Xdisp >> params.Ydisp >> params.Hrise >> params.Incl >> params.Tip >> params.Htwist;
            if( ! str ){
                CSmallString error;
                error << "unable to read Local base-pair helical parameters in:\n" << lbuf;
                ES_ERROR(error);
                return(false);
            }
            params.Valid = true;  // data are valid
        }

        // check if the current structure has the same pairing as reference structure
        if( ReferenceBasePairSteps.size() > 0 ){  // reference pairing is available -> perform test
            if( ReferenceBasePairSteps[params.ID].Step != params.Step ){
                CSmallString error;
                error << "base pair step (" << params.Step << ") with number (" << params.ID << ") is not the same as in reference structure";
                ES_ERROR(error);
                return(false);
            }
        }

        LocalBPHel.push_back(params);

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read Local base-pair helical parameters section";
    ES_ERROR(error);
    return(false);
}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


bool Qx3DNA::WritePDB(CAmberTopology* p_top,CAmberRestart* p_crd,CAmberMaskAtoms* p_mask,FILE* p_fout)
{

    //123456 78901 2 3456 7 8901 2 3456 7890 1234567 89012345 67890123456789012345678901234567890
    //ATOM     145    N     VAL  A   25       32.433   16.336   57.540  1.00 11.92      A1   N

    // init indexes for TER and chain
    p_top->InitMoleculeIndexes();

    // write header
    WritePDBRemark(p_fout,"File generated with CATS for 3DNA analysis");

    int last_mol_id = -1;
    int resid = 0;
    int atid = 0;
    char chain_id = 'A';
    double occ=1.0;
    double tfac=0.0;
    int seg_id = 1;

    for(int i=0; i < p_top->AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = p_mask->GetSelectedAtom(i);
        if( p_atom != NULL ) {
            if( last_mol_id == -1 ) {
                last_mol_id = p_atom->GetMoleculeIndex();
            } else {
                if( last_mol_id != p_atom->GetMoleculeIndex() ) {
                    fprintf(p_fout,"TER\n");
                    chain_id++;
                    seg_id++;
                    last_mol_id = p_atom->GetMoleculeIndex();
                }
            }
            if( chain_id > 'Z' ){
                chain_id = 'A';
            }
            if( strncmp(p_atom->GetResidue()->GetName(),"WAT ",4) == 0 ){
                chain_id = 'W';
            }
            resid = p_atom->GetResidue()->GetIndex()+1;
            atid =  i+1;
            if( seg_id > 99 ){
                seg_id = 1;
            }
            if( atid > 999999 ) {
                atid = 1;
            }
            if( resid > 9999 ){
                resid = 1;
            }
            fprintf(p_fout,"ATOM  %5d %4s %4s%c%4d    %8.3f%8.3f%8.3f%6.2f%6.2f     P%02d%4s\n",
                    atid,GetPDBAtomName(p_atom,p_atom->GetResidue()),GetPDBResName(p_atom,p_atom->GetResidue()),
                    chain_id,
                    resid,
                    p_crd->GetPosition(i).x,p_crd->GetPosition(i).y,p_crd->GetPosition(i).z,
                    occ,tfac,seg_id,PeriodicTable.GetSymbol(p_atom->GuessZ()));
        }
    }


    fprintf(p_fout,"TER\n");

    return(true);
}

//------------------------------------------------------------------------------

bool Qx3DNA::WritePDBRemark(FILE* p_file,const CSmallString& string)
{
    int end = string.GetLength();
    int start = 0;

    while( start < end ) {
        CSmallString substr;
        int len = 73;
        if( end - start < 73 ) len = end - start;
        substr = string.GetSubString(start,len);
        fprintf(p_file,"REMARK %s\n",(const char*)substr);
        start += len;
    }

    return(true);
}

//------------------------------------------------------------------------------

const char* Qx3DNA::GetPDBResName(CAmberAtom* p_atom,CAmberResidue* p_res)
{
    static char name[5];

    // clear name
    memset(name,0,5);

    // no change
    memcpy(name,p_res->GetName(),4);
    return(name);
}

//------------------------------------------------------------------------------

const char* Qx3DNA::GetPDBAtomName(CAmberAtom* p_atom,CAmberResidue* p_res)
{
    static char name[5];

    // clear name
    memset(name,0,5);

    // direct use
    memcpy(name,p_atom->GetName(),4);
    return(name);
}

//------------------------------------------------------------------------------

