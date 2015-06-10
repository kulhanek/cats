// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QVolumeData.hpp>
#include <fstream>
#include <QSnapshot.hpp>
#include <QPoint.hpp>
#include <SmallString.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

#include <QVolumeData.moc>
using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QVolumeData::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QVolumeData::New);
    QScriptValue metaObject = engine.newQMetaObject(&QVolumeData::staticMetaObject, ctor);
    engine.globalObject().setProperty("VolumeData", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QVolumeData::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    QCATsScriptable scriptable("VolumeData");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "VolumeData object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new VolumeData()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    QVolumeData* p_obj = new QVolumeData();
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QVolumeData::QVolumeData(void)
    : QCATsScriptable("VolumeData")
{
    NX=0;
    NY=0;
    NZ=0;
    OutlierValue = 0.0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QVolumeData::loadCubeFile(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool VolumeData::loadCubeFile(name)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name",1);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name","name",1,name);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    // open file
    ifstream ifs;
    ifs.open(name.toStdString().c_str());
    if( ! ifs ){
        CSmallString error;
        error << "unable to open file : " << name;
        return(ThrowError("name",error));
    }

    // read comment
    getline(ifs,Comment1);
    getline(ifs,Comment2);
    if( ! ifs ){
        return(ThrowError("name","unable to read comments"));
    }

    // read header
    int natoms = 0;
    ifs >> natoms >> Origin.x >> Origin.y >> Origin.z;
    ifs >> NX >> XDir.x >> XDir.y >> XDir.z;
    ifs >> NY >> YDir.x >> YDir.y >> YDir.z;
    ifs >> NZ >> ZDir.x >> ZDir.y >> ZDir.z;
    if( ! ifs ){
        return(ThrowError("name","unable to read header data"));
    }

    if( (XDir.x == 0.0) || (XDir.y != 0) || (XDir.z != 0) ){
        return(ThrowError("name","X-axis of box must be aligned with the Cartesian axes"));
    }
    if( (YDir.x != 0.0) || (YDir.y == 0) || (YDir.z != 0) ){
        return(ThrowError("name","Y-axis of box must be aligned with the Cartesian axes"));
    }
    if( (ZDir.x != 0.0) || (ZDir.y != 0) || (ZDir.z == 0) ){
        return(ThrowError("name","Z-axis of box must be aligned with the Cartesian axes"));
    }

    // amstrong/bohr conversion
    double BOHR_TO_ANG = 0.5291772109;

    if( natoms < 0 ){
        natoms *= -1;
    } else {
        Origin.x *= BOHR_TO_ANG;
        Origin.y *= BOHR_TO_ANG;
        Origin.z *= BOHR_TO_ANG;
    }

    if( NX < 0 ){
        NX *= -1;
    } else {
        XDir.x *= BOHR_TO_ANG;
        XDir.y *= BOHR_TO_ANG;
        XDir.z *= BOHR_TO_ANG;
    }

    if( NY < 0 ){
        NY *= -1;
    } else {
        YDir.x *= BOHR_TO_ANG;
        YDir.y *= BOHR_TO_ANG;
        YDir.z *= BOHR_TO_ANG;
    }

    if( NZ < 0 ){
        NZ *= -1;
    } else {
        ZDir.x *= BOHR_TO_ANG;
        ZDir.y *= BOHR_TO_ANG;
        ZDir.z *= BOHR_TO_ANG;
    }

    // read structure
    for(int i=0; i < natoms; i++){
        int     nz;
        double  tmp;
        CPoint  pos;
        ifs >> nz >> tmp >> pos.x >> pos.y >> pos.z;
        Coordinates.push_back(pos);
    }
    if( ! ifs ){
        return(ThrowError("name","unable to read structure data"));
    }

    // read data
    int ndata = NX*NY*NZ;
    if( ndata <= 0 ){
        return(ThrowError("name","no volume data, number of voxels is zero"));
    }

    VolumeData.CreateVector(ndata);

    for(int i=0; i < ndata; i++){
        ifs >> VolumeData[i];
    }
    if( ! ifs ){
        return(ThrowError("name","unable to read volume data"));
    }

    ifs.close();

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QVolumeData::getRefCrds(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  TrajPool::getRefCrds(snapshot)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("snapshot",1);
    if( value.isError() ) return(value);

    QSnapshot* p_qref;
    value = GetArgAsObject<QSnapshot*>("snapshot","snapshot","Snapshot",1,p_qref);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( (int)Coordinates.size() != p_qref->Restart.GetNumberOfAtoms() ){
        CSmallString error;
        error << "number of atoms does not match, reference snapshot ("
              << p_qref->Restart.GetNumberOfAtoms() << "), volume data (" << Coordinates.size() << ")";
        return( ThrowError("snapshot",error));
    }

    for(unsigned int i=0; i < Coordinates.size(); i++){
        p_qref->Restart.SetPosition(i,Coordinates[i]);
    }

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QVolumeData::getValue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double TrajPool::getValue(x,y,z)" << endl;
        sout << "       double TrajPool::getValue(point)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("point/x,y,z",1,3);
    if( value.isError() ) return(value);

    CPoint pos;
    if( GetArgumentCount() == 1 ){
        QPoint* p_point;
        value = GetArgAsObject<QPoint*>("point","point","Point",1,p_point);
        if( value.isError() ) return(value);
        pos = p_point->Point;
    } else {
        value = GetArgAsRNumber("x,y,z","x",1,pos.x);
        if( value.isError() ) return(value);
        value = GetArgAsRNumber("x,y,z","y",2,pos.y);
        if( value.isError() ) return(value);
        value = GetArgAsRNumber("x,y,z","z",3,pos.z);
        if( value.isError() ) return(value);
    }

// execute ---------------------------------------
    return( InternalGetValue(pos.x,pos.y,pos.z) );
}

//------------------------------------------------------------------------------

double QVolumeData::InternalGetValue(double x, double y, double z)
{
    // this code assume rectangular grid
    int nx = floor((x - Origin.x) / XDir.x);
    int ny = floor((y - Origin.y) / YDir.y);
    int nz = floor((z - Origin.z) / ZDir.z);

//    printf("%d %d %d\n",nx,ny,nz);

    if( (nx < 0) || (nx+1 >= NX) ) return(OutlierValue);
    if( (ny < 0) || (ny+1 >= NY) ) return(OutlierValue);
    if( (nz < 0) || (nz+1 >= NZ) ) return(OutlierValue);

    // cell values
    double V000 = VolumeData[nx*NY*NZ + ny*NZ + nz];
    double V001 = VolumeData[nx*NY*NZ + ny*NZ + nz+1];
    double V010 = VolumeData[nx*NY*NZ + (ny+1)*NZ + nz];
    double V011 = VolumeData[nx*NY*NZ + (ny+1)*NZ + nz+1];

    double V100 = VolumeData[(nx+1)*NY*NZ + ny*NZ + nz];
    double V101 = VolumeData[(nx+1)*NY*NZ + ny*NZ + nz+1];
    double V110 = VolumeData[(nx+1)*NY*NZ + (ny+1)*NZ + nz];
    double V111 = VolumeData[(nx+1)*NY*NZ + (ny+1)*NZ + nz+1];

    double xd = (x - nx*XDir.x - Origin.x)/XDir.x;
    double yd = (y - ny*YDir.y - Origin.y)/YDir.y;
    double zd = (z - nz*ZDir.z - Origin.z)/ZDir.z;

//    printf("%f %f %f\n",xd,yd,zd);

    // interpolation
    double C00 = V000*(1.0 - xd) + V100*xd;
    double C01 = V001*(1.0 - xd) + V101*xd;
    double C10 = V010*(1.0 - xd) + V110*xd;
    double C11 = V011*(1.0 - xd) + V111*xd;

    double C0 = C00*(1.0 - yd) + C10*yd;
    double C1 = C01*(1.0 - yd) + C11*yd;

    double value = C0*(1.0 - zd) + C1*zd;

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QVolumeData::setOutlierValue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: VolumeData::setOutlierValue(value)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("value",1);
    if( value.isError() ) return(value);

    double outval;
    value = GetArgAsRNumber("value","value",1,outval);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    OutlierValue = outval;
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QVolumeData::getOutlierValue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: VolumeData::getOutlierValue()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(OutlierValue);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

