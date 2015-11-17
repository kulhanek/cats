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
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue Qx3DNA::analyze(void)
{
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

get(LocalBP,Shear)
get(LocalBP,Stretch)
get(LocalBP,Stagger)
get(LocalBP,Buckle)
get(LocalBP,Propeller)
get(LocalBP,Opening)

get(LocalBPStep,Shift)
get(LocalBPStep,Slide)
get(LocalBPStep,Rise)
get(LocalBPStep,Tilt)
get(LocalBPStep,Roll)
get(LocalBPStep,Twist)

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
    int status = system( "cd " / WorkDir + " > /dev/null 2>&1 && " +
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
            error << "running 3DNA program failed -	\"command not found\" (possible problem with $PATH)";
        } else {
            error << "running 3DNA program failed - exit code: " << exitCode;
        }
        ES_ERROR(error);
        return(false);
    }

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

    return(true);
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
        if( lbuf.find("----") != string::npos ){
            getline(ifs,lbuf);
            continue; // no relevant data
        }
        stringstream    str(lbuf);
        CLocalBP  params;
        // bp        Shear    Stretch   Stagger    Buckle  Propeller  Opening
        str >> params.ID >> params.Name >> params.Shear >> params.Stretch >> params.Stagger >> params.Buckle >> params.Propeller >> params.Opening;
        if( ! str ){
            CSmallString error;
            error << "unable to read Local base-pair parameters in:\n" << lbuf;
            ES_ERROR(error);
            return(false);
        }
        LocalBP.push_back(params);

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read Local base-pair parameters section - no data";
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
        if( lbuf.find("----") != string::npos ){
            getline(ifs,lbuf);
            continue; // no relevant data
        }
        stringstream    str(lbuf);
        CLocalBPStep  params;
        // step       Shift     Slide      Rise      Tilt      Roll     Twist
        str >> params.ID >> params.Step >> params.Shift >> params.Slide >> params.Rise >> params.Tilt >> params.Roll >> params.Twist;
        if( ! str ){
            CSmallString error;
            error << "unable to read Local base-pair step parameters in:\n" << lbuf;
            ES_ERROR(error);
            return(false);
        }
        LocalBPStep.push_back(params);

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read Local base-pair step parameters section - no data";
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
        if( lbuf.find("----") != string::npos ){
            getline(ifs,lbuf);
            continue; // no relevant data
        }
        stringstream    str(lbuf);
        CLocalBPHel  params;
        // step       X-disp    Y-disp   h-Rise     Incl.       Tip   h-Twist
        str >> params.ID >> params.Step >> params.Xdisp >> params.Ydisp >> params.Hrise >> params.Incl >> params.Tip >> params.Htwist;
        if( ! str ){
            CSmallString error;
            error << "unable to read Local base-pair helical parameters in:\n" << lbuf;
            ES_ERROR(error);
            return(false);
        }
        LocalBPHel.push_back(params);

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read Local base-pair helical parameters section - no data";
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

