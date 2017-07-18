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
#include <QPropSum.hpp>
#include <moc_QPropSum.cpp>
#include <QSelection.hpp>
#include <iomanip>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QPropSum::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QPropSum::New);
    QScriptValue metaObject = engine.newQMetaObject(&QPropSum::staticMetaObject, ctor);
    engine.globalObject().setProperty("PropSum", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QPropSum::New(QScriptContext *context,QScriptEngine *engine)
{
    QCATsScriptable scriptable("MinMax");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "PropSum object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new PropSum()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    QPropSum* p_obj = new QPropSum();
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QPropSum::QPropSum(void)
    : QCATsScriptable("PropSum")
{
    resetInternal();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QPropSum::setName(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: PropSum::setName(name)" << endl;
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

QScriptValue QPropSum::reset(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: PropSum::reset()" << endl;
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

void QPropSum::resetInternal(void)
{
    N = 0;
    SX = 0;
    SX2 = 0;
}

//------------------------------------------------------------------------------

QScriptValue QPropSum::addSample(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: PropSum::addSample(sample)" << endl;
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
    SX += sample;
    SX2 += sample*sample;
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QPropSum::getNumOfSamples(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int PropSum::getNumOfSamples()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(N);
}

//------------------------------------------------------------------------------

QScriptValue QPropSum::getAverage(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double PropSum::getAverage()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( N == 0 ) return(0.0);
    double ave = SX / N;
    return(ave);
}

//------------------------------------------------------------------------------

QScriptValue QPropSum::getSigmaOfSamples(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double PropSum::getSigmaOfSamples()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( N == 0 ) return(0.0);
    double ms = N*SX2 - SX*SX;
    double rmsd = sqrt(ms) / N;
    return(rmsd);
}

//------------------------------------------------------------------------------

QScriptValue QPropSum::getSigmaOfAverage(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double PropSum::getSigmaOfAverage()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( N == 0 ) return(0.0);
    double ms = N*SX2 - SX*SX;
    double rmsd = sqrt(ms) / N;
    double sigma = rmsd / sqrt(N);
    return(sigma);
}

//------------------------------------------------------------------------------

QScriptValue QPropSum::getName(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string PropSum::getName()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Name);
}

//------------------------------------------------------------------------------

QScriptValue QPropSum::printInfo(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: PropSum::printInfo()" << endl;
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
    cout << "#     <P>            sigma(P)        sigma(<P>)  " << endl;
    cout << "# --------------- --------------- ---------------" << endl;
    cout << "  " << setw(15) << right << scientific << getAverage().toNumber();
    cout << " " << setw(15) << getSigmaOfSamples().toNumber() << " " << setw(15) << getSigmaOfAverage().toNumber() << endl;

    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

