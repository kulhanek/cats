#ifndef QAtomH
#define QAtomH
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

#include <QTopology.hpp>
#include <QScriptValue>
#include <QScriptContext>
#include <QScriptable>
#include <AmberAtom.hpp>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

/// Amber Atom
class QAtom : public QTopologyObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QAtom(const QScriptValue& top);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue topology            READ getTopology WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue index               READ getIndex WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue name                READ getName WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue residue             READ getResidue WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue resIndex            READ getResIndex WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue resName             READ getResName WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue numOfNeighbourAtoms READ getNumOfNeighbourAtoms WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue nextAtom            READ getNextAtom WRITE setIsNotAllowed)

// methods ---------------------------------------------------------------------
public slots:
    /// get topology
    /// topology getTopology()
    QScriptValue getTopology(void);

    /// get atom index counting from zero
    /// int getIndex()
    QScriptValue getIndex(void);

    /// get atom name
    /// string getName()
    QScriptValue getName(void);

    /// get residue
    /// residue getResidue()
    QScriptValue getResidue(void);

    /// get residue index
    /// int getResIndex()
    QScriptValue getResIndex(void);

    /// get residue name
    /// string getResName()
    QScriptValue getResName(void);

    /// get number of neighbour atoms
    /// int getNumOfNeighbourAtoms()
    QScriptValue getNumOfNeighbourAtoms(void);

    /// get neighbour atom
    /// atom getNeighbourAtom(index)
    QScriptValue getNeighbourAtom(void);

    /// get neighbour atom
    /// int getNeighbourAtomIndex(index)
    QScriptValue getNeighbourAtomIndex(void);

    /// is bonded with?
    /// bool isBondedWith(atom)
    QScriptValue isBondedWith(void);

    /// get next atom in topology
    /// atom getNextAtom()
    QScriptValue getNextAtom(void);

    /// compare objects
    /// bool isSameAs(atom)
    QScriptValue isSameAs(void);

// access methods --------------------------------------------------------------
public:   
    CAmberAtom*     Atom;
};

//------------------------------------------------------------------------------

#endif
