#ifndef QNetTrajectoryH
#define QNetTrajectoryH
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
#include <TrajectoryClient.hpp>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

class QTopology;
class QSnapshot;

//------------------------------------------------------------------------------

class CATS_PACKAGE QNetTrajectory : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QNetTrajectory(QTopology* p_parent);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// methods ---------------------------------------------------------------------
public slots:
    /// set server name
    void setServerName(const QString& name);

    /// get server name
    const QString getServerName(void);

    /// set server port
    void setServerPort(int port);

    /// get server port
    int getServerPort(void);

    /// set server port
    void setServerPassword(const QString& password);

    /// set server key
    bool setServerKey(const QString& name);

    /// register client
    bool registerClient(void);

    /// read snapshot
    int read(QObject* p_snap);

    /// read snapshot velocities
    int readVelocities(QObject* p_snap);

    /// unregister client
    bool unregisterClient(void);

    /// get client ID
    int getClientID(void);

// section of private data -----------------------------------------------------
private:
    CTrajectoryClient   TrajClient;
    int                 ClientID;
};

//------------------------------------------------------------------------------

#endif

