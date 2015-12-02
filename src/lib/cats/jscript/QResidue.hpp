#ifndef QResidueH
#define QResidueH
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
#include <QTopology.hpp>
#include <QScriptValue>
#include <QScriptable>
#include <AmberResidue.hpp>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

/// Amber Residue
class CATS_PACKAGE QResidue : public QTopologyObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QResidue(const QScriptValue& top);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue topology                READ getTopology WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue index                   READ getIndex WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue name                    READ getName WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue numOfAtoms              READ getNumOfAtoms WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue firstAtom               READ getFirstAtom WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue firstAtomIndex          READ getFirstAtomIndex WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue numOfNeighbourResidues  READ getNumOfNeighbourResidues WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue nextResidue             READ getNextResidue WRITE setIsNotAllowed)

// methods ---------------------------------------------------------------------
public slots:
    /// get topology
    QScriptValue getTopology(void);

    /// get residue index counting from zero
    QScriptValue getIndex(void);

    /// get resieud name
    QScriptValue getName(void);

    /// get number of atoms
    QScriptValue getNumOfAtoms(void);

    /// get atom
    /// int getAtom(index)
    QScriptValue getAtom(void);

    /// get first atom
    QScriptValue getFirstAtom(void);

    /// get first atom index
    QScriptValue getFirstAtomIndex(void);

    /// get number of neighbour residues
    QScriptValue getNumOfNeighbourResidues(void);

    /// get neighbour residue
    /// residue getNeighbourResidue(index)
    QScriptValue getNeighbourResidue(void);

    /// get neighbour residue
    /// int getNeighbourResidueIndex(index)
    QScriptValue getNeighbourResidueIndex(void);

    /// get next residue in topology
    /// residue getNextResidue()
    QScriptValue getNextResidue(void);

    /// compare objects
    /// bool isSameAs(residue)
    QScriptValue isSameAs(void);

// access methods --------------------------------------------------------------
public:   
    CAmberResidue*     Residue;
};

//------------------------------------------------------------------------------

#endif
