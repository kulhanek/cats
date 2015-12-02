#ifndef QSimpleVectorH
#define QSimpleVectorH
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

#include <CATsMainHeader.hpp>
#include <QObject>
#include <QScriptValue>
#include <QScriptable>
#include <Vector.hpp>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

/// vector

class CATS_PACKAGE QSimpleVector : public QObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QSimpleVector(int size);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue dimension       READ getDimension WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue euclideanLength READ getEuclideanLength WRITE setIsNotAllowed)

// methods ---------------------------------------------------------------------
public slots:
    /// get vector dimension
    /// int getDimension()
    QScriptValue getDimension(void);

    /// get vector length
    /// double getEuclideanLength()
    QScriptValue getEuclideanLength(void);

    /// get scalar product
    /// double getScalarProduct(vector)
    QScriptValue getScalarProduct(void);

    /// get angle with another vector
    /// double getAngleWith(vector) - returned value is rad
    QScriptValue getAngleWith(void);

// access methods --------------------------------------------------------------
public:   
    CVector Vector;
};

//------------------------------------------------------------------------------

#endif
