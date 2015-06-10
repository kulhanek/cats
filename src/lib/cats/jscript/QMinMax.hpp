#ifndef QMinMaxH
#define QMinMaxH
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

#include <QObject>
#include <QScriptValue>
#include <QScriptContext>
#include <QScriptable>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

/// property summation
class QMinMax : public QObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QMinMax(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue name                    READ getName WRITE setName)
    Q_PROPERTY(QScriptValue numOfSamples            READ getNumOfSamples WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue minValue                READ getMinValue WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue maxValue                READ getMaxValue WRITE setIsNotAllowed)

// methods ---------------------------------------------------------------------
public slots:
    /// set property name
    QScriptValue setName(const QScriptValue& dummy);

    /// reset all data
    QScriptValue reset(void);

    /// add sample to property summation
    QScriptValue addSample(void);

    /// get number of samples
    QScriptValue getNumOfSamples(void);

    /// return minimal value
    QScriptValue getMinValue(void);

    /// return maximum value
    QScriptValue getMaxValue(void);

    /// get name of property
    QScriptValue getName(void);

    /// print statistics
    QScriptValue printInfo(void);

// manipulation methods --------------------------------------------------------
public:
    /// reset all accumulators
    void resetInternal(void);

// section of private data -----------------------------------------------------
private:
    QString     Name;
    double      MinValue;
    double      MaxValue;
    int         N;          // number of samples
};

//------------------------------------------------------------------------------

#endif

