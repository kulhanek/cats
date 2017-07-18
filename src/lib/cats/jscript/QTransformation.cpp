// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
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
#include <QScriptEngine>
#include <QTransformation.hpp>
#include <moc_QTransformation.cpp>
#include <QSnapshot.hpp>
#include <QSelection.hpp>
#include <AmberAtom.hpp>
#include <QSimpleVector.hpp>
#include <QPoint.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QTransformation::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QTransformation::New);
    QScriptValue metaObject = engine.newQMetaObject(&QTransformation::staticMetaObject, ctor);
    engine.globalObject().setProperty("Transformation", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QTransformation::New(QScriptContext *context,
                                  QScriptEngine *engine)
{
    QCATsScriptable scriptable("Transformation");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Transformation object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new Transformation()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    QTransformation* p_obj = new QTransformation();
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QTransformation::QTransformation(void)
    : QCATsScriptable("Transformation")
{
    Trans.SetUnit();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QTransformation::identity(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Transformation::identity()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Trans.SetUnit();
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QTransformation::apply(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  Transformation::apply(snapshot/point/vector)" << endl;
        return(false);
    }

// check arguments and execute  -------------------------------
    value = CheckNumberOfArguments("snapshot/point/vector",1);
    if( value.isError() ) return(value);

    if( IsArgumentObject<QSnapshot*>(1) == true ){
        QSnapshot* p_qsnap;
        value = GetArgAsObject<QSnapshot*>("snapshot/point/vector","snapshot","Snapshot",1,p_qsnap);
        if( value.isError() ) return(value);

        // transform snapshot
        CPoint zero;
        for(int i=0; i < p_qsnap->Restart.GetNumberOfAtoms(); i++) {
            CPoint tpos = p_qsnap->Restart.GetPosition(i);
            Trans.Apply(tpos);
            p_qsnap->Restart.SetPosition(i,tpos);
            if( p_qsnap->Restart.AreVelocitiesLoaded() ){
                p_qsnap->Restart.SetVelocity(i,zero);
            }
        }
        return(value);
    }

    if( IsArgumentObject<QPoint*>(1) == true ){
        QPoint* p_qpoint;
        value = GetArgAsObject<QPoint*>("snapshot/point/vector","point","Point",1,p_qpoint);
        if( value.isError() ) return(value);

        Trans.Apply(p_qpoint->Point);
        return(value);
    }

    if( IsArgumentObject<QSimpleVector*>(1) == true ){
        QSimpleVector* p_qvect;
        value = GetArgAsObject<QSimpleVector*>("snapshot/point/vector","vector","Vector",1,p_qvect);
        if( value.isError() ) return(value);

        double* p_data = p_qvect->Vector.GetRawDataField();
        int len = p_qvect->Vector.GetLength();
        while( len > 0 ){
            CPoint pos;
            pos.x = p_data[0];
            pos.y = p_data[1];
            pos.z = p_data[2];
            Trans.Apply(pos);
            p_data[0] = pos.x;
            p_data[1] = pos.y;
            p_data[2] = pos.z;
            p_data += 3;
            len -= 3;
        }
        return(value);
    }

    return( ThrowError("point/snapshot/vector","unsupported argument"));
}

//------------------------------------------------------------------------------

QScriptValue QTransformation::translate(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Transformation::translate(x,y,z)" << endl;
        sout << "       Transformation::translate(point)" << endl;
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
    Trans.Translate(pos.x,pos.y,pos.z);
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QTransformation::rotate(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Transformation::rotate(angle,x,y,z)" << endl;
        sout << "       Transformation::rotate(angle,point)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("angle,point/x,y,z",2,4);
    if( value.isError() ) return(value);

    double angle;
    value = GetArgAsRNumber("angle,point/x,y,z","angle",1,angle);
    if( value.isError() ) return(value);

    CPoint pos;
    if( GetArgumentCount() == 2 ){
        QPoint* p_point;
        value = GetArgAsObject<QPoint*>("angle,point","point","Point",2,p_point);
        if( value.isError() ) return(value);
        pos = p_point->Point;
    } else {
        value = GetArgAsRNumber("angle,x,y,z","x",3,pos.x);
        if( value.isError() ) return(value);
        value = GetArgAsRNumber("angle,x,y,z","y",4,pos.y);
        if( value.isError() ) return(value);
        value = GetArgAsRNumber("angle,x,y,z","z",5,pos.z);
        if( value.isError() ) return(value);
    }

// execute ---------------------------------------
    Trans.Rotate(angle,pos.x,pos.y,pos.z);
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QTransformation::changeDirection(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Transformation::changeDirection(pointFrom,pointTo)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("pointFrom,pointTo",2);
    if( value.isError() ) return(value);

    QPoint* p_point1;
    value = GetArgAsObject<QPoint*>("pointFrom,pointTo","pointFrom","Point",1,p_point1);
    if( value.isError() ) return(value);

    QPoint* p_point2;
    value = GetArgAsObject<QPoint*>("pointFrom,pointTo","pointTo","Point",2,p_point2);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Trans.ChangeDirection(p_point1->Point,p_point2->Point);
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QTransformation::rmsdFit(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Transformation::rmsdFit(refsnap,targsnap,key1,...)" << endl;
        sout << "       Transformation::rmsdFit(refsnap,targsnap,refsel,targsel,key1,...)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckMinimumNumberOfArguments("refsnap,targsnap[,refsel,targsel,key1,...]",2);
    if( value.isError() ) return(value);

    QSnapshot* p_qref;
    value = GetArgAsObject<QSnapshot*>("refsnap,targsnap[,refsel,targsel]","refsnap","Snapshot",1,p_qref);
    if( value.isError() ) return(value);

    QSnapshot* p_qsnap;
    value = GetArgAsObject<QSnapshot*>("refsnap,targsnap[,refsel,targsel]","targsnap","Snapshot",2,p_qsnap);
    if( value.isError() ) return(value);

    QSelection* p_qrefsel = NULL;
    QSelection* p_qsnapsel = NULL;
    if( GetArgumentCount() > 2 ){
        value = GetArgAsObject<QSelection*>("refsnap,targsnap,refsel,targsel","refsel","Selection",3,p_qrefsel);
        if( value.isError() ) return(value);
        if( p_qrefsel->Mask.GetTopology() != p_qref->Restart.GetTopology() ){
            return( ThrowError("refsnap,targsnap,refsel,targsel","reference snapshot and selection do not have the same topology") );
        }

        value = GetArgAsObject<QSelection*>("refsnap,targsnap,refsel,targsel","targsel","Selection",4,p_qsnapsel);
        if( value.isError() ) return(value);
        if( p_qsnapsel->Mask.GetTopology() != p_qsnap->Restart.GetTopology() ){
            return( ThrowError("refsnap,targsnap,refsel,targsel","target snapshot and selection do not have the same topology") );
        }
    }

    bool nomass = IsArgumentKeySelected("nomass");
    value = CheckArgumentsUsage("refsnap,targsnap[,refsel,targsel,key1,...]");
    if( value.isError() ) return(value);

// execute ---------------------------------------

    if( p_qrefsel ){
        if( p_qrefsel->Mask.GetNumberOfSelectedAtoms() != p_qsnapsel->Mask.GetNumberOfSelectedAtoms() ){
            CSmallString error;
            error << "number of atoms does not match, reference selection ("
                  << p_qrefsel->Mask.GetNumberOfSelectedAtoms() << "), target (" << p_qsnapsel->Mask.GetNumberOfSelectedAtoms()  << ")";
            return( ThrowError("refsnap,targsnap,refsel,targsel",QString(error)) );
        }
    } else {
        if( p_qref->Restart.GetNumberOfAtoms() != p_qsnap->Restart.GetNumberOfAtoms() ){
            CSmallString error;
            error << "number of atoms does not match, reference snapshot ("
                  << p_qref->Restart.GetNumberOfAtoms() << "), target (" << p_qsnap->Restart.GetNumberOfAtoms() << ")";
            return( ThrowError("refsnap,targsnap",QString(error)) );
        }
    }

    // get COMs
    CPoint com1;
    CPoint com2;
    double  totalmass1 = 0.0;
    double  totalmass2 = 0.0;

    if( p_qrefsel ){
        for(int s=0; s < p_qrefsel->Mask.GetNumberOfSelectedAtoms(); s++) {
            int i = p_qrefsel->Mask.GetSelectedAtomCondensed(s)->GetAtomIndex();
            int j = p_qsnapsel->Mask.GetSelectedAtomCondensed(s)->GetAtomIndex();
            CPoint rpos = p_qref->Restart.GetPosition(i);
            CPoint tpos = p_qsnap->Restart.GetPosition(j);
            double tmass;
            double rmass;
            if( nomass ){
                tmass = 1.0;
                rmass = 1.0;
            } else {
                rmass = p_qref->Restart.GetMass(i);
                tmass = p_qsnap->Restart.GetMass(j);
            }
            com1 += rpos*rmass;
            totalmass1 += rmass;

            com2 += tpos*tmass;
            totalmass2 += tmass;
        }
    } else {
        for(int i=0; i < p_qsnap->Restart.GetNumberOfAtoms(); i++) {
            CPoint tpos = p_qsnap->Restart.GetPosition(i);
            CPoint rpos = p_qref->Restart.GetPosition(i);
            double tmass;
            double rmass;
            if( nomass ){
                tmass = 1.0;
                rmass = 1.0;
            } else {
                tmass = p_qsnap->Restart.GetMass(i);
                rmass = p_qref->Restart.GetMass(i);
            }
            com1 += rpos*rmass;
            totalmass1 += rmass;

            com2 += tpos*tmass;
            totalmass2 += tmass;
        }
    }

    if( fabs(totalmass1-totalmass2) > 0.1 ){
        CSmallString error;
        error << "masses do not match, reference ("
              << CSmallString().DoubleToStr(totalmass2) << "), target (" << CSmallString().DoubleToStr(totalmass1) << ")";
        return( ThrowError("refsnap,targsnap[,refsel,targsel]",QString(error)) );
    }

    if( totalmass1 > 0 ){
        com1 = com1 / totalmass1;
    }
    if( totalmass2 > 0 ){
        com2 = com2 / totalmass2;
    }

    if( totalmass1 < 0.01 ){
        CSmallString error;
        error << "no atoms to fit, total mass is zero";
        return( ThrowError("refsnap,targsnap[,refsel,targsel]",QString(error)) );
    }

    // get the transformation matrix
    CSimpleSquareMatrix<double,4>   helper;
    double       rx,ry,rz;
    double       fx,fy,fz;
    double       xxyx, xxyy, xxyz;
    double       xyyx, xyyy, xyyz;
    double       xzyx, xzyy, xzyz;
    double       q[4];

// generate the upper triangle of the quadratic form matrix
    xxyx = 0.0;
    xxyy = 0.0;
    xxyz = 0.0;
    xyyx = 0.0;
    xyyy = 0.0;
    xyyz = 0.0;
    xzyx = 0.0;
    xzyy = 0.0;
    xzyz = 0.0;

    if( p_qrefsel ){
        for(int s=0; s < p_qrefsel->Mask.GetNumberOfSelectedAtoms(); s++) {
            int i = p_qrefsel->Mask.GetSelectedAtomCondensed(s)->GetAtomIndex();
            int j = p_qsnapsel->Mask.GetSelectedAtomCondensed(s)->GetAtomIndex();
            CPoint rpos = p_qref->Restart.GetPosition(i);
            CPoint tpos = p_qsnap->Restart.GetPosition(j);
            double mass = 1.0;
            if( nomass ){
                mass = 1.0;
            } else {
                mass = p_qsnap->Restart.GetMass(j);
            }
            fx = mass*(rpos.x - com1.x);
            fy = mass*(rpos.y - com1.y);
            fz = mass*(rpos.z - com1.z);

            rx = mass*(tpos.x - com2.x);
            ry = mass*(tpos.y - com2.y);
            rz = mass*(tpos.z - com2.z);

            xxyx += fx*rx;
            xxyy += fx*ry;
            xxyz += fx*rz;
            xyyx += fy*rx;
            xyyy += fy*ry;
            xyyz += fy*rz;
            xzyx += fz*rx;
            xzyy += fz*ry;
            xzyz += fz*rz;
        }
    } else {
        for(int i=0; i < p_qsnap->Restart.GetNumberOfAtoms(); i++) {
            CPoint tpos = p_qsnap->Restart.GetPosition(i);
            CPoint rpos = p_qref->Restart.GetPosition(i);
            double mass = 1.0;
            if( nomass ){
                mass = 1.0;
            } else {
                mass = p_qsnap->Restart.GetMass(i);
            }
            fx = mass*(rpos.x - com1.x);
            fy = mass*(rpos.y - com1.y);
            fz = mass*(rpos.z - com1.z);

            rx = mass*(tpos.x - com2.x);
            ry = mass*(tpos.y - com2.y);
            rz = mass*(tpos.z - com2.z);

            xxyx += fx*rx;
            xxyy += fx*ry;
            xxyz += fx*rz;
            xyyx += fy*rx;
            xyyy += fy*ry;
            xyyz += fy*rz;
            xzyx += fz*rx;
            xzyy += fz*ry;
            xzyz += fz*rz;
        }
    }

    helper.Field[0][0] = xxyx + xyyy + xzyz;

    helper.Field[0][1] = xzyy - xyyz;
    helper.Field[1][1] = xxyx - xyyy - xzyz;

    helper.Field[0][2] = xxyz - xzyx;
    helper.Field[1][2] = xxyy + xyyx;
    helper.Field[2][2] = xyyy - xzyz - xxyx;

    helper.Field[0][3] = xyyx - xxyy;
    helper.Field[1][3] = xzyx + xxyz;
    helper.Field[2][3] = xyyz + xzyy;
    helper.Field[3][3] = xzyz - xxyx - xyyy;

// complete matrix
    helper.Field[1][0] = helper.Field[0][1];

    helper.Field[2][0] = helper.Field[0][2];
    helper.Field[2][1] = helper.Field[1][2];

    helper.Field[3][0] = helper.Field[0][3];
    helper.Field[3][1] = helper.Field[1][3];
    helper.Field[3][2] = helper.Field[2][3];

// diagonalize helper matrix
    helper.EigenProblem(q);

// extract the desired quaternion
    q[0] = helper.Field[0][3];
    q[1] = helper.Field[1][3];
    q[2] = helper.Field[2][3];
    q[3] = helper.Field[3][3];

// generate the rotation matrix
    helper.SetUnit();

    helper.Field[0][0] = q[0]*q[0] + q[1]*q[1] - q[2]*q[2] - q[3]*q[3];
    helper.Field[1][0] = 2.0 * (q[1] * q[2] - q[0] * q[3]);
    helper.Field[2][0] = 2.0 * (q[1] * q[3] + q[0] * q[2]);

    helper.Field[0][1] = 2.0 * (q[2] * q[1] + q[0] * q[3]);
    helper.Field[1][1] = q[0]*q[0] - q[1]*q[1] + q[2]*q[2] - q[3]*q[3];
    helper.Field[2][1] = 2.0 * (q[2] * q[3] - q[0] * q[1]);

    helper.Field[0][2] = 2.0 * (q[3] * q[1] - q[0] * q[2]);
    helper.Field[1][2] = 2.0 * (q[3] * q[2] + q[0] * q[1]);
    helper.Field[2][2] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];

    Trans.Translate(-com2);
    Trans.MultFromRightWith(helper);
    Trans.Translate(com1);

    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


