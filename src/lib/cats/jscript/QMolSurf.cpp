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
#include <moc_QMolSurf.cpp>
#include <TerminalStr.hpp>
#include <PeriodicTable.hpp>
#include <QTemporaryDir>
#include <boost/format.hpp>



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

// do Molalysis -------------------------------
    // clear previous data
    ClearAll();
    
    // create temporary directory
    QTemporaryDir tmp_dir;
    tmp_dir.setAutoRemove(false); // keep files in the case of failure
    if( ! tmp_dir.isValid() ){
        // TODO
        // report that directory cannot be created
        return( ThrowError("snapshot[,selection]","unable to run analysis in tmp dir") );
    }
    WorkDir = CFileName(tmp_dir.path());
	
    //FIXME - put to the error message the pathname to the working directory

    // check if topology or selections contains atoms
    if(p_qsnap->Restart.GetTopology()->AtomList.GetNumberOfAtoms() == 0 ||  p_qsel->Mask.GetNumberOfSelectedAtoms() == 0){
        SASA[0]=0.0;
        SESA[0]=0.0;
        return(true);
    }

    // write input data
    if( WriteInputData(p_qsnap,p_qsel) == false ){
        return( ThrowError("snapshot[,selection]","unable to write input data") );
    }

    // start analysis
    if( RunAnalysis() == false ){
        return( ThrowError("snapshot[,selection]","unable to run analysis (RunAnalysis)"));
    }

    // parse output data
    if( ParseOutputData() == false ){
        return( ThrowError("snapshot[,selection]","unable to parse output"));
    }
    
    // clean temporary directory
    tmp_dir.remove();        

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

/// set probe radius
/// setProbeRadius(double)
QScriptValue QMolSurf::setProbeRadius(const QScriptValue& dummy)
{
    QScriptValue value;

    // help ------------------------------------------
        if( IsHelpRequested() ){
            CTerminalStr sout;
            sout << "usage:  QMolSurf::setProbeRadius(x)" << endl;
            return(value);
        }

// check arguments -------------------------------
        value = CheckNumberOfArguments("x",1);
        if( value.isError() ) return(value);

        double x;

        value = GetArgAsRNumber("x","x",1,x);
        if( value.isError() ) return(value);

// execute ---------------------------------------
        probeRadius = x;
        return(value);
}

// ------------------------------------------------------------------------

/// get probe radius
/// double getProbeRadius()
QScriptValue QMolSurf::getProbeRadius(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double QMolSurf::getProbeRadius()" << endl;
        return(0);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(0);

// execute ---------------------------------------
    return(probeRadius);

}

// ------------------------------------------------------------------------


/// get solvent accesible surface area
/// double getSASA()
/// double getSASA(index)
/// double getSASA(selections)
QScriptValue QMolSurf::getSASA(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double MolSurf::getSASA()"  << \
                "       double getSASA(index)"      << \
                "       double getSASA(selections)" << endl;
        return(false);
    }

// check arguments & execute -------------------------------
    // double getSASA()
    if( GetArgumentCount() == 0 ){
        map<int, double>::iterator it;
        double SASAtot = 0.0;
        for(it = SASA.begin(); it!=SASA.end(); ++it)
            SASAtot += it->second;
        return(SASAtot);
    }
    else if( GetArgumentCount() == 1 )
    {
        value = CheckNumberOfArguments("index/Selection",1);
        if( value.isError() ) return(value);

        // double getSASA(index)
        if( IsArgumentInt(1) ){
            int index;
            value = (GetArgAsInt("index","index",1,index));
            if( value.isError() ) return(value);
            if( (index < 0) || (index >= (int)SASA.size()) )
                return( ThrowError("index","index out-of-legal range") );

            return(SASA[index]);

        } else {

            // double getSASA(selection)
            QSelection* p_qsel = NULL;
            value = GetArgAsObject<QSelection*>("selection","selection","Selection",1,p_qsel);
            if( value.isError() ) return(value);

            double SASAsel = 0.0;

            for(int i = 0; i < p_qsel->Mask.GetNumberOfSelectedAtoms(); i++) {
                map<int, int>::iterator it = AtomIDMap.find(p_qsel->Mask.GetSelectedAtomCondensed(i)->GetAtomIndex());
                if(it != AtomIDMap.end()){
                    SASAsel += SASA[it->second];
                }
            }
            return(SASAsel);
        }
    }
    return(QScriptValue());
}

//------------------------------------------------------------------------------

