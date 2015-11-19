#ifndef QTopologyH
#define QTopologyH
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
#include <QList>

#include <AmberTopology.hpp>

//------------------------------------------------------------------------------

/// Topology object
class QTopology : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QTopology(void);
    ~QTopology(void);

    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue numOfAtoms    READ getNumOfAtoms WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue numOfResidues READ getNumOfResidues WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue firstAtom     READ getFirstAtom WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue firstResidue  READ getFirstResidue WRITE setIsNotAllowed)

// methods ---------------------------------------------------------------------
public slots:
    /// load topology from a file
    /// bool load(name)
    QScriptValue load(void);

    /// save topology to a file
    /// bool save(name)
    QScriptValue save(void);

    /// load topology from fake file such as pdb, g96
    /// bool loadFakeTopology(name,type)
    QScriptValue loadFakeTopology(void);

    /// is fake topology?
    /// bool isFakeTopology()
    QScriptValue isFakeTopology(void);

    /// copy from another topology or selection
    /// bool Topology::copyFrom(topology)
    /// bool Topology::copyFrom(selection[,key1,key2,..])
    QScriptValue copyFrom(void);

    /// clear topology
    /// Topology::clear()
    QScriptValue clear(void);

    /// print info about topology to standard output
    /// Topology::printInfo()
    QScriptValue printInfo(void);

    /// print info about topology to standard output
    /// Topology::printAtoms()
    QScriptValue printAtoms(void);

    /// build list of neighbour atoms
    /// Topology::buidListOfNeighbourAtoms()
    QScriptValue buidListOfNeighbourAtoms(void);

    /// return number of atoms
    /// int Topology::getNumOfAtoms()
    QScriptValue  getNumOfAtoms(void);

    /// get atom
    /// atom Topology::getAtom(index)
    /// atom Topology::getAtom(index,atom)
    QScriptValue  getAtom(void);

    /// get frist atom in topology
    /// atom getFirstAtom()
    QScriptValue getFirstAtom(void);

    /// return number of residues
    /// int Topology::getNumOfResidues()
    QScriptValue  getNumOfResidues(void);

    /// get residue
    /// residue Topology::getResidue(index)
    /// residue Topology::getResidue(index,residue)
    QScriptValue  getResidue(void);

    /// get first residue in topology
    /// residue getFirstResidue()
    QScriptValue getFirstResidue(void);

// section of private data -----------------------------------------------------
private:
    CAmberTopology  Topology;

    // weak objects need cleanup/updates
    QList<QObject*> WeakObjects;    // selection, rselection, snapshot, trajectory

    // child objects will be destroyed upon topology cleanup
    QList<QObject*> ChildObjects;   // atoms, residues

    friend class QTopologyObject;
    friend class QSnapshot;
    friend class QAverageSnapshot;
    friend class QTrajectory;
    friend class QTrajPool;
    friend class QSelection;
    friend class QRSelection ;
    friend class QPMFLibCVs;
    friend class QCovarMatrix;
    friend class QResidue;
    friend class QAtom;
    friend class QThermoIG;

    /// helper methods
    void DestroyChildObjects(void);
    void DestroyWeakObjects(void);
    void CleanWeakObjects(void);
    void UpdateWeakObjects(void);
};

//------------------------------------------------------------------------------

/// object associated with topology
class QTopologyObject : public QObject {
public:
    /// constructor
    QTopologyObject(void);

    /// destructor
    ~QTopologyObject(void);

    /// register as child object
    void RegisterAsChildObject(const QScriptValue& top);

    /// register as weak object
    void RegisterAsWeakObject(const QScriptValue& top);

    /// unlink object
    void UnlinkTopologyObject(void);

    /// clear object data if topology is cleaned - only weak objects
    virtual void CleanData(void);

    /// update object data if topology is updated - only weak objects
    virtual void UpdateData(void);

    /// get associated QTopology
    QTopology* GetQTopology(void);

// section of private data -----------------------------------------------------
protected:
    QScriptValue    Topology;
};

//------------------------------------------------------------------------------

#endif
