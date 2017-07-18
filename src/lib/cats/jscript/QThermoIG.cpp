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
#include <QThermoIG.hpp>
#include <moc_QThermoIG.cpp>
#include <TerminalStr.hpp>
#include <QTopology.hpp>

using namespace std;

//------------------------------------------------------------------------------

// constants
const double Rgas =  8.3144621;         // J/K/mol
const double cal2J = 4.18400;           // 1 J/cal
const double hplanck = 6.62606957e-34;  // J.s
const double kboltz = 1.3806488e-23;    // J/K
const double Na = 6.022140857e23;       // avogadro constant

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QThermoIG::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QThermoIG::New);
    QScriptValue metaObject = engine.newQMetaObject(&QThermoIG::staticMetaObject, ctor);
    engine.globalObject().setProperty("ThermoIG", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QThermoIG::New(QScriptContext *context,
                         QScriptEngine *engine)
{
    QCATsScriptable scriptable("ThermoIG");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Thermodynamics of ideal gas" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new ThermoIG()" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// create pbject
    QThermoIG* p_obj = new QThermoIG();
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QThermoIG::QThermoIG(void)
    : QCATsScriptable("ThermoIG")
{
    Temperature = 300.0;
    Pressure = 100000;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QThermoIG::printInfo(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: ThermoIG::printInfo()" << endl;
        return(0);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(0);

// execute ---------------------------------------
    // TODO
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QThermoIG::getTemperature(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double ThermoIG::getTemperature()" << endl;
        return(0);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(0);

// execute ---------------------------------------
    return(Temperature);
}

//------------------------------------------------------------------------------

QScriptValue QThermoIG::getPressure(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: double ThermoIG::getPressure()" << endl;
        return(0);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(0);

// execute ---------------------------------------
    return(Pressure);
}

//------------------------------------------------------------------------------

QScriptValue QThermoIG::setTemperature(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  ThermoIG::setTemperature(t)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("t",1);
    if( value.isError() ) return(value);

    double t;

    value = GetArgAsRNumber("t","t",1,t);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Temperature = t;
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QThermoIG::setPressure(const QScriptValue& dummy)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  ThermoIG::setPressure(p)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("p",1);
    if( value.isError() ) return(value);

    double p;

    value = GetArgAsRNumber("p","p",1,p);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Pressure = p;
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QThermoIG::getStran(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  ThermoIG::getStran(topology)" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("topology",1);
    if( value.isError() ) return(value);

    QTopology* p_qtop;
    value = GetArgAsObject<QTopology*>("topology","topology","Topology",1,p_qtop);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    double mass = 0.0;
    for(int i=0; i < p_qtop->Topology.AtomList.GetNumberOfAtoms();i++){
        mass += p_qtop->Topology.AtomList.GetAtom(i)->GetMass();
    }
    mass = mass / (Na*1000.0);      // mass of particle

    double qt = pow((2.0*M_PI*mass*kboltz*Temperature)/(hplanck*hplanck),3.0/2.0)*kboltz*Temperature/Pressure;
    double st = 5.0/2.2*Rgas + Rgas*log(qt);
    st = st / cal2J;
    return(st);
}

//------------------------------------------------------------------------------

QScriptValue QThermoIG::getSrot(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage:  ThermoIG::getSrot(rotnum,snapshot[,selection])" << endl;
        return(value);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("topology",1);
    if( value.isError() ) return(value);

    QTopology* p_qtop;
    value = GetArgAsObject<QTopology*>("topology","topology","Topology",1,p_qtop);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


