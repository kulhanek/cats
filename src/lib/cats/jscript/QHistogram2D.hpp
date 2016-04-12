#ifndef QHistogram2DH
#define QHistogram2DH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2016 Viktor Illík
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
#include <vector>
#include <iostream>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

/// histogram analysis

class CATS_PACKAGE QHistogram2D : public QObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QHistogram2D(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue minValueD1    READ getMinValueD1 WRITE setMinValueD1)
    Q_PROPERTY(QScriptValue maxValueD1    READ getMaxValueD1 WRITE setMaxValueD1)
    Q_PROPERTY(QScriptValue numOfBinsD1   READ getNumOfBinsD1 WRITE setNumOfBinsD1)

    Q_PROPERTY(QScriptValue minValueD2    READ getMinValueD2 WRITE setMinValueD2)
    Q_PROPERTY(QScriptValue maxValueD2    READ getMaxValueD2 WRITE setMaxValueD2)
    Q_PROPERTY(QScriptValue numOfBinsD2   READ getNumOfBinsD2 WRITE setNumOfBinsD2)

// methods ---------------------------------------------------------------------
public slots:

    // setup methods ---------------------------------------------------------------
    /// set property name
    QScriptValue setName(const QScriptValue& dummy);

    /// get name of property
    QScriptValue getName(void);

    /// set minimum value
    QScriptValue setMinValueD1(const QScriptValue& dummy);
    QScriptValue setMinValueD2(const QScriptValue& dummy);

    /// get minimum value
    QScriptValue getMinValueD1(void);
    QScriptValue getMinValueD2(void);

    /// set maxium value
    QScriptValue setMaxValueD1(const QScriptValue& dummy);
    QScriptValue setMaxValueD2(const QScriptValue& dummy);

    /// get maxium value
    QScriptValue getMaxValueD1(void);
    QScriptValue getMaxValueD2(void);

    /// set number of bins
    QScriptValue setNumOfBinsD1(const QScriptValue& dummy);
    QScriptValue setNumOfBinsD2(const QScriptValue& dummy);

    /// get number of bins
    QScriptValue getNumOfBinsD1(void);
    QScriptValue getNumOfBinsD2(void);

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

    /// calculate integral within interval from;to rounded to given bins
    /// double getIntegral(from,to)
    QScriptValue getIntegral(void);

    /// return max value of occupancy
    /// double getMaxOccupancy()
    QScriptValue getMaxOccupancy(void);

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
    double              MinValueD1, MinValueD2;
    double              MaxValueD1, MaxValueD2;
    int                 NBinsD1, NBinsD2, NBins;
    double              BinSizeD1, BinSizeD2, BinSize;
    int                 NumOfSamples;
    int                 NumOfSamplesWithin;

    std::vector<int> Histogram;
    std::vector<double>    DataN;
    std::vector<double> DataSum;
    std::vector<double> Data2Sum;
};

//------------------------------------------------------------------------------

#endif

