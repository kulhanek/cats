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
#include <errno.h>
#include <fstream>
#include <ErrorSystem.hpp>
#include <QScriptEngine>
#include <Qx3DNA.hpp>
#include <Qx3DNA.moc>
#include <TerminalStr.hpp>

#include <QTopology.hpp>
#include <QSnapshot.hpp>
#include <QSelection.hpp>

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
    WorkDir = "/tmp";   /// we can change later
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

/// UPRAVY
/// topologii prosim vem ze snapshotu
/// CAmberTopology* p_top = p_qsnap->Restart.GetTopology();

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
    // start analysis
    if( ParseOutputData() == false ){
        return( ThrowError("snapshot[,selection]","unable to parse aoutput") );
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

// getters

QScriptValue Qx3DNA::getLocalBPShear(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double x3DNA::getLocalBPShear(index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);

    if( (index < 0) || (index >= (int)LocalBPParams.size()) ){
        return( ThrowError("index", "index is out-of-range") );
    }

// execute code ----------------------------------
    double shear = LocalBPParams[index].Shear;
    return(shear);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void Qx3DNA::ClearAll(void)
{
/// UPRAVY
    // destroy all previous data
    LocalBPParams.clear();
}

//------------------------------------------------------------------------------

bool Qx3DNA::WriteInputData(QSnapshot* p_qsnap,QSelection* p_qsel)
{
// create PDB file -------------------------------
    // open output file
    FILE* p_fout;
    CFileName fileName = WorkDir / "test.pdb";   // / - is overloaded operator - it merges two strings by path delimiter (/)
    if( (p_fout = fopen(fileName,"w")) == NULL ) {
        CSmallString error;
        error << "unable to open output file " << fileName;
        error << " (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }

/// UPRAVY
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
/// UPRAVY
    // tady zavolej system() a proved analyzu
    // module add nedavej
    // osetreni pripadne chyby ....
    return(true);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ParseOutputData(void)
{
    ifstream ifs;
    // open file - nakonec to nebudeme kesovat do stringstream, budeme rovnou nacitat ze souboru

    if( !ifs ) {
        // report chyba
        return(false);
    }

    string lbuf;

    getline(ifs,lbuf);
    while( ifs ){
//      Local base-pair parameters
//      bp        Shear    Stretch   Stagger    Buckle  Propeller  Opening
        if( lbuf.find("Local base-pair parameters") != string::npos ){  /// nejake klicove slovo - zapati tabulky
            getline(ifs,lbuf); // skip heading
            if( ReadSectionXXX(ifs) == false ) return(false);
        } else
//      Local base-pair step parameters
//                step       Shift     Slide      Rise      Tilt      Roll     Twist
        if( lbuf.find("Local base-pair step parameters") != string::npos ){
            getline(ifs,lbuf); // skip heading
            if( ReadSectionYYY(ifs) == false ) return(false);
        }
        /// ....
        /// ....
        getline(ifs,lbuf);
    }

    return(true);
}

//------------------------------------------------------------------------------

// ReadSectionXXX - parsuje danou sekci

bool Qx3DNA::ReadSectionXXX(std::ifstream& ifs)
{
    string lbuf;
    getline(ifs,lbuf);
    while( ifs ){
        if( lbuf.find("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~") != string::npos ){
            return(true); // konec sekce
        }
        stringstream    str(lbuf);
        CLocalBPParams  params;
        // bp        Shear    Stretch   Stagger    Buckle  Propeller  Opening
        str >> params.ID >> params.Name >> params.Shear >> params.Stretch >> params.Stagger >> params.Buckle >> params.Propeller >> params.Opening;
        if( ! str ){
            // report error
            return(false);
        }
        LocalBPParams.push_back(params);

        getline(ifs,lbuf);
    }

    // report error

    return( false );
}

//------------------------------------------------------------------------------

bool ReadSectionYYY(std::ifstream& ifs)
{
    return( false );
}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

/// UPRAVY
/// predej pointery na CAmberTopology, CAmberRestart a CAmberMaskAtoms

bool Qx3DNA::WritePDB(CAmberTopology* p_top,CAmberRestart* p_crd,CAmberMaskAtoms* p_mask,FILE* p_fout)
{

//    //123456 78901 2 3456 7 8901 2 3456 7890 1234567 89012345 67890123456789012345678901234567890
//    //ATOM     145    N     VAL  A   25       32.433   16.336   57.540  1.00 11.92      A1   N

//    //CSmallTimeAndDate time;
//    //time.GetActualTimeAndDate();

//    // init indexes for TER and chain
//    p_qtop->Topology.InitMoleculeIndexes();

//    // write header
//    WritePDBRemark(p_fout,"File generated with CATS for 3DNA analysis");
//    //WritePDBRemark(p_fout,"=== Topology ===");
//    //WritePDBRemark(p_fout,p_qtop->Topology.GetTitle());
//    //WritePDBRemark(p_fout,"=== Coordinates ===");
//    //WritePDBRemark(p_fout,Options.GetArgCrdName());
//    //WritePDBRemark(p_fout,"=== Date ===");
//    //WritePDBRemark(p_fout,time.GetSDateAndTime());
//    //WritePDBRemark(p_fout,"=== Number of selected atoms ===");
//    //CSmallString tmp;
//    //tmp.IntToStr(Mask.GetNumberOfSelectedAtoms());
//    //WritePDBRemark(p_fout,tmp);
//    //WritePDBRemark(p_fout,"=== Mask ===");
//    //WritePDBRemark(p_fout,Mask.GetMask());

//    int last_mol_id = -1;
//    int resid = 0;
//    int atid = 0;
//    char chain_id = 'A';
//    double occ=1.0;
//    double tfac=0.0;
//    int seg_id = 1;

///// UPRAVY
/////    zrusit duplicitu kodu, ted mask nemuze byt NULL, viz fake_mask

//    if ( p_qsel == NULL ){
//        for(int i=0; i < p_qtop->Topology.AtomList.GetNumberOfAtoms(); i++ ) {
//            CAmberAtom* p_atom = p_qtop->Topology.AtomList.GetAtom(i);
//            if( p_atom != NULL ) {
//                if( last_mol_id == -1 ) {
//                    last_mol_id = p_atom->GetMoleculeIndex();
//                } else {
//                    if( last_mol_id != p_atom->GetMoleculeIndex() ) {
//                        fprintf(p_fout,"TER\n");
//                        chain_id++;
//                        seg_id++;
//                        last_mol_id = p_atom->GetMoleculeIndex();
//                    }
//                }
//                if( chain_id > 'Z' ){
//                    chain_id = 'A';
//                }
//                if( strncmp(p_atom->GetResidue()->GetName(),"WAT ",4) == 0 ){
//                    chain_id = 'W';
//                }
//                resid = p_atom->GetResidue()->GetIndex()+1;
//                atid =  i+1;
//                if( seg_id > 99 ){
//                    seg_id = 1;
//                }
//                if( atid > 999999 ) {
//                    atid = 1;
//                }
//                if( resid > 9999 ){
//                    resid = 1;
//                }
//                fprintf(p_fout,"ATOM  %5d %4s %4s%c%4d    %8.3f%8.3f%8.3f%6.2f%6.2f     P%02d%4s\n",
//                        atid,GetPDBAtomName(p_atom,p_atom->GetResidue()),GetPDBResName(p_atom,p_atom->GetResidue()),
//                        chain_id,
//                        resid,
//                        p_qsnap->Restart.GetPosition(i).x,p_qsnap->Restart.GetPosition(i).y,p_qsnap->Restart.GetPosition(i).z,
//                        occ,tfac,seg_id,PeriodicTable.GetSymbol(p_atom->GuessZ()));
//            }
//        }
//    } else {
//        for(int i=0; i < p_qtop->Topology.AtomList.GetNumberOfAtoms(); i++ ) {
//            CAmberAtom* p_atom = p_qsel->Mask.GetSelectedAtom(i);
//            if( p_atom != NULL ) {
//                if( last_mol_id == -1 ) {
//                    last_mol_id = p_atom->GetMoleculeIndex();
//                } else {
//                    if( last_mol_id != p_atom->GetMoleculeIndex() ) {
//                        fprintf(p_fout,"TER\n");
//                        chain_id++;
//                        seg_id++;
//                        last_mol_id = p_atom->GetMoleculeIndex();
//                    }
//                }
//                if( chain_id > 'Z' ){
//                    chain_id = 'A';
//                }
//                if( strncmp(p_atom->GetResidue()->GetName(),"WAT ",4) == 0 ){
//                    chain_id = 'W';
//                }
//                resid = p_atom->GetResidue()->GetIndex()+1;
//                atid =  i+1;
//                if( seg_id > 99 ){
//                    seg_id = 1;
//                }
//                if( atid > 999999 ) {
//                    atid = 1;
//                }
//                if( resid > 9999 ){
//                    resid = 1;
//                }
//                fprintf(p_fout,"ATOM  %5d %4s %4s%c%4d    %8.3f%8.3f%8.3f%6.2f%6.2f     P%02d%4s\n",
//                        atid,GetPDBAtomName(p_atom,p_atom->GetResidue()),GetPDBResName(p_atom,p_atom->GetResidue()),
//                        chain_id,
//                        resid,
//                        p_qsnap->Restart.GetPosition(i).x,p_qsnap->Restart.GetPosition(i).y,p_qsnap->Restart.GetPosition(i).z,
//                        occ,tfac,seg_id,PeriodicTable.GetSymbol(p_atom->GuessZ()));
//            }
//        }
//    }


//    fprintf(p_fout,"TER\n");

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

