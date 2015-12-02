#ifndef QTransformationH
#define QTransformationH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
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

#include <QObject>
#include <QScriptValue>
#include <QScriptable>
#include <Transformation.hpp>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

/// transformation matrix

class CATS_PACKAGE QTransformation : public QObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QTransformation(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// methods ---------------------------------------------------------------------
public slots:
    /// create identity matrix
    /// identity()
    QScriptValue identity(void);

    /// apply transformation
    /// apply(point)
    /// apply(vector)
    /// apply(snapshot)
    QScriptValue apply(void);

// transformation methods ------------------------------------------------------
    /// translate by vector
    /// translate(x,y,z)
    /// translate(point)
    QScriptValue translate(void);

    /// rotate around vector
    /// rotate(angle,x,y,z)
    /// rotate(angle,point)
    QScriptValue rotate(void);

    /// change direction from one vector to another
    /// changeDirection(pointFrom,pointTo)
    QScriptValue changeDirection(void);

    /// fit two snapshots by rmsd
    /// rmsdFit(snapshot1,snapshot2,selection1,selection2)
    QScriptValue rmsdFit(void);

// section of private data -----------------------------------------------------
private:
    CTransformation Trans;
};

//------------------------------------------------------------------------------

#endif
