#ifndef QAverageSnapshotH
#define QAverageSnapshotH
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
#include <QScriptContext>
#include <QScriptable>
#include <AmberRestart.hpp>
#include <QCATsScriptable.hpp>
#include <QTopology.hpp>

//------------------------------------------------------------------------------

class QTopology;

//------------------------------------------------------------------------------

/// average snapshot optionaly including velocity and box sizes

class QAverageSnapshot : public QTopologyObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QAverageSnapshot(const QScriptValue& top);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// methods ---------------------------------------------------------------------
public slots:
    /// start structure averaging
    /// begin()
    QScriptValue begin(void);

    /// add snapshot or selection
    /// addSample(snapshot)
    QScriptValue addSample(void);

    /// end structure averaging
    /// bool finish()
    QScriptValue finish(void);

    /// save average structure
    /// bool save(name)
    QScriptValue save(void);

// section of private data -----------------------------------------------------
private:
    CAmberRestart   Restart;
    bool            SamplingMode;   // object is in sampling mode
    int             NumOfSnapshostsForCrd;
    int             NumOfSnapshostsForVel;
    int             NumOfSnapshostsForBox;

    friend class QSnapshot;

    /// clear object data if topology is cleaned - only weak objects
    virtual void CleanData(void);

    /// update object data if topology is updated - only weak objects
    virtual void UpdateData(void);
};

//------------------------------------------------------------------------------

#endif
