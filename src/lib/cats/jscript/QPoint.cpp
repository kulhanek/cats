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
#include <QScriptEngine>
#include <QPoint.hpp>
#include <moc_QPoint.cpp>
#include <Qx3DNA.hpp>
#include <TerminalStr.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QPoint::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QPoint::New);
    QScriptValue metaObject = engine.newQMetaObject(&QPoint::staticMetaObject, ctor);
    engine.globalObject().setProperty("Point", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    QCATsScriptable scriptable("Point");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Point object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new Point()" << endl;
        sout << "   new Point(x,y,z)" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("[x,y,z]",0,3);
    if( value.isError() ) return(value);

    CPoint pos;
    if( scriptable.GetArgumentCount() > 0 ){
        double x,y,z;
        value = scriptable.GetArgAsRNumber("x,y,z","x",1,x);
        if( value.isError() ) return(value);
        value = scriptable.GetArgAsRNumber("x,y,z","y",2,y);
        if( value.isError() ) return(value);
        value = scriptable.GetArgAsRNumber("x,y,z","z",3,z);
        if( value.isError() ) return(value);
        pos.Set(x,y,z);
    }

// create pbject
    QPoint* p_obj = new QPoint();
    p_obj->Point = pos;
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QPoint::QPoint(void)
    : QCATsScriptable("Point")
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QPoint::getX(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Point::getX()" << endl;
        return(0);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(0);

// execute ---------------------------------------
    return(Point.x);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::getY(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Point::getY()" << endl;
        return(0);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(0);

// execute ---------------------------------------
    return(Point.y);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::getZ(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Point::getZ()" << endl;
        return(0);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(0);

// execute ---------------------------------------
    return(Point.z);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::setX(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  Point::setX(x)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("x",1);
    if( value.isError() ) return(value);

    double x;

    value = GetArgAsRNumber("x","x",1,x);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Point.x = x;
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::setY(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  Point::setY(y)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("y",0);
    if( value.isError() ) return(value);

    double y;

    value = GetArgAsRNumber("y","y",1,y);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Point.y = y;
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::setZ(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  Point::setZ(z)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("z",0);
    if( value.isError() ) return(value);

    double z;

    value = GetArgAsRNumber("z","z",1,z);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Point.z = z;
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::set(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  Point::set(x,y,z)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("x,y,z",3);
    if( value.isError() ) return(value);

    double x,y,z;

    value = GetArgAsRNumber("x,y,z","x",1,x);
    if( value.isError() ) return(value);
    value = GetArgAsRNumber("x,y,z","y",2,y);
    if( value.isError() ) return(value);
    value = GetArgAsRNumber("x,y,z","z",3,z);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Point.x = x;
    Point.y = y;
    Point.z = z;

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::setFrom3DNAHelAxis(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Point::setFrom3DNAHelAxis(x3dna,index)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("x3dna,index",2);
    if( value.isError() ) return(value);

    Qx3DNA* p_x3dna;
    GetArgAsObject("x3dna,index","x3dna","x3DNA",1,p_x3dna);
    if( value.isError() ) return(value);

    int index;
    value = GetArgAsInt("x3dna,index","index",2,index);
    if( value.isError() ) return(value);

    int size = p_x3dna->HelAxisVec.size();
    if( (index < 0) || (index >= size) ){
        CSmallString error;
        error << "index " << index << " is out-of-range <0;" << size-1 << ">";
        return( ThrowError("index", error) );
    }

// execute ---------------------------------------
    Point.x = p_x3dna->HelAxisVec[index].Hx;
    Point.y = p_x3dna->HelAxisVec[index].Hy;
    Point.z = p_x3dna->HelAxisVec[index].Hz;

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::add(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: point Point::add(point)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("point",1);
    if( value.isError() ) return(value);

    QPoint* p_point = NULL;
    value = GetArgAsObject<QPoint*>("point","point","Point",1,p_point);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    QPoint* p_res = new QPoint();
    p_res->Point = Point + p_point->Point;
    value = engine()->newQObject(p_res, QScriptEngine::ScriptOwnership);
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::sub(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: point Point::sub(point)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("point",1);
    if( value.isError() ) return(value);

    QPoint* p_point = NULL;
    value = GetArgAsObject<QPoint*>("point","point","Point",1,p_point);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    QPoint* p_res = new QPoint();
    p_res->Point = Point - p_point->Point;
    value = engine()->newQObject(p_res, QScriptEngine::ScriptOwnership);
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::multBy(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: point Point::multBy(fact)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("fact",1);
    if( value.isError() ) return(value);

    double fac = 0;
    value = GetArgAsRNumber("fact","fact",1,fac);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    QPoint* p_res = new QPoint();
    p_res->Point = Point*fac;
    value = engine()->newQObject(p_res, QScriptEngine::ScriptOwnership);
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::normalize(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  Point::normalize()" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Point.Normalize();
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::getNormalVector(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  point Point::getNormalVector()" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    QPoint* p_res = new QPoint();
    p_res->Point = Point;
    p_res->Point.Normalize();
    value = engine()->newQObject(p_res, QScriptEngine::ScriptOwnership);
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QPoint::size(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  double Point::size()" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Size(Point));
}

//------------------------------------------------------------------------------

QScriptValue QPoint::dot(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  double Point::dot(point)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("point",1);
    if( value.isError() ) return(value);

    QPoint* p_point = NULL;
    value = GetArgAsObject<QPoint*>("point","point","Point",1,p_point);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(VectDot(Point,p_point->Point));
}

//------------------------------------------------------------------------------

QScriptValue QPoint::cross(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  point Point::cross(point)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("point",1);
    if( value.isError() ) return(value);

    QPoint* p_point = NULL;
    value = GetArgAsObject<QPoint*>("point","point","Point",1,p_point);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    QPoint* p_res = new QPoint();
    p_res->Point = CrossDot(Point,p_point->Point);
    value = engine()->newQObject(p_res, QScriptEngine::ScriptOwnership);
    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


