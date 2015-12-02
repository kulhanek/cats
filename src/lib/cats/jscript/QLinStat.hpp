#ifndef QLinStatH
#define QLinStatH
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

#include <CATsMainHeader.hpp>
#include <QObject>
#include <QScriptValue>
#include <QScriptContext>
#include <QScriptable>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

/// property summation
class CATS_PACKAGE QLinStat : public QObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QLinStat(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue name                READ getName WRITE setName)
    Q_PROPERTY(QScriptValue numOfSamples        READ getNumOfSamples WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue slope               READ getSlope WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue yIntercept          READ getYIntercept WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue pearsonCoeff        READ getPearsonCoeff WRITE setIsNotAllowed)

// methods ---------------------------------------------------------------------
public slots:
    /// set property name
    /// setName(string)
    QScriptValue setName(const QScriptValue& dummy);

    /// reset all data
    /// reset()
    QScriptValue reset(void);

    /// add sample to linstat
    /// addSample(x,y)
    QScriptValue addSample(void);

    /// get number of samples
    /// double getNumOfSamples()
    QScriptValue getNumOfSamples(void);

    /// return the slope of linear regression model
    /// double getSlope()
    QScriptValue getSlope(void);

    /// return the y-intercept of linear regression model
    /// double getYIntercept()
    QScriptValue getYIntercept(void);

    /// return the sample Pearson correlation coefficient
    /// double getPearsonCoeff()
    QScriptValue getPearsonCoeff(void);

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
    double      SY;         // sum of samples
    double      SY2;        // sum of sample squares
    double      SXY;        // correlation
};

//------------------------------------------------------------------------------

#endif

