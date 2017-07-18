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
#include <QMinMax.hpp>
#include <moc_QMinMax.cpp>
#include <QSelection.hpp>
#include <iomanip>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QMinMax::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QMinMax::New);
    QScriptValue metaObject = engine.newQMetaObject(&QMinMax::staticMetaObject, ctor);
    engine.globalObject().setProperty("MinMax", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QMinMax::New(QScriptContext *context,QScriptEngine *engine)
{
    QCATsScriptable scriptable("MinMax");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "MinMax object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new MinMax()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    QMinMax* p_obj = new QMinMax();
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QMinMax::QMinMax(void)
    : QCATsScriptable("MinMax")
{
    resetInternal();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QMinMax::setName(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: MinMax::setName(name)" << endl;
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

QScriptValue QMinMax::reset(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: MinMax::reset()" << endl;
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

void QMinMax::resetInternal(void)
{
    N = 0;
    MinValue = 0;
    MaxValue = 0;
}

//------------------------------------------------------------------------------

QScriptValue QMinMax::addSample(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: MinMax::addSample(sample)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("sample",1);
    if( value.isError() ) return(value);

    double sample;
    value = GetArgAsRNumber("sample","sample",1,sample);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    N += 1;
    if( N ==  1 ){
        MinValue = sample;
        MaxValue = sample;
    } else {
        if( MinValue > sample ) MinValue = sample;
        if( MaxValue < sample ) MaxValue = sample;
    }

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QMinMax::getNumOfSamples(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int MinMax::getNumOfSamples()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(N);
}

//------------------------------------------------------------------------------

QScriptValue QMinMax::getMinValue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double MinMax::getMinValue()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(MinValue);
}

//------------------------------------------------------------------------------

QScriptValue QMinMax::getMaxValue(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double MinMax::getMaxValue()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(MaxValue);
}

//------------------------------------------------------------------------------

QScriptValue QMinMax::getName(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string MinMax::getName()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Name);
}

//------------------------------------------------------------------------------

QScriptValue QMinMax::printInfo(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string MinMax::printInfo()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    cout << "=== Property" << endl;
    cout << "# Name: "  << Name.toStdString() <<  endl;
    cout << "# Number of samples: " << N << endl;
    cout << "#" << endl;
    cout << "#    Minvalue        Maxvalue    " << endl;
    cout << "# --------------- ---------------" << endl;
    cout << "  " << setw(15) << right << scientific << getMinValue().toNumber();
    cout << " " << setw(15) << getMaxValue().toNumber() << endl;

    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

