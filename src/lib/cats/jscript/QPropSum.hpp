#ifndef QPropSumH
#define QPropSumH
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

class CATS_PACKAGE QPropSum : public QObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QPropSum(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue name                READ getName WRITE setName)
    Q_PROPERTY(QScriptValue numOfSamples        READ getNumOfSamples WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue average             READ getAverage WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue sigmaOfSamples      READ getSigmaOfSamples WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue sigmaOfAverage      READ getSigmaOfAverage WRITE setIsNotAllowed)

// methods ---------------------------------------------------------------------
public slots:
    /// set property name
    /// setName(string)
    QScriptValue setName(const QScriptValue& dummy);

    /// reset all data
    /// reset()
    QScriptValue reset(void);

    /// add sample to property summation
    /// addSample(double)
    QScriptValue addSample(void);

    /// get number of samples
    /// double getNumOfSamples()
    QScriptValue getNumOfSamples(void);

    /// return average of property
    /// double getAverage()
    QScriptValue getAverage(void);

    /// return standard deviation of property
    /// double getSigmaOfSamples()
    QScriptValue getSigmaOfSamples(void);

    /// return standard deviation of property mean
    /// double getSigmaOfAverage()
    QScriptValue getSigmaOfAverage(void);

    /// get name of property
    /// string getName()
    QScriptValue getName(void);

    /// print statistics
    /// printInfo()
    QScriptValue printInfo(void);

// manipulation methods --------------------------------------------------------
public:
    /// reset all accumulators
    void resetInternal(void);

// section of private data -----------------------------------------------------
private:
    QString     Name;
    int         N;          // number of samples
    double      SX;         // sum of samples
    double      SX2;        // sum of sample squares
};

//------------------------------------------------------------------------------

#endif

