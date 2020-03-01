#ifndef QTrajPoolH
#define QTrajPoolH
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
#include <vector>
#include <QCATsScriptable.hpp>
#include <QTopology.hpp>

//------------------------------------------------------------------------------

class QTopology;
class QSnapshot;

//------------------------------------------------------------------------------

class CATS_PACKAGE QTrajPool : public QTopologyObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QTrajPool(const QScriptValue& top);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// methods ---------------------------------------------------------------------
public slots:
    /// get topology
    QScriptValue getTopology(void);

    /// add trajectory
    /// addTrajectory(name)
    /// addTrajectory(name,format)
    QScriptValue addTrajectory(void);

    /// add trajectory list
    /// addTrajList(first,last)
    /// addTrajList(first,last,tmpname,format)
    QScriptValue addTrajList(void);

    /// add trajectory list from
    /// addTrajListFrom(first,last,path)
    /// addTrajListFrom(first,last,path,tmpname,format)
    QScriptValue addTrajListFrom(void);

    /// clear pool
    QScriptValue clear(void);

    /// rewind to begining
    QScriptValue rewind(void);

    /// read snapshot
    /// snapshot read()
    /// snapshot read(snapshot)
    QScriptValue read(void);

    /// print info about entire pool
    QScriptValue printInfo(void);

    /// print progress
    QScriptValue printProgress(void);

// access methods --------------------------------------------------------------
public:
    /// decode format
    ETrajectoryFormat decodeFormat(const QString& format);

    /// encode format
    const QString encodeFormat(ETrajectoryFormat format);

    /// add trajectory to the pool
    /// 0 - OK, 1 - not exist, < 0 - some error, abort
    int addTrajFile(const QString& name,const QString& fmt);

// section of private data -----------------------------------------------------
private:
    class CTrajPoolItem {
    public:
        CTrajPoolItem(void);
        QString     Name;
        QString     Format;
        int         NumOfSnapshots;
    };

    QString                     DefaultTmpName;
    std::vector<CTrajPoolItem>  Items;
    int                         CurrentItem;
    CAmberTrajectory            Trajectory;
    bool                        IgnoreMissingFiles;

    // progress
    bool                ProgressStarted;    
    int                 CurrentSnapshot;
    int                 ProgressSnaphost;
    int                 PrevCurrSnapshot;
    int                 PrevNumOfSnapshots;

    /// clear object data if topology is cleaned - only weak objects
    virtual void CleanData(void);
};

//------------------------------------------------------------------------------

#endif
