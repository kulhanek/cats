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

#include <QScriptEngine>
#include <QSimpleVector.hpp>
#include <moc_QSimpleVector.cpp>
#include <SmallString.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QSimpleVector::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QSimpleVector::New);
    QScriptValue metaObject = engine.newQMetaObject(&QSimpleVector::staticMetaObject, ctor);
    engine.globalObject().setProperty("Vector", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QSimpleVector::New(QScriptContext *context,QScriptEngine *engine)
{
    QCATsScriptable scriptable("Vector");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Vector object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new Vector(size)" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("size",1);
    if( value.isError() ) return(value);

    int size = 0;
    value = scriptable.GetArgAsInt("size","size",1,size);
    if( value.isError() ) return(value);

    if( size <= 0 ){
        return( scriptable.ThrowError("size","size must be greater than zero") );
    }

    QSimpleVector* p_obj = new QSimpleVector(size);
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QSimpleVector::QSimpleVector(int size)
    : QCATsScriptable("Vector")
{
    Vector.CreateVector(size);
    Vector.SetZero();
}

//------------------------------------------------------------------------------

QScriptValue QSimpleVector::getDimension(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int Vector::getDimension()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return( (int)Vector.GetLength() );
}

//------------------------------------------------------------------------------

QScriptValue QSimpleVector::getEuclideanLength(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Vector::getEuclideanLength()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------

    double len = 0;
    for(size_t i=0; i < Vector.GetLength(); i++){
        double item = Vector[i];
        len += item*item;
    }
    len = sqrt(len);
    return(len);
}

//------------------------------------------------------------------------------

QScriptValue QSimpleVector::getScalarProduct(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Vector::getScalarProduct(vector)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("vector",1);
    if( value.isError() ) return(value);

    QSimpleVector* p_qleft = NULL;
    value = GetArgAsObject<QSimpleVector*>("size","size","Vector",1,p_qleft);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( Vector.GetLength() != p_qleft->Vector.GetLength() ){
        CSmallString error;
        error << "Vector::getScalarProduct(vector) - dimension mismatch, ";
        error << "right (" << Vector.GetLength() << "), left (" << p_qleft->Vector.GetLength() << ")";
        return( ThrowError("vector",error));
    }

    double dot = 0;
    for(size_t i=0; i < Vector.GetLength(); i++){
        dot += Vector[i]*p_qleft->Vector[i];
    }

    return(dot);
}

//------------------------------------------------------------------------------

QScriptValue QSimpleVector::getAngleWith(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double Vector::getAngleWith(vector)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("vector",1);
    if( value.isError() ) return(value);

    QSimpleVector* p_qleft = NULL;
    value = GetArgAsObject<QSimpleVector*>("size","size","Vector",1,p_qleft);
    if( value.isError() ) return(value);

    if( Vector.GetLength() != p_qleft->Vector.GetLength() ){
        CSmallString error;
        error << "Vector::getAngleWith(vector) - dimension mismatch, ";
        error << "right (" << Vector.GetLength() << "), left (" << p_qleft->Vector.GetLength() << ")";
        return( ThrowError("vector",error));
    }

    double top = 0.0;
    double v1 = 0.0;
    double v2 = 0.0;

    for(size_t i=0; i < Vector.GetLength(); i++){
        double t1 = Vector[i];
        double t2 = p_qleft->Vector[i];
        v1 += t1*t1;
        v2 += t2*t2;
        top += t1*t2;
    }

    v1 = sqrt(v1);
    v2 = sqrt(v2);

    if( v1 == 0.0 ){
        CSmallString error;
        error << "Vector::getAngleWith(vector) - right vector has zero length)";
        context()->throwError(QString(error));
        return( ThrowError("vector",error));
    }
    if( v2 == 0.0 ){
        CSmallString error;
        error << "Vector::getAngleWith(vector) - left vector has zero length)";
        return( ThrowError("vector",error));
    }

    double down = v1*v2;
    double ac = top / down;
    if( ac >  1.0 ) ac = 1.0;
    if( ac < -1.0 ) ac = -1.0;

    double angle = acos(ac);
// TODO - via option
//    angle = angle * 180.0 / M_PI; // convert to degrees

    return(angle);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


