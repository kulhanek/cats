#ifndef QSnapshotH
#define QSnapshotH
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
#include <QCATsScriptable.hpp>
#include <QTopology.hpp>
#include <AmberRestart.hpp>

//------------------------------------------------------------------------------

class QTopology;

//------------------------------------------------------------------------------

/// system snapshot optionaly including velocity and box sizes

class CATS_PACKAGE QSnapshot : public QTopologyObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QSnapshot(const QScriptValue& top);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(int numatoms READ GetNumOfAtoms)

// methods ---------------------------------------------------------------------
public slots:
    /// get topology
    QScriptValue getTopology(void);

    /// load amber restart file
    /// bool load(name)
    QScriptValue load(void);

    /// save amber restart file
    /// bool save(name)
    QScriptValue save(void);

    /// copy from selection, snapshot, or average snapshot
    /// copyFrom(snapshot[,selection])
    /// copyFrom(averageSnapshot[,selection])
    QScriptValue copyFrom(void);

    /// clear all data
    QScriptValue clear(void);

// snapshot setup methods ------------------------------------------------------
    /// set snapshot title
    /// setTitle(title)
    QScriptValue setTitle(void);

    /// get snapshot title
    /// string getTitle()
    QScriptValue getTitle(void);

    /// set snapshot time
    /// setTime(time)
    QScriptValue setTime(void);

    /// get snapshot time
    /// double getTime()
    QScriptValue getTime(void);

// velocities methods ----------------------------------------------------------
    /// are velocities available
    QScriptValue areVelocitiesAvailable(void);

// box methods -----------------------------------------------------------------
    /// is box available
    /// bool isBoxPresent()
    QScriptValue isBoxPresent(void);

    /// get box A vector size
    /// double getBoxASize()
    QScriptValue getBoxASize(void);

    /// get box B vector size
    /// double getBoxBSize()
    QScriptValue getBoxBSize(void);

    /// get box C vector size
    /// double getBoxCSize()
    QScriptValue getBoxCSize(void);

    /// get radius of largest inscribed sphere in A
    /// double getLargestSphereRadius()
    QScriptValue getLargestSphereRadius(void);
    
    /// get box volume in A^3
    /// double getVolume()
    QScriptValue getVolume(void);

    /// get density in g/ml
    /// double getDensity()
    QScriptValue getDensity(void);      
    
    /// get concentration in M
    /// double getConcentration(number)
    QScriptValue getConcentration(void);

// manipulation methods --------------------------------------------------------
    /// center snapshot
    /// center([key1,...])
    QScriptValue center(void);

    /// image snapshot
    /// image([key1,...])
    QScriptValue image(void);

    /// fit snapshot by rmsd to reference structure
    /// rmsdFit([key1,...])
    QScriptValue rmsdFit(void);

    /// get rmsd to reference structure
    QScriptValue getRMSD(void);

    /// get COM point
    /// point getCOM([selection][,point][,key1,...])
    QScriptValue getCOM(void);

// atom access methods ---------------------------------------------------------
    /// get number of atoms
    /// int getNumOfAtoms()
    QScriptValue getNumOfAtoms(void);

    /// get position of atom
    /// point getPosition(index[,point])
    QScriptValue getPosition(void);

    /// get atom mass
    /// double getMass(index)
    QScriptValue getMass(void);

    /// set position of atom
    /// setPosition(index,point)
    QScriptValue setPosition(void);

// access methods --------------------------------------------------------------
public:
    /// return number of atoms
    int  GetNumOfAtoms(void);

    /// return COM (centre of mass) of all atoms
    const CPoint GetCOMPoint(bool nomass=false);

// section of private data -----------------------------------------------------
private:
    CAmberRestart   Restart;

    friend class QSelection;
    friend class QRSelection;
    friend class QAverageSnapshot;
    friend class QTrajectory;
    friend class QNetTrajectory;
    friend class QTrajPool;
    friend class QPMFLibCVs;
    friend class QCovarMatrix;
    friend class QTransformation;
    friend class QVolumeData;
    friend class QPMFLib;
    friend class Qx3DNA;
    friend class QMolSurf;
    friend class QTinySpline;
    friend class QCurvesP;

    /// clear object data if topology is cleaned - only weak objects
    virtual void CleanData(void);

    /// update object data if topology is updated - only weak objects
    virtual void UpdateData(void);
};

//------------------------------------------------------------------------------

#endif
