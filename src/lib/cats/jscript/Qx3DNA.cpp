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
        sout << "usage: bool x3DNA::analyze(topology,snapshot[,selection])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("topology,snapshot[,selection]",2,3);
    if( value.isError() ) return(value);

    QTopology* p_qtop = NULL;
    value = GetArgAsObject<QTopology*>("topology,snapshot[,selection]","topology","Topology",1,p_qtop);
    if( value.isError() ) return(value);

    QSnapshot* p_qsnap = NULL;
    value = GetArgAsObject<QSnapshot*>("topology,snapshot[,selection]","snapshot","Snapshot",2,p_qsnap);
    if( value.isError() ) return(value);

    QSelection* p_qsel = NULL;
    if( GetArgumentCount() > 2 ){
        value = GetArgAsObject<QSelection*>("topology,snapshot,selection","selection","Selection",3,p_qsel);
        if( value.isError() ) return(value);
    }

// create PDB file -------------------------------
    // open output file
    FILE* p_fout;
    CSmallString fileName = "test.pdb";
    if( (p_fout = fopen(fileName,"w")) == NULL ) {
        CSmallString error;
        error << "unable to open output file " << fileName;
        error << " (" << strerror(errno) << ")";
        ES_ERROR(error);
        return( ThrowError("topology,snapshot[,selection]",error) );
    }

    // write to output file
    WritePDB(p_qtop,p_qsnap,p_qsel,p_fout);

    // close output file
    fclose(p_fout);

// do 3DNA analysis -------------------------------

    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



bool Qx3DNA::WritePDB(QTopology* p_qtop,QSnapshot* p_qsnap,QSelection* p_qsel,FILE* p_fout)
{

    //123456 78901 2 3456 7 8901 2 3456 7890 1234567 89012345 67890123456789012345678901234567890
    //ATOM     145    N     VAL  A   25       32.433   16.336   57.540  1.00 11.92      A1   N

    //CSmallTimeAndDate time;
    //time.GetActualTimeAndDate();

    // init indexes for TER and chain
    p_qtop->Topology.InitMoleculeIndexes();

    // write header
    WritePDBRemark(p_fout,"File generated with CATS for 3DNA analysis");
    //WritePDBRemark(p_fout,"=== Topology ===");
    //WritePDBRemark(p_fout,p_qtop->Topology.GetTitle());
    //WritePDBRemark(p_fout,"=== Coordinates ===");
    //WritePDBRemark(p_fout,Options.GetArgCrdName());
    //WritePDBRemark(p_fout,"=== Date ===");
    //WritePDBRemark(p_fout,time.GetSDateAndTime());
    //WritePDBRemark(p_fout,"=== Number of selected atoms ===");
    //CSmallString tmp;
    //tmp.IntToStr(Mask.GetNumberOfSelectedAtoms());
    //WritePDBRemark(p_fout,tmp);
    //WritePDBRemark(p_fout,"=== Mask ===");
    //WritePDBRemark(p_fout,Mask.GetMask());

    int last_mol_id = -1;
    int resid = 0;
    int atid = 0;
    char chain_id = 'A';
    double occ=1.0;
    double tfac=0.0;
    int seg_id = 1;

    if ( p_qsel == NULL ){
        for(int i=0; i < p_qtop->Topology.AtomList.GetNumberOfAtoms(); i++ ) {
            CAmberAtom* p_atom = p_qtop->Topology.AtomList.GetAtom(i);
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
                        p_qsnap->Restart.GetPosition(i).x,p_qsnap->Restart.GetPosition(i).y,p_qsnap->Restart.GetPosition(i).z,
                        occ,tfac,seg_id,PeriodicTable.GetSymbol(p_atom->GuessZ()));
            }
        }
    } else {
        for(int i=0; i < p_qtop->Topology.AtomList.GetNumberOfAtoms(); i++ ) {
            CAmberAtom* p_atom = p_qsel->Mask.GetSelectedAtom(i);
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
                        p_qsnap->Restart.GetPosition(i).x,p_qsnap->Restart.GetPosition(i).y,p_qsnap->Restart.GetPosition(i).z,
                        occ,tfac,seg_id,PeriodicTable.GetSymbol(p_atom->GuessZ()));
            }
        }
    }


    fprintf(p_fout,"TER\n");

    return(false);
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

    return(name);
}

//------------------------------------------------------------------------------

