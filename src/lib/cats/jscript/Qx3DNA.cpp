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
#include <TerminalStr.hpp>
#include <PeriodicTable.hpp>
#include <QTemporaryDir>
#include <AmberResidue.hpp>
#include <boost/format.hpp>

#include <Qx3DNA.moc>

//------------------------------------------------------------------------------

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
    // default value
    AutoReferenceMode = false;
    ParameterType = E3DP_LOCAL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue Qx3DNA::setAutoReferenceMode(const QScriptValue& dummy)
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

    bool set = false;
    value = GetArgAsBool("set","set",1,set);
    if( value.isError() ) return(value);

// update value of AutoReferenceMode -------------
    AutoReferenceMode = set;

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::isAutoReferenceModeSet(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNA::isAutoReferenceModeSet()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// update value of AutoReferenceMode -------------
    return( AutoReferenceMode );
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::setParameterType(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNA::setParameterType(type)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("type",1);
    if( value.isError() ) return(value);

    QString stype;
    value = GetArgAsString("type","type",1,stype);
    if( value.isError() ) return(value);

// update value of AutoReferenceMode -------------
    if( stype == "local" ){
        ParameterType = E3DP_LOCAL;
    } else if ( stype == "simple" ){
        ParameterType = E3DP_SIMPLE;
    } else {
        CSmallString error;
        error << "unrecognized parameter type: " << CSmallString(stype);
        return( ThrowError("type",error) );
    }

    return(true);
}

//------------------------------------------------------------------------------

QString Qx3DNA::GetParameterTypeString(void) const
{
    switch(ParameterType){
        case E3DP_LOCAL:
            return("local");
        case E3DP_SIMPLE:
            return("simple");
        default:
            return("unknown");
    }
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::getParameterType(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string x3DNA::getParameterType()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments(0);
    if( value.isError() ) return(value);

// update value of AutoReferenceMode -------------
    return( GetParameterTypeString() );
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
    ClearAll();
    AutoReferenceMode = false;
    
    // create temporary directory
    QTemporaryDir tmp_dir;
    tmp_dir.setAutoRemove(false); // keep files in the case of failure
    if( ! tmp_dir.isValid() ){
        CSmallString error;
        error << "unable to create a working directory: " << tmp_dir.path();
        return( ThrowError("snapshot[,selection]",error) );
    }
    WorkDir = CFileName(tmp_dir.path());
	
    // write input data
    if( WriteInputData(p_qsnap,p_qsel) == false ){
        CSmallString error;
        error << "unable to write a reference input data into the working directory: " << tmp_dir.path();
        return( ThrowError("snapshot[,selection]",error) );
    }

    // start analysis
    if( RunAnalysis() == false ){
        CSmallString error;
        error << "unable to run reference analysis, temporary data were left in the working directory: " << tmp_dir.path();
        return( ThrowError("snapshot[,selection]",error) );
    }

    // parse output data
    if( ParseReferenceData() == false ){
        CSmallString error;
        error << "unable to parse reference output, temporary data were left in the working directory: " << tmp_dir.path();
        return( ThrowError("snapshot[,selection]",error) );
    }

    // clean temporary directory
    tmp_dir.remove();    
    
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
    if( ReferenceBPIDs.size() > 0 ){
        return((int)ReferenceBPIDs.size());
    } else {
        return((int)BPPar.size());
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
    if( ReferenceBPStepIDs.size() > 0 ){
        return((int)ReferenceBPStepIDs.size());
    } else {
        return((int)BPStepPar.size());
    }
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::getBPIndex(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int x3DNA::getBPIndex(residA,residB)" << endl;
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
        if( (it->second.ResIDA == residA) &&
            (it->second.ResIDB == residB) ){
            return( it->first );
        }
        it++;
    }

    return(-1);
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::getBPStepIndex(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int x3DNA::getBPStepIndex(residA,residB,residC,residD)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("residA,residB,residC,residD",4);
    if( value.isError() ) return(value);
    int residA = -1;
    int residB = -1;
    int residC = -1;
    int residD = -1;
    value = GetArgAsInt("residA,residB,residC,residD","residA",1,residA);
    if( value.isError() ) return(value);
    value = GetArgAsInt("residA,residB,residC,residD","residB",2,residB);
    if( value.isError() ) return(value);
    value = GetArgAsInt("residA,residB,residC,residD","residC",3,residB);
    if( value.isError() ) return(value);
    value = GetArgAsInt("residA,residB,residC,residD","residD",4,residB);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( ResIDMap.find(residA) == ResIDMap.end() ){
        return( ThrowError("residA,residB,residC,residD","residA was not included in the analysis") );
    }
    if( ResIDMap.find(residB) == ResIDMap.end() ){
        return( ThrowError("residA,residB,residC,residD","residB was not included in the analysis") );
    }
    if( ResIDMap.find(residC) == ResIDMap.end() ){
        return( ThrowError("residA,residB,residC,residD","residC was not included in the analysis") );
    }
    if( ResIDMap.find(residD) == ResIDMap.end() ){
        return( ThrowError("residA,residB,residC,residD","residD was not included in the analysis") );
    }

    // remap to local indexing
    residA = ResIDMap[residA];
    residB = ResIDMap[residB];
    residC = ResIDMap[residC];
    residD = ResIDMap[residD];

    std::map<int,CNABPStepID>::iterator it = BPStepIDs.begin();
    std::map<int,CNABPStepID>::iterator ie = BPStepIDs.end();

    while( it != ie ){
        if( (it->second.ResIDA == residA) &&
            (it->second.ResIDB == residB) &&
            (it->second.ResIDC == residC) &&
            (it->second.ResIDD == residD) ){
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
    if( (index < 0) || (index >= (int)what##Par.size()) ){\
        CSmallString error; \
        error << "index " << index << " is out-of-range <0;" << (int)what##Par.size() << ">"; \
        return( ThrowError("index", error) );\
    }\
\
    double num = what##Par[index].param; \
    return(num);\
}

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::areBPParamsValid(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNA::areBPParamsValid(index)" << endl;
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
        error << "index " << index << " is out-of-range <0;" << (int)BPPar.size() << ">";
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

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::areBPStepParamsValid(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNA::areBPStepParamsValid(index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);

    if( (index < 0) || (index >= (int)BPStepPar.size()) ){
        CSmallString error;
        error << "index " << index << " is out-of-range <0;" << (int)BPStepPar.size() << ">";
        return( ThrowError("index", error) );
    }

// execute ---------------------------------------
    bool rvalue = BPStepPar[index].Valid;
    return(rvalue);
}

get(BPStep,Shift)
get(BPStep,Slide)
get(BPStep,Rise)
get(BPStep,Tilt)
get(BPStep,Roll)
get(BPStep,Twist)

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::areBPHelParamsValid(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNA::areBPHelParamsValid(index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);

    if( (index < 0) || (index >= (int)BPHelPar.size()) ){
        CSmallString error;
        error << "index " << index << " is out-of-range <0;" << (int)BPHelPar.size() << ">";
        return( ThrowError("index", error) );
    }

// execute ---------------------------------------
    bool rvalue = BPHelPar[index].Valid;
    return(rvalue);
}

get(BPHel,Xdisp)
get(BPHel,Ydisp)
get(BPHel,Hrise)
get(BPHel,Incl)
get(BPHel,Tip)
get(BPHel,Htwist)

//------------------------------------------------------------------------------

QScriptValue Qx3DNA::arePParamsValid(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool x3DNA::arePParamsValid(index)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);

    if( (index < 0) || (index >= (int)PPar.size()) ){
        CSmallString error;
        error << "index " << index << " is out-of-range <0;" << (int)PPar.size() << ">";
        return( ThrowError("index", error) );
    }

// execute ---------------------------------------
    bool rvalue = PPar[index].Valid;
    return(rvalue);
}

get(P,Xp)
get(P,Yp)
get(P,Zp)
get(P,XpH)
get(P,YpH)
get(P,ZpH)

QScriptValue Qx3DNA::getPForm(void)
{
    QScriptValue value;

    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int x3DNA::getPForm(index)" << endl;
        return(false);
    }

    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    int index;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);

    if( (index < 0) || (index >= (int)PPar.size()) ){
        CSmallString error;
        error << "index " << index << " is out-of-range <0;" << (int)PPar.size() << ">";
        return( ThrowError("index", error) );
    }

    int num = PPar[index].Form;
    return(num);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void Qx3DNA::ClearAll(void)
{
    // destroy all previous data
    ResIDMap.clear();
    BPIDs.clear();
    BPStepIDs.clear();
    BPPar.clear();
    BPStepPar.clear();
    BPHelPar.clear();
    PPar.clear();
    BPOrigins.clear();
    HelAxisPositions.clear();

    if ( AutoReferenceMode == true ){
        // destroy reference data only in the case a new reference structure is going to be analyzed
        ReferenceBPIDs.clear();
        ReferenceBPStepIDs.clear();
    }

    // data in the temporary directory are removed by QTemporaryDir
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

bool Qx3DNA::RunAnalysis(void)
{
// run 3DNA program -------------------------------
    // prepare command
    stringstream cmd;
    cmd << boost::format("cd %s > /dev/null 2>&1 && "
                         "find_pair Qx3DNA.pdb Qx3DNA.inp > find_pair.stdout 2>&1 && "
                         "analyze Qx3DNA.inp > analyze.stdout 2>&1")%WorkDir;
    int status = system( cmd.str().c_str() );

    if ( status != 0 ){
        CSmallString error;
        error << "running 3DNA program failed - " << strerror(errno);
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
            if( ReadSectionBPIDs(ifs,ReferenceBPIDs) == false ) return(false);
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
            if( ReadSectionBPStepIDs(ifs,ReferenceBPIDs,ReferenceBPStepIDs) == false ) return(false);
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

// parse BP indexes -----------------------------------
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
            if( ReadSectionBPIDs(ifs,BPIDs) == false ) return(false);
        }

        getline(ifs,lbuf);
    }

    ifs.close();

// parse the other data -------------------------------
    // open file
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

        if( lbuf.find("bp        Ox        Oy        Oz        Nx        Ny        Nz") != string::npos ){
            if( ReadSectionBPOrigins(ifs) == false ) return(false);
        }

        if( lbuf.find("step      i1-i2        i1-j2        j1-i2        j1-j2        sum") != string::npos ){
            if( ReadSectionBPStepIDs(ifs,BPIDs,BPStepIDs) == false ) return(false);
        }

        if( lbuf.find("step       Px        Py        Pz        Hx        Hy        Hz") != string::npos ){
            if( ReadSectionHelPos(ifs) == false ) return(false);
        }

        if( ParameterType == E3DP_LOCAL ){
            if( lbuf.find("Local base-pair parameters") != string::npos ){
                getline(ifs,lbuf); // skip heading
                if( ReadSectionBPPar(ifs) == false ) return(false);
            }
            if( lbuf.find("Local base-pair step parameters") != string::npos ){
                getline(ifs,lbuf); // skip heading
                if( ReadSectionBPStepPar(ifs) == false ) return(false);
            }
            if( lbuf.find("Local base-pair helical parameters") != string::npos ){
                getline(ifs,lbuf); // skip heading
                if( ReadSectionBPHelPar(ifs) == false ) return(false);
            }
        }

        if( ParameterType == E3DP_SIMPLE ){
            if( lbuf.find("Simple base-pair parameters based on") != string::npos ){
                getline(ifs,lbuf); // skip heading
                if( ReadSectionBPPar(ifs) == false ) return(false);
            }
            if( lbuf.find("Simple base-pair step parameters based on") != string::npos ){
                getline(ifs,lbuf); // skip heading
                if( ReadSectionBPStepPar(ifs) == false ) return(false);
            }
        }

        if( lbuf.find("structure: A-like; B-like; TA-like; intermediate of A and B, or other cases") != string::npos ){
            getline(ifs,lbuf); // skip empty row
            getline(ifs,lbuf); // skip heading
            if( ReadSectionPPar(ifs) == false ) return(false);
        }

        getline(ifs,lbuf);
    }

// cross-check number of BP and BP Pairs (steps)
    if( ReferenceBPIDs.size() > 0 ){
        if( ReferenceBPIDs.size() != BPPar.size() ){
            CSmallString error;
            error << "number of base pairs is not the same as in reference structure";
            ES_ERROR(error);
            return(false);
        }
        if( ReferenceBPStepIDs.size() != BPStepPar.size() ){
            CSmallString error;
            error << "number of base pair steps is not the same as in reference structure";
            ES_ERROR(error);
            return(false);
        }
        if( ReferenceBPStepIDs.size() != BPHelPar.size() ){
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

bool Qx3DNA::ReadSectionBPIDs(std::ifstream& ifs,std::map<int,CNABPID>& bps)
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

bool Qx3DNA::ReadSectionBPStepIDs(std::ifstream& ifs,std::map<int,CNABPID>& bps,std::map<int,CNABPStepID>& bpsteps)
{
    string lbuf;
    getline(ifs,lbuf);
    while( ifs ){
        if( lbuf.find("****************************************************************************") != string::npos ){


            return(true); // end of steps
        }

        stringstream        str(lbuf);
        CNABPStepID         params;

        str >> params.ID >> params.Step;

        if( ! str ){
            CSmallString error;
            error << "unable to read base-pair step in: " << lbuf;
            ES_ERROR(error);
            return(false);
        }

        params.ID--;

        // generate indexes
        std::map<int,CNABPID>::iterator it = bps.begin();
        std::map<int,CNABPID>::iterator ie = bps.end();

        while( it != ie ){
            if( it->second.ID == params.ID ){
                params.ResIDA = it->second.ResIDA;
                params.ResIDD = it->second.ResIDB;
            }
            if( it->second.ID == params.ID + 1 ){
                params.ResIDB = it->second.ResIDA;
                params.ResIDC = it->second.ResIDB;
            }
            it++;
        }

        // insert item
        bpsteps[params.ID] = params;

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read base-pair step";
    ES_ERROR(error);
    return(false);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ReadSectionBPPar(std::ifstream& ifs)
{
    string lbuf,tmp;

    getline(ifs,lbuf);

    while( ifs ){
        if( ( lbuf.find("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~") != string::npos ) || ( lbuf.find("****************************************************************************") != string::npos ) ){
            return(true); // end of BPPar
        }

        CNABPPar        params;
        stringstream    str(lbuf);

        // bp
        str >> tmp;
        if( tmp == "*" ){
            str >> params.ID;
        } else {
            stringstream    tstr(tmp);
            tstr >> params.ID;
        }
        tmp.clear();

        str >> params.Name;        // this should not fail
        params.ID--;

        if( ! ( lbuf.find("----") != string::npos ) ){
            // read the rest of parameters
            //        Shear    Stretch   Stagger    Buckle  Propeller  Opening
            str >> params.Shear >> params.Stretch >> params.Stagger >> params.Buckle >> params.Propeller >> params.Opening;
            if( ! str ){
                CSmallString error;
                error << "unable to read base-pair parameters in: " << lbuf;
                ES_ERROR(error);
                return(false);
            }
            params.Valid = true;  // data are valid
        }

        // check if the current structure has the same pairing as reference structure
        if( ReferenceBPIDs.size() > 0 ){  // reference pairing is available -> perform test
            if( ( ReferenceBPIDs[params.ID].Name.substr(0,1) != params.Name.substr(0,1) ) || ( ReferenceBPIDs[params.ID].Name.substr(2,1) != params.Name.substr(2,1) ) ){
                CSmallString error;
                error << "base pair (" << params.Name << ") with number (" << params.ID << ") is not the same as in reference structure";
                ES_ERROR(error);
                return(false);
            }
        }

        BPPar.push_back(params);

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read base-pair parameters section";
    ES_ERROR(error);
    return(false);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ReadSectionBPStepPar(std::ifstream& ifs)
{
    string lbuf,tmp;

    getline(ifs,lbuf);

    while( ifs ){
        if( ( lbuf.find("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~") != string::npos ) || ( lbuf.find("****************************************************************************") != string::npos ) ){
            return(true); // end of BPStepPar
        }

        stringstream        str(lbuf);
        CNABPStepPar   params;

        // step
        str >> tmp;
        if( tmp == "*" ){
            str >> params.ID;
        } else {
            stringstream    tstr(tmp);
            tstr >> params.ID;
        }
        tmp.clear();

        str >> params.Step; // this should not fail
        params.ID--;

        if( ! ( lbuf.find("----") != string::npos ) ){
            // read the rest of parameters
            // step       Shift     Slide      Rise      Tilt      Roll     Twist
            str >> params.Shift >> params.Slide >> params.Rise >> params.Tilt >> params.Roll >> params.Twist;
            if( ! str ){
                CSmallString error;
                error << "unable to read base-pair step parameters in: " << lbuf;
                ES_ERROR(error);
                return(false);
            }
            params.Valid = true;  // data are valid
        }

        // check if the current structure has the same pairing as reference structure
        if( ReferenceBPStepIDs.size() > 0 ){  // reference pairing is available -> perform test
            if( ReferenceBPStepIDs[params.ID].Step != params.Step ){
                CSmallString error;
                error << "base pair step (" << params.Step << ") with number (" << params.ID << ") is not the same as in reference structure";
                ES_ERROR(error);
                return(false);
            }
        }

        BPStepPar.push_back(params);

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read base-pair step parameters section";
    ES_ERROR(error);
    return(false);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ReadSectionBPHelPar(std::ifstream& ifs)
{
    string lbuf;
    getline(ifs,lbuf);
    while( ifs ){
        if( ( lbuf.find("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~") != string::npos ) || ( lbuf.find("****************************************************************************") != string::npos ) ){
            return(true); // end of BPHelPar
        }

        stringstream    str(lbuf);
        CNABPHelPar     params;
        // step
        str >> params.ID >> params.Step;        // this should not fail - but you can test success as well, code is commented below
        params.ID--;
        //        if( ! str ){
        //            CSmallString error;
        //            error << "unable to read base-pair parameters in: " << lbuf;
        //            ES_ERROR(error);
        //            return(false);
        //        }
        if( ! ( lbuf.find("----") != string::npos ) ){
            // step       X-disp    Y-disp   h-Rise     Incl.       Tip   h-Twist
            str >> params.Xdisp >> params.Ydisp >> params.Hrise >> params.Incl >> params.Tip >> params.Htwist;
            if( ! str ){
                CSmallString error;
                error << "unable to read base-pair helical parameters in: " << lbuf;
                ES_ERROR(error);
                return(false);
            }
            params.Valid = true;  // data are valid
        }

        // check if the current structure has the same pairing as reference structure
        if( ReferenceBPStepIDs.size() > 0 ){  // reference pairing is available -> perform test
            if( ReferenceBPStepIDs[params.ID].Step != params.Step ){
                CSmallString error;
                error << "base pair step (" << params.Step << ") with number (" << params.ID << ") is not the same as in reference structure";
                ES_ERROR(error);
                return(false);
            }
        }

        BPHelPar.push_back(params);

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read base-pair helical parameters section";
    ES_ERROR(error);
    return(false);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ReadSectionPPar(std::ifstream& ifs)
{
    string lbuf,tmp;

    getline(ifs,lbuf);

    while( ifs ){
        if( lbuf.find("****************************************************************************") != string::npos ){
            return(true); // end of PPar
        }

        stringstream        str(lbuf);
        CNAPPar   params;

        // step
        str >> tmp;
        if( tmp == "*" ){
            str >> params.ID;
        } else {
            stringstream    tstr(tmp);
            tstr >> params.ID;
        }
        tmp.clear();

        str >> params.Step; // this should not fail
        params.ID--;

        if( ! ( lbuf.find("---") != string::npos ) ){
            // read the rest of parameters
            // step       Xp      Yp      Zp     XpH     YpH     ZpH    Form
            str >> params.Xp >> params.Yp >> params.Zp >> params.XpH >> params.YpH >> params.ZpH;
            if( ! str ){
                CSmallString error;
                error << "unable to read P parameters in: " << lbuf;
                ES_ERROR(error);
                return(false);
            }
            // determine form of DNA/RNA
            if( str >> tmp ){
                if ( tmp == "A" ) {
                    params.Form = 1;
                } else if ( tmp == "B" ) {
                    params.Form = 2;
                } else if ( tmp == "A-like" ) {
                    params.Form = 3;
                } else if ( tmp == "B-like" ) {
                    params.Form = 4;
                } else if ( tmp == "TA-like" ) {
                    params.Form = 5;
                } else {
                    // unknown form
                    params.Form = -1;
                }
                tmp.clear();
            } else {
                params.Form = -1;
            }

            params.Valid = true;  // data are valid
        }

        // check if the current structure has the same pairing as reference structure
        if( ReferenceBPStepIDs.size() > 0 ){  // reference pairing is available -> perform test
            if( ReferenceBPStepIDs[params.ID].Step != params.Step ){
                CSmallString error;
                error << "base pair step (" << params.Step << ") with number (" << params.ID << ") is not the same as in reference structure";
                ES_ERROR(error);
                return(false);
            }
        }

        PPar.push_back(params);

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read P parameters section";
    ES_ERROR(error);
    return(false);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ReadSectionBPOrigins(std::ifstream& ifs)
{
    string lbuf;
    getline(ifs,lbuf);
    while( ifs ){
        if( lbuf.find("****************************************************************************") != string::npos ){
            return(true); // end of BPOrigins
        }

        stringstream    str(lbuf);
        CPoint          origin;
        int             id;
        string          bp;
        // step
        str >> id >> bp >> origin.x >> origin.y >> origin.z;

        if( ! str ){
            CSmallString error;
            error << "unable to read base-pair origin parameters in: " << lbuf;
            ES_ERROR(error);
            return(false);
        }

        BPOrigins.push_back(origin);

        getline(ifs,lbuf);
    }

    CSmallString error;
    error << "unable to read base-pair origin parameters section";
    ES_ERROR(error);
    return(false);
}

//------------------------------------------------------------------------------

bool Qx3DNA::ReadSectionHelPos(std::ifstream& ifs)
{
    string lbuf;
    getline(ifs,lbuf);
    while( ifs ){
        if( lbuf.empty() ){
            return(true); // end of HelPos
        }

        stringstream    str1(lbuf);
        stringstream    str2(lbuf);
        CPoint          pos;
        int             id;
        string          step;
        string          test;

        str1 >> id >> step >> test;
        if( test != "----" ){
            // step
            str2 >> id >> step >> pos.x >> pos.y >> pos.z;

            if( ! str2 ){
                CSmallString error;
                error << "unable to read helical axis position parameters in: " << lbuf;
                ES_ERROR(error);
                return(false);
            }

            HelAxisPositions.push_back(pos);
        }

        getline(ifs,lbuf);
    }

    // this is the final section
    return(true);
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
    fprintf(p_fout,"REMARK File generated with CATs for 3DNA analysis\n");

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

