#ifndef QSelectionH
#define QSelectionH
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

#include <AmberMaskAtoms.hpp>
#include <vector>

// -----------------------------------------------------------------------------

class QTopology;
class QSnapshot;

//------------------------------------------------------------------------------

/// select group of atoms and provide their properties

class QSelection : public QTopologyObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QSelection(const QScriptValue& top);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue numOfSelectedAtoms  READ getNumOfSelectedAtoms WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue mask                READ getMask WRITE setIsNotAllowed)

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
    /// get number of selected atoms
    QScriptValue getNumOfSelectedAtoms(void);

    /// get selected atom
    /// atom getAtom(index)
    /// atom getAtom(index,atom)
    QScriptValue getAtom(void);

// section of private data -----------------------------------------------------
private:
    CAmberMaskAtoms Mask;

    friend class QGeometry;
    friend class QTopology;
    friend class QSnapshot;
    friend class QAverageSnapshot;
    friend class QTransformation;
    friend class Qx3DNA;

    /// clear object data if topology is cleaned - only weak objects
    virtual void CleanData(void);
};

//------------------------------------------------------------------------------

#endif
