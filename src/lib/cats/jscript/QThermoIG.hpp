#ifndef QThermoIGH
#define QThermoIGH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2015 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <QObject>
#include <QScriptValue>
#include <QScriptContext>
#include <QScriptable>
#include <Point.hpp>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

/// thermochemistry of ideal gas

class QThermoIG : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QThermoIG(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    /// access coordinates via properties
    Q_PROPERTY(QScriptValue temperature READ getTemperature WRITE setTemperature)
    Q_PROPERTY(QScriptValue pressure READ getPressure WRITE setPressure)


// methods ---------------------------------------------------------------------
public slots:
    /// print info about system
    /// printInfo()
    QScriptValue printInfo(void);

    /// overal system properties
    /// temperature [K] - default 300 K
    QScriptValue getTemperature(void);
    /// pressure [Pa] - default 100000 Pa
    QScriptValue getPressure(void);

    /// set individual system properties
    /// temperature [K]
    QScriptValue setTemperature(const QScriptValue& dummy);
    /// pressure [Pa]
    QScriptValue setPressure(const QScriptValue& dummy);

    /// get translational entropy [cal/mol/K]
    /// double getStran(topology)
    QScriptValue getStran(void);

    /// get rotational entropy [cal/mol/K]
    /// double getSrot(rotnumber,snapshot[,selection])
    QScriptValue getSrot(void);

// access methods --------------------------------------------------------------
public:   
    double  Temperature;  // temperature [K]
    double  Pressure;  // presure [Pa]
};

//------------------------------------------------------------------------------

#endif
