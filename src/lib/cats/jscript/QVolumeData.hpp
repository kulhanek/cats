#ifndef QVolumeDataH
#define QVolumeDataH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Point.hpp>
#include <vector>
#include <string>
#include <SimpleVector.hpp>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

/// volume data

class CATS_PACKAGE QVolumeData : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QVolumeData(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// methods ---------------------------------------------------------------------
public slots:
    /// load data
    /// bool loadCubeFile(name)
    QScriptValue loadCubeFile(void);

    /// get reference coordinates if associated
    /// getRefCrds(snapshot)
    QScriptValue getRefCrds(void);

    /// get value of data point
    /// double getValue(x,y,z)
    /// double getValue(point)
    QScriptValue getValue(void);

    /// set outlier value
    /// setOutlierValue(value)
    QScriptValue setOutlierValue(void);

    /// get outlier value
    /// double getOutlierValue()
    QScriptValue getOutlierValue(void);

// access methods --------------------------------------------------------------
public:
    std::string Comment1;
    std::string Comment2;
    int         NX;
    int         NY;
    int         NZ;
    CPoint      Origin;
    CPoint      XDir;
    CPoint      YDir;
    CPoint      ZDir;
    std::vector<CPoint>     Coordinates;
    CSimpleVector<double>   VolumeData;
    double                  OutlierValue;

    double InternalGetValue(double x, double y, double z);
};

//------------------------------------------------------------------------------

#endif
