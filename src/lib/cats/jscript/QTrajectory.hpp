#ifndef QTrajectoryH
#define QTrajectoryH
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

#include <CATsMainHeader.hpp>
#include <QObject>
#include <QScriptValue>
#include <AmberTrajectory.hpp>
#include <QScriptable>
#include <QCATsScriptable.hpp>
#include <QTopology.hpp>

//------------------------------------------------------------------------------

class QTopology;
class QSnapshot;

//------------------------------------------------------------------------------

class CATS_PACKAGE QTrajectory : public QTopologyObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QTrajectory(const QScriptValue& top);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue topology    READ getTopology WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue title       READ getTitle WRITE setTitle)
    Q_PROPERTY(QScriptValue format      READ getFormat WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue isopen      READ isOpened WRITE setIsNotAllowed)

// methods ---------------------------------------------------------------------
public slots:    
    /// get topology
    QScriptValue getTopology(void);

    /// open trajectory in read mode
    QScriptValue open(void);

    /// read snapshot
    /// snapshot read()
    /// snapshot read(snapshot)
    QScriptValue read(void);

    /// write snapshot
    /// bool write(snapshot)
    QScriptValue write(void);

    /// close trajectory
    QScriptValue close(void);

    /// rewind trajectory (it will call close and open)
    QScriptValue rewind(void);

    /// is trajectory opened?
    QScriptValue isOpened(void);

    /// set trajectory title - trajectory cannot be opened
    QScriptValue setTitle(const QScriptValue& tmp);

    /// get trajectory title
    QScriptValue getTitle(void);

    /// get trajectory format
    QScriptValue getFormat(void);

    /// print info
    QScriptValue printInfo(void);

    /// print progress
    QScriptValue printProgress(void);

    /// get number of snapshots
    QScriptValue getNumOfSnapshots(void);

// access methods --------------------------------------------------------------
public:
    /// decode format
    ETrajectoryFormat decodeFormat(const QString& format);

    /// encode format
    const QString encodeFormat(ETrajectoryFormat format);

    /// encode open mode
    const QString encodeOpenMode(ETrajectoryOpenMode mode);

// section of private data -----------------------------------------------------
private:
    CAmberTrajectory    Trajectory;
    QString             Name;
    ETrajectoryFormat   Format;
    ETrajectoryOpenMode OpenMode;
    bool                ProgressStarted;
    int                 CurrentSnapshot;
    int                 ProgressSnapshot;

    /// clear object data if topology is cleaned - only weak objects
    virtual void CleanData(void);
};

//------------------------------------------------------------------------------

#endif
