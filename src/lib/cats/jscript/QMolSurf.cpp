// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2015 Viktor Illik, DOPSAT
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
#include <QMolSurf.hpp>
#include <QMolSurf.moc>
#include <TerminalStr.hpp>
#include <PeriodicTable.hpp>


using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QMolSurf::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QMolSurf::New);
    QScriptValue metaObject = engine.newQMetaObject(&QMolSurf::staticMetaObject, ctor);
    engine.globalObject().setProperty("MolSurf", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QMolSurf::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    QCATsScriptable scriptable("MolSurf");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "MolSurf object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new MolSurf()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    QMolSurf* p_obj = new QMolSurf();
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QMolSurf::QMolSurf(void)
    : QCATsScriptable("MolSurf")
{
    // set and create working dir in /tmp
    ostringstream str_pid;
    str_pid << getpid();
    string my_pid(str_pid.str());
    CFileName fn_pid = my_pid.c_str();

    WorkDir = "/tmp" / fn_pid;
    // FIXME
    // mkdir(WorkDir, S_IRWXU);
    mkdir(WorkDir);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QMolSurf::analyze(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool MolSurf::analyze(snapshot[,selection])" << endl;
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

/// get solvent accesible surface area
/// double getSASA()
/// double getSASA(index)
/// double getSASA(selections)
QScriptValue QMolSurf::getSASA(void)
{
    return(QScriptValue());
}

//------------------------------------------------------------------------------

/// get solvent excluded surface area
/// double getSESA()
/// double getSESA(index)
/// double getSESA(selections)
QScriptValue QMolSurf::getSESA(void)
{
    return(QScriptValue());
}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QMolSurf::ClearAll(void)
{
    // destroy all previous data
    SASA.clear();
    SESA.clear();

    // remove all old tmp files
//    remove ( WorkDir / "QMolSurf.pdb" );
//    remove ( WorkDir / "QMolSurf.out" );
//    remove ( WorkDir / "auxiliary.par" );
//    remove ( WorkDir / "bestpairs.pdb" );
//    remove ( WorkDir / "bp_helical.par" );
//    remove ( WorkDir / "bp_order.dat" );
//    remove ( WorkDir / "bp_step.par" );
}

//------------------------------------------------------------------------------

bool QMolSurf::WriteInputData(QSnapshot* p_qsnap,QSelection* p_qsel)
{
// create XYZR file -------------------------------
    // open output file
    FILE* p_fout;
    CFileName fileName = WorkDir / "QMolSurf.xyzr";   // / - is overloaded operator - it merges two strings by path delimiter (/)
    if( (p_fout = fopen(fileName,"w")) == NULL ) {
        CSmallString error;
        error << "unable to open output file " << fileName;
        error << " (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }

    if( p_qsel != NULL ){
        // write to output file
        WriteXYZR(p_qsnap->Restart.GetTopology(),&p_qsnap->Restart,&p_qsel->Mask,p_fout);
    } else {
        CAmberMaskAtoms fake_mask;
        fake_mask.AssignTopology(p_qsnap->Restart.GetTopology());
        fake_mask.SelectAllAtoms();
        // write to output file
        WriteXYZR(p_qsnap->Restart.GetTopology(),&p_qsnap->Restart,&fake_mask,p_fout);
    }

    // is everything OK?
    int ret = ferror(p_fout);

    // close output file
    fclose(p_fout);

    return(ret == 0);
}

//------------------------------------------------------------------------------

bool QMolSurf::RunAnalysis(void)
{
// run 3DNA program -------------------------------

    // create input file for analysis first & run analyze program to create output files after
    int status = system( "cd " / WorkDir + " > /dev/null 2>&1 && " +
                         "find_pair " + WorkDir / "QMolSurf.pdb QMolSurf.inp > /dev/null 2>&1 && " +
                         "analyze QMolSurf.inp > /dev/null 2>&1" );

    if ( status < 0 ){
        CSmallString error;
        error << "running 3DNA program failed - " << strerror(errno);
        ES_ERROR(error);
        return(false);
    }
    if ( status > 0 ){
	    // FIXME
        // int exitCode = WEXITSTATUS(status);
        int exitCode = status;
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

bool QMolSurf::ParseOutputData(void)
{
    ifstream ifs;
    // open file
    CFileName fileName = WorkDir / "QMolSurf.out";
    ifs.open( fileName );
    if( ifs.fail() ) {
        CSmallString error;
        error << "unable to open analysis output file " << fileName;
        error << " (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }

    string lbuf;

//    getline(ifs,lbuf);
//    while( !ifs.eof() ){
////      Local base-pair parameters
////      bp        Shear    Stretch   Stagger    Buckle  Propeller  Opening
//        if( lbuf.find("Local base-pair parameters") != string::npos ){
//            getline(ifs,lbuf); // skip heading
//            if( ReadSectionLocalBP(ifs) == false ) return(false);
//        }
////      Local base-pair step parameters
////      step       Shift     Slide      Rise      Tilt      Roll     Twist
//        if( lbuf.find("Local base-pair step parameters") != string::npos ){
//            getline(ifs,lbuf); // skip heading
//            if( ReadSectionLocalBPStep(ifs) == false ) return(false);
//        }
////      Local base-pair helical parameters
////      step       X-disp    Y-disp   h-Rise     Incl.       Tip   h-Twist
//        if( lbuf.find("Local base-pair helical parameters") != string::npos ){
//            getline(ifs,lbuf); // skip heading
//            if( ReadSectionLocalBPHel(ifs) == false ) return(false);
//        }
//        /// ....
//        /// ....
//        getline(ifs,lbuf);
//    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool QMolSurf::WriteXYZR(CAmberTopology* p_top,CAmberRestart* p_crd,CAmberMaskAtoms* p_mask,FILE* p_fout)
{

//    //123456 78901 2 3456 7 8901 2 3456 7890 1234567 89012345 67890123456789012345678901234567890
//    //ATOM     145    N     VAL  A   25       32.433   16.336   57.540  1.00 11.92      A1   N

//    // init indexes for TER and chain
//    p_top->InitMoleculeIndexes();

//    // write header
//    WritePDBRemark(p_fout,"File generated with CATS for 3DNA analysis");

//    int last_mol_id = -1;
//    int resid = 0;
//    int atid = 0;
//    char chain_id = 'A';
//    double occ=1.0;
//    double tfac=0.0;
//    int seg_id = 1;

//    for(int i=0; i < p_top->AtomList.GetNumberOfAtoms(); i++ ) {
//        CAmberAtom* p_atom = p_mask->GetSelectedAtom(i);
//        if( p_atom != NULL ) {
//            if( last_mol_id == -1 ) {
//                last_mol_id = p_atom->GetMoleculeIndex();
//            } else {
//                if( last_mol_id != p_atom->GetMoleculeIndex() ) {
//                    fprintf(p_fout,"TER\n");
//                    chain_id++;
//                    seg_id++;
//                    last_mol_id = p_atom->GetMoleculeIndex();
//                }
//            }
//            if( chain_id > 'Z' ){
//                chain_id = 'A';
//            }
//            if( strncmp(p_atom->GetResidue()->GetName(),"WAT ",4) == 0 ){
//                chain_id = 'W';
//            }
//            resid = p_atom->GetResidue()->GetIndex()+1;
//            atid =  i+1;
//            if( seg_id > 99 ){
//                seg_id = 1;
//            }
//            if( atid > 999999 ) {
//                atid = 1;
//            }
//            if( resid > 9999 ){
//                resid = 1;
//            }
//            fprintf(p_fout,"ATOM  %5d %4s %4s%c%4d    %8.3f%8.3f%8.3f%6.2f%6.2f     P%02d%4s\n",
//                    atid,GetPDBAtomName(p_atom,p_atom->GetResidue()),GetPDBResName(p_atom,p_atom->GetResidue()),
//                    chain_id,
//                    resid,
//                    p_crd->GetPosition(i).x,p_crd->GetPosition(i).y,p_crd->GetPosition(i).z,
//                    occ,tfac,seg_id,PeriodicTable.GetSymbol(p_atom->GuessZ()));
//        }
//    }


//    fprintf(p_fout,"TER\n");

    return(false);
}

//------------------------------------------------------------------------------

