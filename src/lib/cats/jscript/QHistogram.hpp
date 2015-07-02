#ifndef QHistogramH
#define QHistogramH
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
#include <vector>
#include <iostream>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

/// histogram analysis

class QHistogram : public QObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QHistogram(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue name        READ getName WRITE setName)
    Q_PROPERTY(QScriptValue minValue    READ getMinValue WRITE setMinValue)
    Q_PROPERTY(QScriptValue maxValue    READ getMaxValue WRITE setMaxValue)
    Q_PROPERTY(QScriptValue numOfBins   READ getNumOfBins WRITE setNumOfBins)

// methods ---------------------------------------------------------------------
public slots:

// setup methods ---------------------------------------------------------------
    /// set property name
    QScriptValue setName(const QScriptValue& dummy);

    /// get name of property
    QScriptValue getName(void);

    /// set minimum value
    QScriptValue setMinValue(const QScriptValue& dummy);

    /// get minimum value
    QScriptValue getMinValue(void);

    /// set maxium value
    QScriptValue setMaxValue(const QScriptValue& dummy);

    /// get maxium value
    QScriptValue getMaxValue(void);

    /// set number of bins
    QScriptValue setNumOfBins(const QScriptValue& dummy);

    /// get number of bins
    QScriptValue getNumOfBins(void);

    /// print statistics
    QScriptValue printInfo(void);

// data manipulation methods ---------------------------------------------------
    /// add sample
    /// addSample(sample)
    QScriptValue addSample(void);

    /// return number of samples loaded via addSample
    /// double getNumOfSamples()
    QScriptValue getNumOfSamples(void);

    /// return number of samples within min,max interval
    /// double getNumOfSamplesWithin()
    QScriptValue getNumOfSamplesWithin(void);

    /// clear all values
    /// reset()
    QScriptValue reset(void);

    /// mult by factor
    /// multBy(factor)
    QScriptValue multBy(void);

    /// normalize histogram
    /// normalize()
    QScriptValue normalize(void);

    /// transform to radial
    /// transformToRadial()
    QScriptValue transformToRadial(void);

    /// calculate integral within interval from;to rounded to given bins
    /// double getIntegral(from,to)
    QScriptValue getIntegral(void);

// i/o methods -----------------------------------------------------------------
    /// save histogram
    QScriptValue save(void);

// manipulation methods --------------------------------------------------------
public:
    /// alocate arrays
    void allocateInternal(void);

    /// release arrays
    void releaseInternal(void);

    /// reset arrays
    void resetInternal(void);

    /// save histogram
    void save(std::ostream& str);

// section of private data -----------------------------------------------------
private:
    QString             Name;
    double              MinValue;
    double              MaxValue;
    int                 NBins;
    int                 NumOfSamples;
    int                 NumOfSamplesWithin;
    std::vector<double> Histogram;
    std::vector<int>    DataN;
    std::vector<double> DataSum;
    std::vector<double> Data2Sum;
};

//------------------------------------------------------------------------------

#endif

