// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QGeometry.hpp>
#include <QGeometry.moc>
#include <QPoint.hpp>
#include <TerminalStr.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QGeometry::Register(QScriptEngine& engine)
{
    QObject* p_obj = new QGeometry;
    QScriptValue objectValue = engine.newQObject(p_obj);
    engine.globalObject().setProperty("geo", objectValue);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QGeometry::QGeometry(void)
    : QCATsScriptable("geo")
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QGeometry::getDistance(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double geo::getDistance(point1,point2)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("point1,point2",2);
    if( value.isError() ) return(value);

    CPoint p1,p2;

    QPoint* p_qpoint;
    value = GetArgAsObject<QPoint*>("point1,point2","point1","Point",1,p_qpoint);
    if( value.isError() ) return(value);
    p1 = p_qpoint->Point;

    value = GetArgAsObject<QPoint*>("point1,point2","point2","Point",2,p_qpoint);
    if( value.isError() ) return(value);
    p2 = p_qpoint->Point;

// execute ---------------------------------------
    return( GetDistance(p1,p2) );
}

//------------------------------------------------------------------------------

QScriptValue QGeometry::getAngle(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double geo::getAngle(point1,point2,point3)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("point1,point2,point3",3);
    if( value.isError() ) return(value);

    CPoint p1,p2,p3;

    QPoint* p_qpoint;
    value = GetArgAsObject<QPoint*>("point1,point2,point3","point1","Point",1,p_qpoint);
    if( value.isError() ) return(value);
    p1 = p_qpoint->Point;

    value = GetArgAsObject<QPoint*>("point1,point2,point3","point2","Point",2,p_qpoint);
    if( value.isError() ) return(value);
    p2 = p_qpoint->Point;

    value = GetArgAsObject<QPoint*>("point1,point2,point3","point3","Point",3,p_qpoint);
    if( value.isError() ) return(value);
    p3 = p_qpoint->Point;

    return( GetAngle(p1,p2,p3) );
}

//------------------------------------------------------------------------------

QScriptValue QGeometry::getDihedral(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double geo::getAngle(point1,point2,point3,point4)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("point1,point2,point3,point4",4);
    if( value.isError() ) return(value);

    CPoint p1,p2,p3,p4;

    QPoint* p_qpoint;
    value = GetArgAsObject<QPoint*>("point1,point2,point3,point4","point1","Point",1,p_qpoint);
    if( value.isError() ) return(value);
    p1 = p_qpoint->Point;

    value = GetArgAsObject<QPoint*>("point1,point2,point3,point4","point2","Point",2,p_qpoint);
    if( value.isError() ) return(value);
    p2 = p_qpoint->Point;

    value = GetArgAsObject<QPoint*>("point1,point2,point3,point4","point3","Point",3,p_qpoint);
    if( value.isError() ) return(value);
    p3 = p_qpoint->Point;

    value = GetArgAsObject<QPoint*>("point1,point2,point3,point4","point4","Point",4,p_qpoint);
    if( value.isError() ) return(value);
    p3 = p_qpoint->Point;

    return( GetDihedral(p1,p2,p3,p4) );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

double QGeometry::GetDistance(const CPoint& p1,const CPoint& p2)
{
    return(Size(p1-p2));
}

//------------------------------------------------------------------------------

double QGeometry::GetAngle(const CPoint& p1,const CPoint& p2,const CPoint& p3)
{
    CPoint v1,v2;
    v1 = p1-p2;
    v2 = p3-p2;

    double top = VectDot(v1,v2);
    double down = Size(v1)*Size(v2);

    if( down < 0.1e-7 ) return(0.0);
    double ac = top / down;
    if( ac >  1.0 ) ac = 1.0;
    if( ac < -1.0 ) ac = -1.0;

    double angle = acos(ac);
    angle = angle * 180.0 / M_PI; // convert to degrees

    return(angle);
}

//------------------------------------------------------------------------------

#define SIGN(a,b)  ((b >= 0.0) ? (a) : -(a))

double QGeometry::GetDihedral(const CPoint& p1,const CPoint& p2,const CPoint& p3,const CPoint& p4)
{
    float iax ,iay ,iaz ,ibx ,iby ,ibz ;
    float icx ,icy ,icz ,idx ,idy ,idz ;

    iax = p1.x;
    iay = p1.y;
    iaz = p1.z;

    ibx =p2.x;
    iby = p2.y;
    ibz = p2.z;

    icx = p3.x;
    icy = p3.y;
    icz = p3.z;

    idx = p4.x;
    idy = p4.y;
    idz = p4.z;

    float eabx,eaby,eabz;
    float ebcx,ebcy,ebcz;
    float ecdx,ecdy,ecdz;
    float abbcx,abbcy,abbcz;
    float dccbx,dccby,dccbz;
    float abcdx,abcdy,abcdz,signum;
    float cosdel,xrcd;
    float rab,rbc,rcd,xrbc,xrab,cosb,phib,xsinb,cosc,phic,xsinc;

    //       bond lengths and unit vectors

    eabx = ibx - iax;
    eaby = iby - iay;
    eabz = ibz - iaz;

    rab = sqrt (eabx * eabx + eaby * eaby + eabz * eabz);
    xrab = 1.0 / rab;
    eabx = eabx * xrab;
    eaby = eaby * xrab;
    eabz = eabz * xrab;
    ebcx = icx - ibx;
    ebcy = icy - iby;
    ebcz = icz - ibz;

    rbc = sqrt (ebcx * ebcx + ebcy * ebcy + ebcz * ebcz);
    xrbc = 1.0 / rbc;
    ebcx = ebcx * xrbc;
    ebcy = ebcy * xrbc;
    ebcz = ebcz * xrbc;
    ecdx = idx - icx;
    ecdy = idy - icy;
    ecdz = idz - icz;

    rcd = sqrt (ecdx * ecdx + ecdy * ecdy + ecdz * ecdz);
    xrcd = 1.0 / rcd;
    ecdx = ecdx * xrcd;
    ecdy = ecdy * xrcd;
    ecdz = ecdz * xrcd;

    //       cross and dot products between unit vectors, and bond (!)
    //       angles

    abbcx = eaby * ebcz - eabz * ebcy;
    abbcy = eabz * ebcx - eabx * ebcz;
    abbcz = eabx * ebcy - eaby * ebcx;
    cosb = - (eabx * ebcx + eaby * ebcy + eabz * ebcz);
    phib = acos(cosb);
    xsinb = 1.0 / sin(phib);
    dccbx = ecdy * ebcz - ecdz * ebcy;
    dccby = ecdz * ebcx - ecdx * ebcz;
    dccbz = ecdx * ebcy - ecdy * ebcx;
    cosc = - (ecdx * ebcx + ecdy * ebcy + ecdz * ebcz);
    phic = acos(cosc);
    xsinc = 1.0 / sin(phic);

    //        torsional angle

    abcdx = - ( abbcy * dccbz - abbcz * dccby );
    abcdy = - ( abbcz * dccbx - abbcx * dccbz );
    abcdz = - ( abbcx * dccby - abbcy * dccbx );
    signum = SIGN(1.0,
                  (abcdx * ebcx + abcdy * ebcy + abcdz * ebcz) );
    cosdel = - (abbcx * dccbx + abbcy * dccby + abbcz * dccbz)
             *  xsinb * xsinc;

    if(cosdel < -1.0) cosdel = -1.0;
    if(cosdel >  1.0) cosdel =  1.0;

    float  angle = signum * acos (cosdel);

    return(angle);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

