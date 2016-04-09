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
#include <QCurve.hpp>
#include <TerminalStr.hpp>
#include <QSnapshot.hpp>
#include <QTopology.hpp>
#include <QSelection.hpp>
#include <Qx3DNA.hpp>
#include <fstream>
#include <boost/format.hpp>

//------------------------------------------------------------------------------

#include <QCurve.moc>
using namespace std;
using boost::format;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QCurve::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QCurve::New);
    QScriptValue metaObject = engine.newQMetaObject(&QCurve::staticMetaObject, ctor);
    engine.globalObject().setProperty("Curve", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QCurve::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    QCATsScriptable scriptable("Curve");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Curve object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new Curve()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    QCurve* p_obj = new QCurve();
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QCurve::QCurve(void)
    : QCATsScriptable("Curve")
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
  
QScriptValue QCurve::setDegree(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Curve::setDegree(degree)" << endl;
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

QScriptValue QCurve::getDegree(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Curve::getDegree()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Degree);
}

//------------------------------------------------------------------------------

QScriptValue QCurve::setDimension(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Curve::setDimension(dim)" << endl;
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

QScriptValue QCurve::getDimension(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Curve::getDimension()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Dimension);
}    

//------------------------------------------------------------------------------
  
QScriptValue QCurve::setType(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Curve::setType(type)" << endl;
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

QScriptValue QCurve::getType(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string Curve::getType()" << endl;
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

QScriptValue QCurve::initSpline(void)
{
        QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Curve::initSpline(nctrlpts)" << endl;
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
        return(ThrowError("nctrlpts","unable to init spline"));
    }
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QCurve::initSplineFromAtoms(void)
{       
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Curve::initSplineFromAtoms(snapshot[,selection])" << endl;
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
            return(ThrowError("snapshot[,selection]","unable to init spline"));
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
            return(ThrowError("snapshot[,selection]","unable to init spline"));
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

QScriptValue QCurve::initSplineFrom3DNABPOrigins(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Curve::initSplineFrom3DNABPOrigins(x3dna)" << endl;
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
        return(ThrowError("x3dna","unable to init spline"));
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

QScriptValue QCurve::initSplineFrom3DNAHelAxis(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Curve::initSplineFrom3DNAHelAxis(x3dna)" << endl;
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
        return(ThrowError("x3dna","unable to init spline"));
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

QScriptValue QCurve::setCtrlPointValue(void)
{
        QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Curve::setCtrlPointValue(index,value,dim)" << endl;
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

QScriptValue QCurve::getNumOfCtrlPoints(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Curve::getNumOfCtrlPoints()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return((int)SSpline.n_ctrlp);   
}

//------------------------------------------------------------------------------

QScriptValue QCurve::writeXYZ(void)
{
        QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Curve::writeXYZ(name[,density[,symbol]])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name[,density[,symbol]]",1,3);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name[,density[,symbol]]","name",1,name);
    if( value.isError() ) return(value); 
    
    int density = 200;
    if( GetArgumentCount() > 1 ){
        value = GetArgAsInt("name,density[,symbol]","density",2,density);
        if( value.isError() ) return(value);
    }
    if( density <= 0 ){
        return(ThrowError("name[,density[,symbol]]","density must be larger than 0"));
    }
    
    QString symbol("O");
    if( GetArgumentCount() > 2 ){    
        value = GetArgAsString("name,density,symbol","symbol",3,symbol);
        if( value.isError() ) return(value);
    }
    
    if( ! ((SSpline.dim == 3) || (SSpline.dim == 4)) ){
        return(ThrowError("name[,density[,symbol]]","spline dimension must be either 3 or 4"));        
    }
            
// execute ---------------------------------------
    ofstream ofs;
    ofs.open(name.toStdString().c_str());
    if( ! ofs ){
        return(ThrowError("name[,density[,symbol]]","unable to open output file"));
    }
    ofs << density + 1 <<  endl;
    ofs << "curve" << endl;

    for(int i=0; i <= density; i++){
        double u = (double)i/((double)density);
        tsDeBoorNet net;
        ts_bspline_evaluate(&SSpline, u, &net);
        ofs << format("%2s %14.3f %14.3f %14.3f\n")%symbol.toStdString()%net.result[0]%net.result[1]%net.result[2];
        ts_deboornet_free(&net);
    }
    
    ofs.close();
    
    return(value);    
}

//------------------------------------------------------------------------------

QScriptValue QCurve::writeCurvature(void)
{
        QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Curve::writeCurvature(name[,density])" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name[,density]",1,2);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name[,density]","name",1,name);
    if( value.isError() ) return(value);

    int density = 200;
    if( GetArgumentCount() > 1 ){
        value = GetArgAsInt("name[,density]","density",2,density);
        if( value.isError() ) return(value);
    }
    if( density <= 0 ){
        return(ThrowError("name[,density]","density must be larger than 0"));
    }

    if( ! ((SSpline.dim == 2) || (SSpline.dim == 3) || (SSpline.dim == 4)) ){
        return(ThrowError("name[,density]","dimension must be 2, 3, or 4"));
    }

// execute ---------------------------------------

    // prepare spline derivatives
    ts_bspline_free(&DSpline);
    ts_bspline_free(&TSpline);
    if( ts_bspline_derive(&SSpline,&DSpline) != TS_SUCCESS ){
        return(ThrowError("name[,density]","unable to calculate first derivatives"));
    }
    if( ts_bspline_derive(&DSpline,&TSpline) != TS_SUCCESS ){
        return(ThrowError("name[,density]","unable to calculate second derivatives"));
    }

    ofstream ofs;
    ofs.open(name.toStdString().c_str());
    if( ! ofs ){
        return(ThrowError("name[,density]","unable to open output file"));
    }

    ofs << "# alpha curvature" << endl;
    ofs << "# ----- ---------" << endl;

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

        ofs << format("%7.5f %20.6f\n")%u%kappa;
    }

    ofs.close();

    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