/// get solvent excluded surface area
/// double getSESA()
/// double getSESA(index)
/// double getSESA(selections)
QScriptValue QMolSurf::getSESA(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double MolSurf::getSESA()"  << \
                "       double getSESA(index)"      << \
                "       double getSESA(Selection)" << endl;
        return(false);
    }

// check arguments & execute -------------------------------

    // double getSESA()
    if( GetArgumentCount() == 0 ){
        map<int, double>::iterator it;
        double SESAtot = 0.0;
        for(it = SESA.begin(); it!=SESA.end(); ++it)
            SESAtot += it->second;
        return(SESAtot);
    }

    else if( GetArgumentCount() == 1 )
    {
        value = CheckNumberOfArguments("index/Selection",1);
        if( value.isError() ) return(value);

        // double getSESA(index)
        if( IsArgumentInt(1) ){
            int index;
            value = (GetArgAsInt("index","index",1,index));
            if( value.isError() ) return(value);
            if( (index < 0) || (index >= (int)SESA.size()) )
                return( ThrowError("index","index out-of-legal range") );

            return(SESA[index]);

        } else {

            // double getSESA(selection)
            /// FIXME selekcia podselekciou tej v analyze
            QSelection* p_qsel = NULL;
            value = GetArgAsObject<QSelection*>("selection","selection","Selection",1,p_qsel);
            if( value.isError() ) return(value);

            double SESAsel = 0.0;

            for(int i = 0; i < p_qsel->Mask.GetNumberOfSelectedAtoms(); i++) {
                map<int, int>::iterator it = AtomIDMap.find(p_qsel->Mask.GetSelectedAtomCondensed(i)->GetAtomIndex());
                if(it != AtomIDMap.end()){
                    SESAsel += SESA[it->second];
                }
            }
            return(SESAsel);
        }
    }
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
    AtomIDMap.clear();
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
        // make AtomIDMap
        for(int i = 0; i < p_qsel->Mask.GetNumberOfSelectedAtoms(); i++){
                AtomIDMap[p_qsel->Mask.GetSelectedAtomCondensed(i)->GetAtomIndex()] = i;
        }
    } else {
        CAmberMaskAtoms fake_mask;
        fake_mask.AssignTopology(p_qsnap->Restart.GetTopology());
        fake_mask.SelectAllAtoms();
        // write to output file
        WriteXYZR(p_qsnap->Restart.GetTopology(),&p_qsnap->Restart,&fake_mask,p_fout);
        // make AtomIDMap
        for(int i = 0; i < fake_mask.GetNumberOfSelectedAtoms(); i++){
            AtomIDMap[i] = i;
        }
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
// check topology & snapshot


// run MolSurf program -------------------------------

    // prepare command
    stringstream cmd;
    cmd << boost::format("cd %s > /dev/null 2>&1 && "
                         "msms -if QMolSurf.xyzr -af QMolSurf.area -probe_radius %d > msms.stdout 2>&1")%WorkDir %probeRadius;

    // start analysis -> msms of .xyzr to area output file .area containing sasa&sesa for indexed atoms
    int status = system( cmd.str().c_str() );

    if (status != 0){
        CSmallString error;
        error << "running MSMS program failed - " << strerror(errno);
        ES_ERROR(error);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool QMolSurf::ParseOutputData(void)
{
    //  open file
    ifstream ifs;
    CFileName fileName = WorkDir / "QMolSurf.area";
    ifs.open( fileName );
    if( ifs.fail() ) {
        CSmallString error;
        error << "unable to open analysis output file " << fileName;
        error << " (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }

    // read MSMS surface area output params SASA & SESA
    string lbuf;
    getline(ifs,lbuf);
    getline(ifs,lbuf);
    while( !ifs.eof() ){
        int n =0;
        double sasa = 0.0;
        double sesa = 0.0;
        stringstream    str(lbuf);
        str >>   n >> sesa >> sasa;
        SASA.insert(std::pair<int, double>(n, sasa));
        SESA.insert(std::pair<int, double>(n, sesa));
        getline(ifs,lbuf);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool QMolSurf::WriteXYZR(CAmberTopology* p_top,CAmberRestart* p_crd,CAmberMaskAtoms* p_mask,FILE* p_fout)
{

    for(int i=0; i < p_top->AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = p_mask->GetSelectedAtom(i);
        if( p_atom == NULL ) {
            continue;
        }
        fprintf(p_fout,"%8.3f%8.3f%8.3f %6.3f\n",
                p_crd->GetPosition(i).x, p_crd->GetPosition(i).y, p_crd->GetPosition(i).z,
                p_atom->GetRadius());
    }
    return(true);
}

//------------------------------------------------------------------------------

