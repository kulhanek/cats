// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2016 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QScriptEngine>
#include <QTinySpline.hpp>
#include <moc_QTinySpline.cpp>
#include <TerminalStr.hpp>
#include <QSnapshot.hpp>
#include <QTopology.hpp>
#include <QSelection.hpp>
#include <Qx3DNA.hpp>
#include <QCurvesP.hpp>
#include <QOFile.hpp>
#include <fstream>
#include <boost/format.hpp>

//------------------------------------------------------------------------------

using namespace std;
using boost::format;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QTinySpline::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QTinySpline::New);
    QScriptValue metaObject = engine.newQMetaObject(&QTinySpline::staticMetaObject, ctor);
    engine.globalObject().setProperty("TinySpline", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QTinySpline::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    QCATsScriptable scriptable("TinySpline");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "TinySpline object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new TinySpline()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    QTinySpline* p_obj = new QTinySpline();
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QTinySpline::QTinySpline(void)
    : QCATsScriptable("TinySpline")
{
    ts_bspline_default(&SSpline);
    ts_bspline_default(&DSpline);
    ts_bspline_default(&TSpline);
    Degree = 3;
    Dimension = 3;
    Type = TS_CLAMPED;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
  
QScriptValue QTinySpline::setDegree(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TinySpline::setDegree(degree)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("degree",1);
    if( value.isError() ) return(value);

    int degree;
    value = GetArgAsInt("degree","degree",1,degree);
    if( value.isError() ) return(value);
    
    if( degree < 1 ){
        return(ThrowError("degree","degree should be greater than 2"));
    }    
    
// execute ---------------------------------------  
    Degree = degree;
    return(value);
}
 
//------------------------------------------------------------------------------

QScriptValue QTinySpline::getDegree(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int TinySpline::getDegree()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Degree);
}

//------------------------------------------------------------------------------

QScriptValue QTinySpline::setDimension(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TinySpline::setDimension(dim)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("dim",1);
    if( value.isError() ) return(value);
    
    int dim;
    value = GetArgAsInt("dim","dim",1,dim);
    if( value.isError() ) return(value);
    
    if( dim < 2 ){
        return(ThrowError("dim","dimension should be greater than 1"));
    }       

// execute ---------------------------------------  
    Dimension = dim;
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QTinySpline::getDimension(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int TinySpline::getDimension()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Dimension);
}    

//------------------------------------------------------------------------------
  
QScriptValue QTinySpline::setType(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TinySpline::setType(type)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("type",1);
    if( value.isError() ) return(value);

    QString stype;
    value = GetArgAsString("type","type",1,stype);
    if( value.isError() ) return(value); 

// execute ---------------------------------------    
    Type = TS_CLAMPED;
    if( stype == "clamped" ){
        Type = TS_CLAMPED;
    } else if( stype == "opened" ) {
        Type = TS_OPENED;        
    } else if( stype == "beziers" ) {
        Type = TS_BEZIERS;        
    } else if( stype == "none" ) {
        Type = TS_NONE;        
    }  else {
        CSmallString error;
        error << "unsupported spline type '" << stype << "'";
        return(ThrowError("degree,dim,nctrlpts[,type]",error));
    }
    
    return(value);
}
  
//------------------------------------------------------------------------------

QScriptValue QTinySpline::getType(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string TinySpline::getType()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    switch(Type){
        case TS_CLAMPED:
            return("clamped");
        case TS_OPENED:
            return("opened");
        case TS_BEZIERS:
            return("beziers");
        case TS_NONE:
        default:
            return("none");
    } 
}  
    
//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================    

QScriptValue QTinySpline::initSpline(void)
{
        QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TinySpline::initSpline(nctrlpts)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("nctrlpts",1);
    if( value.isError() ) return(value);

    int nctrlpts;
    value = GetArgAsInt("nctrlpts","nctrlpts",1,nctrlpts);
    if( value.isError() ) return(value);
    
    if( nctrlpts < 2 ){
        return(ThrowError("nctrlpts","number of control points should be greater than 1"));
    }         
     
// execute ---------------------------------------
    ts_bspline_free(&SSpline);
    ts_bspline_free(&DSpline);
    ts_bspline_free(&TSpline); 
    
    tsError retcode = ts_bspline_new(Degree, Dimension, nctrlpts, Type, &SSpline);
    if( retcode != TS_SUCCESS ){
        CSmallString error;
        error << "unable to init spline, number of points: " << nctrlpts;
        return(ThrowError("nctrlpts",error));
    }
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QTinySpline::initSplineFromAtoms(void)
{       
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TinySpline::initSplineFromAtoms(snapshot[,selection])" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("snapshot[,selection]",1,2);
    if( value.isError() ) return(value);

    QSnapshot* p_snap;
    GetArgAsObject("snapshot[,selection]","snapshot","Snapshot",1,p_snap);
    if( value.isError() ) return(value);
    
    if( ! ((Dimension == 3) || (Dimension == 4)) ){
        return(ThrowError("snapshot[,selection]","spline dimension must be either 3 or 4"));        
    }
    
// execute ---------------------------------------
    ts_bspline_free(&SSpline);
    ts_bspline_free(&DSpline);
    ts_bspline_free(&TSpline);     
    
    if( GetArgumentCount() == 1 ){

        int nctrlpts = p_snap->Restart.GetNumberOfAtoms();
        tsError retcode = ts_bspline_new(Degree, Dimension, nctrlpts, Type, &SSpline);
        if( retcode != TS_SUCCESS ){
            CSmallString error;
            error << "unable to init spline, number of points: " << nctrlpts;
            return(ThrowError("snapshot[,selection]",error));
        }
        
        CAmberRestart* p_src = &p_snap->Restart;
        for(int i=0; i < p_src->GetNumberOfAtoms(); i++ ){
            CPoint pos = p_src->GetPosition(i);
            int lindex;
            lindex = i * SSpline.dim + 0;
            SSpline.ctrlp[lindex] = pos.x;
            lindex = i * SSpline.dim + 1;
            SSpline.ctrlp[lindex] = pos.y; 
            lindex = i * SSpline.dim + 2;
            SSpline.ctrlp[lindex] = pos.z;
            if( SSpline.dim == 4 ){
                lindex = i * SSpline.dim + 3;
                SSpline.ctrlp[lindex] = 1.0;
            }
        }        
        
        return(value);
    } else {

        QSelection* p_sel;
        GetArgAsObject("snapshot,selection","selection","Selection",2,p_sel);

        if( p_snap->Restart.GetNumberOfAtoms() < p_sel->Mask.GetNumberOfTopologyAtoms() ){
            CSmallString error;
            error << "inconsistent number of atoms, selection topology (" << p_sel->Mask.GetNumberOfTopologyAtoms() << "), source (" << p_snap->Restart.GetNumberOfAtoms() << ")";
            return(ThrowError("snapshot,selection",error));
        }  

        int nctrlpts = p_sel->Mask.GetNumberOfSelectedAtoms();
        tsError retcode = ts_bspline_new(Degree, Dimension, nctrlpts, Type, &SSpline);
        if( retcode != TS_SUCCESS ){
            CSmallString error;
            error << "unable to init spline, number of points: " << nctrlpts;
            return(ThrowError("snapshot,selection",error));
        }
        
        CAmberRestart* p_src = &p_snap->Restart;
        for(int i=0; i < p_sel->Mask.GetNumberOfSelectedAtoms(); i++ ){
            int j = p_sel->Mask.GetSelectedAtomCondensed(i)->GetAtomIndex();
            CPoint pos = p_src->GetPosition(j);
            int lindex;
            lindex = i * SSpline.dim + 0;
            SSpline.ctrlp[lindex] = pos.x;
            lindex = i * SSpline.dim + 1;
            SSpline.ctrlp[lindex] = pos.y; 
            lindex = i * SSpline.dim + 2;
            SSpline.ctrlp[lindex] = pos.z;
            if( SSpline.dim == 4 ){
                lindex = i * SSpline.dim + 3;
                SSpline.ctrlp[lindex] = 1.0;
            }
        }         
        
        return(value);
    }
}

//------------------------------------------------------------------------------

QScriptValue QTinySpline::initSplineFrom3DNABPOrigins(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TinySpline::initSplineFrom3DNABPOrigins(x3dna)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("x3dna",1);
    if( value.isError() ) return(value);

    Qx3DNA* p_x3dna;
    GetArgAsObject("x3dna","x3dna","x3DNA",1,p_x3dna);
    if( value.isError() ) return(value);

    if( ! ((Dimension == 3) || (Dimension == 4)) ){
        return(ThrowError("x3dna","spline dimension must be either 3 or 4"));
    }

// execute ---------------------------------------
    ts_bspline_free(&SSpline);
    ts_bspline_free(&DSpline);
    ts_bspline_free(&TSpline);

    int nctrlpts = p_x3dna->BPOrigins.size();
    tsError retcode = ts_bspline_new(Degree, Dimension, nctrlpts, Type, &SSpline);
    if( retcode != TS_SUCCESS ){
        CSmallString error;
        error << "unable to init spline, number of points: " << nctrlpts;
        return(ThrowError("x3dna",error));
    }

    for(int i = 0; i < nctrlpts; i++){
        CPoint pos = p_x3dna->BPOrigins[i];
        int lindex;
        lindex = i * SSpline.dim + 0;
        SSpline.ctrlp[lindex] = pos.x;
        lindex = i * SSpline.dim + 1;
        SSpline.ctrlp[lindex] = pos.y;
        lindex = i * SSpline.dim + 2;
        SSpline.ctrlp[lindex] = pos.z;
        if( SSpline.dim == 4 ){
            lindex = i * SSpline.dim + 3;
            SSpline.ctrlp[lindex] = 1.0;
        }
    }

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QTinySpline::initSplineFrom3DNAHelAxis(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TinySpline::initSplineFrom3DNAHelAxis(x3dna)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("x3dna",1);
    if( value.isError() ) return(value);

    Qx3DNA* p_x3dna;
    GetArgAsObject("x3dna","x3dna","x3DNA",1,p_x3dna);
    if( value.isError() ) return(value);

    if( ! ((Dimension == 3) || (Dimension == 4)) ){
        return(ThrowError("x3dna","spline dimension must be either 3 or 4"));
    }

// execute ---------------------------------------
    ts_bspline_free(&SSpline);
    ts_bspline_free(&DSpline);
    ts_bspline_free(&TSpline);

    int nctrlpts = p_x3dna->HelAxisPositions.size();
    tsError retcode = ts_bspline_new(Degree, Dimension, nctrlpts, Type, &SSpline);
    if( retcode != TS_SUCCESS ){
        CSmallString error;
        error << "unable to init spline, number of points: " << nctrlpts;
        return(ThrowError("x3dna",error));
    }

    for(int i = 0; i < nctrlpts; i++){
        CPoint pos = p_x3dna->HelAxisPositions[i];
        int lindex;
        lindex = i * SSpline.dim + 0;
        SSpline.ctrlp[lindex] = pos.x;
        lindex = i * SSpline.dim + 1;
        SSpline.ctrlp[lindex] = pos.y;
        lindex = i * SSpline.dim + 2;
        SSpline.ctrlp[lindex] = pos.z;
        if( SSpline.dim == 4 ){
            lindex = i * SSpline.dim + 3;
            SSpline.ctrlp[lindex] = 1.0;
        }
    }

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QTinySpline::initSplineFromCurvesPHelAxis(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TinySpline::initSplineFromCurvesPHelAxis(curvesp)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("curvesp",1);
    if( value.isError() ) return(value);

    QCurvesP* p_curvesp;
    GetArgAsObject("curvesp","curvesp","CurvesP",1,p_curvesp);
    if( value.isError() ) return(value);

    if( ! ((Dimension == 3) || (Dimension == 4)) ){
        return(ThrowError("curvesp","spline dimension must be either 3 or 4"));
    }

// execute ---------------------------------------
    ts_bspline_free(&SSpline);
    ts_bspline_free(&DSpline);
    ts_bspline_free(&TSpline);

    int nctrlpts = p_curvesp->HelAxisPositions.size();
    tsError retcode = ts_bspline_new(Degree, Dimension, nctrlpts, Type, &SSpline);
    if( retcode != TS_SUCCESS ){
        CSmallString error;
        error << "unable to init spline, number of points: " << nctrlpts;
        return(ThrowError("curvesp",error));
    }

    for(int i = 0; i < nctrlpts; i++){
        CPoint pos = p_curvesp->HelAxisPositions[i];
        int lindex;
        lindex = i * SSpline.dim + 0;
        SSpline.ctrlp[lindex] = pos.x;
        lindex = i * SSpline.dim + 1;
        SSpline.ctrlp[lindex] = pos.y;
        lindex = i * SSpline.dim + 2;
        SSpline.ctrlp[lindex] = pos.z;
        if( SSpline.dim == 4 ){
            lindex = i * SSpline.dim + 3;
            SSpline.ctrlp[lindex] = 1.0;
        }
    }

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QTinySpline::setCtrlPointValue(void)
{
        QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TinySpline::setCtrlPointValue(index,value,dim)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index,value,dim",3);
    if( value.isError() ) return(value);

    int index;
    value = GetArgAsInt("index,value,dim","index",1,index); 
    
    if( (index >= (int)SSpline.n_ctrlp) || (index < 0 ) ){
        CSmallString error;
        error << "index '" << index << "' is  out of legal range <0;" << SSpline.n_ctrlp-1 << ">";
        return(ThrowError("index,value,dim",error));
    }    
    
    double data;
    value = GetArgAsRNumber("index,value,dim","value",2,data);
    if( value.isError() ) return(value);
    
    int dim;
    value = GetArgAsInt("index,value,dim","dim",3,dim);
    if( value.isError() ) return(value);

    if( (dim >= (int)SSpline.dim) || (dim < 0 ) ){
        CSmallString error;
        error << "dimension '" << dim << "' is  out of legal range <0;" << SSpline.dim-1 << ">";
        return(ThrowError("index,value,dim",error));
    }
        
// execute ---------------------------------------
    int lindex = index * SSpline.dim + dim;
    SSpline.ctrlp[lindex] = data;
    
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QTinySpline::getNumOfCtrlPoints(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int TinySpline::getNumOfCtrlPoints()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return((int)SSpline.n_ctrlp);   
}

//------------------------------------------------------------------------------

QScriptValue QTinySpline::getCurvature(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double TinySpline::getCurvature(u)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("u",1);
    if( value.isError() ) return(value);

    double u;
    value = GetArgAsRNumber("u","u",1,u);
    if( value.isError() ) return(value);

    if( (u < 0.0) || (u > 1.0 ) ){
        return(ThrowError("u","parameter u is not in <0;1> interval"));
    }

    if( ! ((SSpline.dim == 3) || (SSpline.dim == 4)) ){
        return(ThrowError("u","spline dimension must be either 3 or 4"));
    }

// execute ---------------------------------------
    // prepare spline derivatives
    ts_bspline_free(&DSpline);
    ts_bspline_free(&TSpline);
    if( ts_bspline_derive(&SSpline,&DSpline) != TS_SUCCESS ){
        return(ThrowError("u","unable to calculate first derivatives"));
    }
    if( ts_bspline_derive(&DSpline,&TSpline) != TS_SUCCESS ){
        return(ThrowError("u","unable to calculate second derivatives"));
    }

    tsDeBoorNet net1;
    tsDeBoorNet net2;
    ts_bspline_evaluate(&DSpline, u, &net1);
    ts_bspline_evaluate(&TSpline, u, &net2);

    double kappa = 0.0;

    if( SSpline.dim == 2 ){
        kappa = fabs(net1.result[0]*net2.result[1] - net1.result[1]*net2.result[0]) /
                pow(net1.result[0]*net1.result[0] + net1.result[1]*net1.result[1],1.5);

    } else {
        // SSpline.dim == 3 or higher
        kappa = sqrt( pow((net2.result[2]*net1.result[1] - net2.result[1]*net1.result[2]),2.0) +
                      pow((net2.result[0]*net1.result[2] - net2.result[2]*net1.result[0]),2.0) +
                      pow((net2.result[1]*net1.result[0] - net2.result[0]*net1.result[1]),2.0) ) /
                pow(net1.result[0]*net1.result[0] + net1.result[1]*net1.result[1] + net1.result[2]*net1.result[2],1.5);
    }

    ts_deboornet_free(&net1);
    ts_deboornet_free(&net2);

    return(kappa);
}

//------------------------------------------------------------------------------

QScriptValue QTinySpline::writeXYZ(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TinySpline::writeXYZ(name/ofile[,density[,symbol]])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name/ofile[,density[,symbol]]",1,3);
    if( value.isError() ) return(value);

    ofstream  ofs;
    ofstream* p_ofs = NULL;
    if( IsArgumentObject<QOFile*>(1) ){
        QOFile* p_ofile;
        GetArgAsObject("name/ofile[,density[,symbol]]","ofile","OFile",1,p_ofile);
        if( value.isError() ) return(value);
        p_ofs = &p_ofile->vout;
    } else {
        QString name;
        value = GetArgAsString("name/ofile[,density[,symbol]]","name",1,name);
        if( value.isError() ) return(value);

        ofs.open(name.toStdString().c_str());
        if( ! ofs ){
            return(ThrowError("name/ofile[,density[,symbol]]","unable to open output file"));
        }
        p_ofs = &ofs;
    }
    
    int density = 200;
    if( GetArgumentCount() > 1 ){
        value = GetArgAsInt("name/ofile,density[,symbol]","density",2,density);
        if( value.isError() ) return(value);
    }
    if( density <= 0 ){
        return(ThrowError("name/ofile[,density[,symbol]]","density must be larger than 0"));
    }
    
    QString symbol("O");
    if( GetArgumentCount() > 2 ){    
        value = GetArgAsString("name/ofile,density,symbol","symbol",3,symbol);
        if( value.isError() ) return(value);
    }
    
    if( ! ((SSpline.dim == 3) || (SSpline.dim == 4)) ){
        return(ThrowError("name/ofile[,density[,symbol]]","spline dimension must be either 3 or 4"));
    }
            
// execute ---------------------------------------

    *p_ofs << density + 1 <<  endl;
    *p_ofs << "curve" << endl;

    for(int i=0; i <= density; i++){
        double u = (double)i/((double)density);
        tsDeBoorNet net;
        ts_bspline_evaluate(&SSpline, u, &net);
        *p_ofs << format("%2s %14.3f %14.3f %14.3f\n")%symbol.toStdString()%net.result[0]%net.result[1]%net.result[2];
        ts_deboornet_free(&net);
    }
        
    return(value);    
}

//------------------------------------------------------------------------------

QScriptValue QTinySpline::writeCurvature(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TinySpline::writeCurvature(name/ofile[,density])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name/ofile[,density]",1,2);
    if( value.isError() ) return(value);

    ofstream  ofs;
    ofstream* p_ofs = NULL;
    if( IsArgumentObject<QOFile*>(1) ){
        QOFile* p_ofile;
        GetArgAsObject("name/ofile[,density]","ofile","OFile",1,p_ofile);
        if( value.isError() ) return(value);
        p_ofs = &p_ofile->vout;
    } else {
        QString name;
        value = GetArgAsString("name/ofile[,density]","name",1,name);
        if( value.isError() ) return(value);

        ofs.open(name.toStdString().c_str());
        if( ! ofs ){
            return(ThrowError("name/ofile[,density]","unable to open output file"));
        }
        p_ofs = &ofs;
    }

    int density = 200;
    if( GetArgumentCount() > 1 ){
        value = GetArgAsInt("name/ofile[,density]","density",2,density);
        if( value.isError() ) return(value);
    }
    if( density <= 0 ){
        return(ThrowError("name/ofile[,density]","density must be larger than 0"));
    }

    if( ! ((SSpline.dim == 2) || (SSpline.dim == 3) || (SSpline.dim == 4)) ){
        return(ThrowError("name/ofile[,density]","dimension must be 2, 3, or 4"));
    }

// execute ---------------------------------------

    // prepare spline derivatives
    ts_bspline_free(&DSpline);
    ts_bspline_free(&TSpline);
    if( ts_bspline_derive(&SSpline,&DSpline) != TS_SUCCESS ){
        return(ThrowError("name/ofile[,density]","unable to calculate first derivatives"));
    }
    if( ts_bspline_derive(&DSpline,&TSpline) != TS_SUCCESS ){
        return(ThrowError("name/ofile[,density]","unable to calculate second derivatives"));
    }

    *p_ofs << "# alpha curvature" << endl;
    *p_ofs << "# ----- ---------" << endl;

    for(int i=0; i <= density; i++){
        double u = (double)i/((double)density);
        tsDeBoorNet net1;
        tsDeBoorNet net2;
        ts_bspline_evaluate(&DSpline, u, &net1);
        ts_bspline_evaluate(&TSpline, u, &net2);

        double kappa = 0.0;

        if( SSpline.dim == 2 ){
            kappa = fabs(net1.result[0]*net2.result[1] - net1.result[1]*net2.result[0]) /
                    pow(net1.result[0]*net1.result[0] + net1.result[1]*net1.result[1],1.5);

        } else {
            // SSpline.dim == 3 or higher
            kappa = sqrt( pow((net2.result[2]*net1.result[1] - net2.result[1]*net1.result[2]),2.0) +
                          pow((net2.result[0]*net1.result[2] - net2.result[2]*net1.result[0]),2.0) +
                          pow((net2.result[1]*net1.result[0] - net2.result[0]*net1.result[1]),2.0) ) /
                    pow(net1.result[0]*net1.result[0] + net1.result[1]*net1.result[1] + net1.result[2]*net1.result[2],1.5);
        }

        ts_deboornet_free(&net1);
        ts_deboornet_free(&net2);

        *p_ofs << format("%7.5f %20.6f\n")%u%kappa;
    }

    *p_ofs << endl;

    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


