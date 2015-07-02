#ifndef QRSelectionH
#define QRSelectionH
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
#include <QCATsScriptable.hpp>
#include <QTopology.hpp>

#include <AmberMaskResidues.hpp>
#include <vector>

// -----------------------------------------------------------------------------

class QTopology;
class QSnapshot;

//------------------------------------------------------------------------------

/// select group of atoms and provide their properties

class QRSelection : public QTopologyObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QRSelection(const QScriptValue& top);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue numOfSelectedResidues   READ getNumOfSelectedResidues WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue mask                    READ getMask WRITE setIsNotAllowed)

// methods ---------------------------------------------------------------------
public slots:
    /// get topology
    QScriptValue getTopology(void);

    /// init selection by ASL mask
    /// setByMask(mask)
    /// setByMask(mask,snapshot)
    QScriptValue setByMask(void);

    /// get mask for selection
    QScriptValue getMask(void);

    /// print info about selected atoms
    QScriptValue printInfo(void);

// atom access methods ---------------------------------------------------------
    /// get number of selected residues
    QScriptValue getNumOfSelectedResidues(void);

    /// get selected atom
    /// residue getResidue(index)
    /// residue getResidue(index,residue)
    QScriptValue getResidue(void);

// section of private data -----------------------------------------------------
private:
    CAmberMaskResidues Mask;

    /// clear object data if topology is cleaned - only weak objects
    virtual void CleanData(void);
};

//------------------------------------------------------------------------------

#endif
