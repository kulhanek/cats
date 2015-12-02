#ifndef QGeometryH
#define QGeometryH
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
#include <QScriptable>
#include <Point.hpp>
#include <SmallString.hpp>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

class CATS_PACKAGE QGeometry : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QGeometry(void);
    static void Register(QScriptEngine& engine);

// methods ---------------------------------------------------------------------
public slots:
    /// return distance between COMs of two points
    /// double getDistance(point1,point2)
    QScriptValue getDistance(void);

    /// return angle among COMs of three points
    /// double getAngle(point1,point2,point4)
    QScriptValue getAngle(void);

    /// return angle among COMs of fours points
    /// double getDihedral(point1,point2,point3,point4)
    QScriptValue getDihedral(void);

// section of private data -----------------------------------------------------
private:
    /// get distance
    double GetDistance(const CPoint& p1,const CPoint& p2);

    /// get angle
    double GetAngle(const CPoint& p1,const CPoint& p2,const CPoint& p3);

    /// get dihedral
    double GetDihedral(const CPoint& p1,const CPoint& p2,const CPoint& p3,const CPoint& p4);
};

//------------------------------------------------------------------------------

#endif

