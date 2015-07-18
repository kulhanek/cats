#ifndef QPointH
#define QPointH
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

#include <QObject>
#include <QScriptValue>
#include <QScriptContext>
#include <QScriptable>
#include <Point.hpp>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

/// 3D point

class QPoint : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QPoint(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    /// access coordinates via properties
    Q_PROPERTY(QScriptValue x READ getX WRITE setX)
    Q_PROPERTY(QScriptValue y READ getY WRITE setY)
    Q_PROPERTY(QScriptValue z READ getZ WRITE setZ)

// methods ---------------------------------------------------------------------
public slots:
    /// get individual coordinates
    QScriptValue getX(void);
    QScriptValue getY(void);
    QScriptValue getZ(void);

    /// set individual coordinates
    QScriptValue setX(const QScriptValue& dummy);
    QScriptValue setY(const QScriptValue& dummy);
    QScriptValue setZ(const QScriptValue& dummy);

    /// set all
    QScriptValue set(void);

    /// add point
    /// add(point)
    QScriptValue add(void);

    /// mult by constant
    /// multBy(fact)
    QScriptValue multBy(void);

// access methods --------------------------------------------------------------
public:   
    CPoint  Point;
};

//------------------------------------------------------------------------------

#endif
