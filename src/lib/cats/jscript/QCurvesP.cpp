// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2016 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QCurvesP.hpp>
#include <TerminalStr.hpp>
#include <PeriodicTable.hpp>
#include <QTemporaryDir>
#include <AmberResidue.hpp>
#include <boost/format.hpp>

#include <QCurvesP.moc>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QCurvesP::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QCurvesP::New);
    QScriptValue metaObject = engine.newQMetaObject(&QCurvesP::staticMetaObject, ctor);
    engine.globalObject().setProperty("CurvesP", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QCurvesP::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    QCATsScriptable scriptable("CurvesP");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "CurvesP object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new CurvesP()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    QCurvesP* p_obj = new QCurvesP();
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QCurvesP::QCurvesP(void)
    : QCATsScriptable("CurvesP")
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QCurvesP::analyze(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool CurvesP::analyze(snapshot[,selection])" << endl;
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
    
    // create temporary directory
    QTemporaryDir tmp_dir;
    tmp_dir.setAutoRemove(false); // keep files in the case of failure
    if( ! tmp_dir.isValid() ){
        CSmallString error;
        error << "unable to create a working directory: " << tmp_dir.path();
        return( ThrowError("snapshot[,selection]","unable to run analysis") );
    }
    WorkDir = CFileName(tmp_dir.path());
	
    // write input data
    if( WriteInputData(p_qsnap,p_qsel) == false ){
        CSmallString error;
        error << "unable to write an input data into the working directory: " << tmp_dir.path();
        return( ThrowError("snapshot[,selection]",error) );
    }

    // start analysis
    if( RunAnalysis() == false ){
        CSmallString error;
        error << "unable to run analysis, temporary data were left in the working directory: " << tmp_dir.path();
        return( ThrowError("snapshot[,selection]",error) );
    }

    // parse output data
    if( ParseOutputData() == false ){
        CSmallString error;
        error << "unable to parse output, temporary data were left in the working directory: " << tmp_dir.path();
        return( ThrowError("snapshot[,selection]",error) );
    }
    
    // clean temporary directory
    tmp_dir.remove();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QCurvesP::getNumOfBasePairs(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int CurvesP::getNumOfBasePairs()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
     return((int)BPPar.size());
}

//------------------------------------------------------------------------------

QScriptValue QCurvesP::getBPIndex(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int CurvesP::getBPIndex(residA,residB)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("residA,residB",2);
    if( value.isError() ) return(value);
    int residA = -1;
    int residB = -1;
    value = GetArgAsInt("residA,residB","residA",1,residA);
    if( value.isError() ) return(value);
    value = GetArgAsInt("residA,residB","residB",2,residB);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( ResIDMap.find(residA) == ResIDMap.end() ){
        return( ThrowError("residA,residB","residA was not included in the analysis") );
    }
    if( ResIDMap.find(residB) == ResIDMap.end() ){
        return( ThrowError("residA,residB","residB was not included in the analysis") );
    }

    // remap to local indexing
    residA = ResIDMap[residA];
    residB = ResIDMap[residB];

    std::map<int,CNABPID>::iterator it = BPIDs.begin();
    std::map<int,CNABPID>::iterator ie = BPIDs.end();

    while( it != ie ){
        if( (it->second.ResIDA == residA) ||
            (it->second.ResIDB == residB) ){
            return( it->first );
        }
        it++;
    }

    return(-1);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

#define get(what,param) \
QScriptValue QCurvesP::get##what##param(void)\
{\
    QScriptValue value;\
\
    if( IsHelpRequested() ){\
        CTerminalStr sout;\
        sout << "usage: double CurvesP::get##what##param(index)" << endl;\
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
    if( (index < 0) || (index >= (int)what##Par.size()) ){\
        CSmallString error; \
        error << "index " << index << " is out-of-range <0;" << (int)what##Par.size()-1 << ">"; \
        return( ThrowError("index", error) );\
    }\
\
    double num = what##Par[index].param; \
    return(num);\
}

//------------------------------------------------------------------------------

QScriptValue QCurvesP::areBPParamsValid(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool CurvesP::areBPParamsValid(index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);

    if( (index < 0) || (index >= (int)BPPar.size()) ){
        CSmallString error;
        error << "index " << index << " is out-of-range <0;" << (int)BPPar.size()-1 << ">";
        return( ThrowError("index", error) );
    }

// execute ---------------------------------------
    bool rvalue = BPPar[index].Valid;
    return(rvalue);
}

get(BP,Shear)
get(BP,Stretch)
get(BP,Stagger)
get(BP,Buckle)
get(BP,Propeller)
get(BP,Opening)

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QCurvesP::ClearAll(void)
{
    // destroy all previous data
    ResIDMap.clear();
    BPIDs.clear();
    BPPar.clear();
    HelAxisPositions.clear();
}

//------------------------------------------------------------------------------

bool QCurvesP::WriteInputData(QSnapshot* p_qsnap,QSelection* p_qsel)
{
// create PDB file -------------------------------
    // open output file
    FILE* p_fout;
    CFileName fileName = WorkDir / "QCurvesP.pdb";   // / - is overloaded operator - it merges two strings by path delimiter (/)
    if( (p_fout = fopen(fileName,"w")) == NULL ) {
        CSmallString error;
        error << "unable to open output file " << fileName;
        error << " (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }

    if( p_qsel != NULL ){

        // create map and count residues
        CAmberResidue* p_last = NULL;
        int       rcount = 0;
        for(int i=0; i < p_qsel->Mask.GetNumberOfSelectedAtoms(); i++){
            CAmberAtom* p_atom = p_qsel->Mask.GetSelectedAtomCondensed(i);
            CAmberResidue* p_res = p_atom->GetResidue();
            if( p_last != p_res ){
                ResIDMap[p_res->GetIndex()] = rcount;
                rcount++;
                p_last = p_res;
            }
        }

        if( rcount > 9999 ){
            CSmallString error;
            error << "too many residues for analysis: " << rcount;
            ES_ERROR(error);
            return(false);
        }

        // write to output file
        WritePDB(p_qsnap->Restart.GetTopology(),&p_qsnap->Restart,&p_qsel->Mask,p_fout);
    } else {
        if( p_qsnap->Restart.GetTopology()->ResidueList.GetNumberOfResidues() > 9999 ){
            CSmallString error;
            error << "too many residues for analysis: " << p_qsnap->Restart.GetTopology()->ResidueList.GetNumberOfResidues();
            ES_ERROR(error);
            return(false);
        }

        // create map
        for(int i=0; i < p_qsnap->Restart.GetTopology()->ResidueList.GetNumberOfResidues(); i++ ){
            ResIDMap[i] = i;
        }

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

bool QCurvesP::RunAnalysis(void)
{
// run find_pair from 3dna -------------------------------
    // prepare command
    stringstream cmd1;
    cmd1 << boost::format("cd %s > /dev/null 2>&1 && "
                         "find_pair QCurvesP.pdb QCurvesP.fp > find_pair.stdout 2>&1")%WorkDir;
    int status1 = system( cmd1.str().c_str() );

    if ( status1 != 0 ){
        CSmallString error;
        error << "running 3DNA program 'find_pair' failed - " << strerror(errno);
        ES_ERROR(error);
        return(false);
    }

    if( ParseFindPairOutputData() == false ){
        return(false);
    }

// prepare input file for Cur+ ----------------------------
    ofstream ofs;

    CFileName fileName = WorkDir / "QCurvesP.inp";
    ofs.open( fileName );
    ofs << " &inp" << endl;
    ofs << " file=QCurvesP.pdb," << endl;
    ofs << " lis=QCurvesP," << endl;
    ofs << " fit=.t.," << endl;
    ofs << " lib=" << getenv("CURVES_HOME") << "/standard," << endl;
    ofs << " isym=1," << endl;
    ofs << " test=.t.," << endl;
    ofs << " &end" << endl;
    ofs << " 2 1 -1 0 0" << endl;
    for(size_t i=0; i < BPIDs.size(); i++){
        CNABPID bpid = BPIDs[i];
        ofs << " " << bpid.ResIDA+1;
    }
    ofs << endl;
    for(size_t i=0; i < BPIDs.size(); i++){
        CNABPID bpid = BPIDs[i];
        ofs << " " << bpid.ResIDB+1;
    }
    ofs << endl;

    if( ofs.fail() ) {
        CSmallString error;
        error << "unable to create Cur+ input file " << fileName;
        error << " (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }
    ofs.close();

// run Cur+ from curves+ -------------------------------
    stringstream cmd2;
    cmd2 << boost::format("cd %s > /dev/null 2>&1 && "
                         "Cur+ < QCurvesP.inp > QCurvesP.stdout 2>&1")%WorkDir;
    int status2 = system( cmd2.str().c_str() );

    if ( status2 != 0 ){
        CSmallString error;
        error << "running curves+ program 'Cur+' failed - " << strerror(errno);
        ES_ERROR(error);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool QCurvesP::ParseFindPairOutputData(void)
{
    ifstream ifs;

// parse BP indexes -----------------------------------
    // open file
    CFileName fileName = WorkDir / "QCurvesP.fp";
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
            if( ReadSectionBPIDs(ifs,BPIDs) == false ) return(false);
        }

        getline(ifs,lbuf);
    }

    ifs.close();
    return(true);
}

//------------------------------------------------------------------------------

bool QCurvesP::ParseOutputData(void)
{
    ifstream    ifs;
    CFileName   fileName;

// parse BP  -----------------------------------

    // open file
    fileName = WorkDir / "QCurvesP.lis";
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

        if( lbuf.find("Axis calculation ...") != string::npos ){
            if( ReadSectionHelPos(ifs) == false ) return(false);
        }

        getline(ifs,lbuf);
    }

    ifs.close();

    return(true);
}

//------------------------------------------------------------------------------

bool QCurvesP::ReadSectionBPIDs(std::ifstream& ifs,std::map<int,CNABPID>& bps)
{
    string lbuf;
    getline(ifs,lbuf);
    while( ifs ){
        if( lbuf.find("##### Base-pair criteria used:") != string::npos ){
            return(true); // end of BP
        }

        stringstream    str(lbuf);
        CNABPID         params;
        string          dummy;

        str >> params.ResIDA >> params.ResIDB;
        str >> dummy >> dummy;
        str >> params.ID;
        str >> dummy >> dummy;
        params.Name = dummy.substr(18,1) + "-" + dummy.substr(24,1);
        if( ! str ){
            CSmallString error;
            error << "unable to read base-pairs in: " << lbuf;
            ES_ERROR(error);
            return(false);
        }

        // FIXME? - should we use dedicated counter for ID?
        params.ID--;
        params.ResIDA--;
        params.ResIDB--;
        bps[params.ID] = params;

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read base-pairs";
    ES_ERROR(error);
    return(false);
}

//------------------------------------------------------------------------------

bool QCurvesP::ReadSectionHelPos(std::ifstream& ifs)
{
    string lbuf;
    getline(ifs,lbuf); // skip empty line
    getline(ifs,lbuf); // read data
    while( ifs ){

        if( lbuf.empty() ){
            return(true); // end of helical axis positions
        }

        stringstream    str(lbuf);
        CPoint          pos;
        string          tmp;

        str >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> pos.x >> pos.y >> pos.z;

        if( ! str ){
            CSmallString error;
            error << "unable to read  helical axis position parameters in: " << lbuf;
            ES_ERROR(error);
            return(false);
        }

        HelAxisPositions.push_back(pos);
        getline(ifs,lbuf);
    }

    return(false);
}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool QCurvesP::WritePDB(CAmberTopology* p_top,CAmberRestart* p_crd,CAmberMaskAtoms* p_mask,FILE* p_fout)
{
    //123456 78901 2 3456 7 8901 2 3456 7890 1234567 89012345 67890123456789012345678901234567890
    //ATOM     145    N     VAL  A   25       32.433   16.336   57.540  1.00 11.92      A1   N

    // init indexes for TER and chain
    p_top->InitMoleculeIndexes();

    // write header
    fprintf(p_fout,"REMARK File generated with CATS for 3DNA/Curves+ analysis\n");

    int last_mol_id = -1;
    int resid = 0;
    int atid = 0;
    char chain_id = 'A';
    double occ=1.0;
    double tfac=0.0;
    int seg_id = 1;
    char aname[5];
    char rname[5];

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

            // setup names
            memset(aname,0,5);
            memcpy(aname,p_atom->GetName(),4);

            memset(rname,0,5);
            memcpy(rname,p_atom->GetResidue()->GetName(),4);

            fprintf(p_fout,"ATOM  %5d %4s %4s%c%4d    %8.3f%8.3f%8.3f%6.2f%6.2f     P%02d%4s\n",
                    atid,aname,rname,
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

