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
#include <QLinStat.hpp>
#include <QSelection.hpp>
#include <iomanip>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

#include <QLinStat.moc>
using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QLinStat::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QLinStat::New);
    QScriptValue metaObject = engine.newQMetaObject(&QLinStat::staticMetaObject, ctor);
    engine.globalObject().setProperty("LinStat", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QLinStat::New(QScriptContext *context, QScriptEngine *engine)
{
    QCATsScriptable scriptable("LinStat");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "LinStat object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new LinStat()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    QLinStat* p_obj = new QLinStat();
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QLinStat::QLinStat(void)
    : QCATsScriptable("LinStat")
{
    resetInternal();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QLinStat::setName(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: LinStat::setName(name)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name",1);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name","name",1,name);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Name = name;
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QLinStat::reset(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: LinStat::reset()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    resetInternal();
    return(value);
}

//------------------------------------------------------------------------------

void QLinStat::resetInternal(void)
{
    N = 0;
    SX = 0;
    SX2 = 0;
    SY = 0;
    SY2 = 0;
    SXY = 0;
}

//------------------------------------------------------------------------------

QScriptValue QLinStat::addSample(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: MinMax::addSample(x,y)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("x,y",2);
    if( value.isError() ) return(value);

    double x,y;
    value = GetArgAsRNumber("x,y","x",1,x);
    if( value.isError() ) return(value);
    value = GetArgAsRNumber("x,y","y",2,y);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    N += 1;
    SX += x;
    SX2 += x*x;
    SY += y;
    SY2 += y*y;
    SXY += x*y;
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QLinStat::getNumOfSamples(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int LinStat::getNumOfSamples()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(N);
}

//------------------------------------------------------------------------------

QScriptValue QLinStat::getSlope(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double LinStat::getSlope()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( N < 2 ){
        return( ThrowError("","at least two samples are needed"));
    }

    double top = N*SXY - SX*SY;
    double bottom = N*SX2 - SX*SX;

    if( bottom == 0.0 ){
        return( ThrowError("","unable to determine slope (not enough of independent data?"));
    }

    double result = top / bottom;
    return( result );
}

//------------------------------------------------------------------------------

QScriptValue QLinStat::getYIntercept(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double LinStat::getYIntercept()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( N < 2 ){
        return( ThrowError("","at least two samples are needed"));
    }

    double beta = getSlope().toNumber();
    double result = (SY - beta*SX)/N;
    return( result );
}

//------------------------------------------------------------------------------

QScriptValue QLinStat::getPearsonCoeff(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double LinStat::getPearsonCoeff()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( N < 2 ){
        return( ThrowError("","at least two samples are needed"));
    }

    double top = N*SXY - SX*SY;
    // bottom is always positive if N > 0
    double bottom = (N*SX2 - SX*SX)*(N*SY2 - SY*SY);

    if( bottom == 0.0 ){
        return( ThrowError("","unable to determine R (not enough of independent data?)"));
    }

    double result = top / sqrt(bottom);
    return( result );
}

//------------------------------------------------------------------------------

QScriptValue QLinStat::getName(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string LinStat::getName()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Name);
}

//------------------------------------------------------------------------------

QScriptValue QLinStat::printInfo(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: LinStat::printInfo()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    cout << "=== Linear Regression Model" << endl;
    cout << "# Name: "  << Name.toStdString() <<  endl;
    cout << "# Number of samples   : " << N << endl;
    if( N >= 2 ){
        cout << "# Line slope          : " << getSlope().toNumber() << endl;
        cout << "# Line Y-intercept    : " << getYIntercept().toNumber() << endl;
        cout << "# Pearson correlation : " << getPearsonCoeff().toNumber() << endl;
    } else {
        cout << "# No data to build linear model" << endl;
    }

    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

