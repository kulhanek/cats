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

#include <iostream>
#include <QScriptEngine>
#include <QNetTrajectory.hpp>
#include <QNetTrajectory.moc>
#include <QTopology.hpp>
#include <QSnapshot.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QNetTrajectory::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QNetTrajectory::New);
    QScriptValue metaObject = engine.newQMetaObject(&QNetTrajectory::staticMetaObject, ctor);
    engine.globalObject().setProperty("NetTrajectory", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QNetTrajectory::New(QScriptContext *context,
                                 QScriptEngine *engine)
{
    if( context->isCalledAsConstructor() == false ) {
        context->throwError("Trajectory() can be called only as a constructor\nusage: new NetTrajectory(topology)");
        return(engine->undefinedValue());
    }
    if( context->argumentCount() != 1 ) {
        context->throwError("Illegal number of arguments\nusage: new NetTrajectory(topology)");
        return(engine->undefinedValue());
    }

    QTopology* p_qtop =  dynamic_cast<QTopology*>(context->argument(0).toQObject());
    if( p_qtop == NULL ) {
        context->throwError("First parameter is not Topology");
        return(engine->undefinedValue());
    }

    QNetTrajectory* p_obj = new QNetTrajectory(p_qtop);
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QNetTrajectory::QNetTrajectory(QTopology* p_parent)
    : QCATsScriptable("NetTrajectory")
{
    ClientID = -1;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QNetTrajectory::setServerName(const QString& name)
{
    if( argumentCount() != 1 ) {
        context()->throwError("illegal number of arguments\nusage: NetTrajectory::setServerName()");
        return;
    }
    TrajClient.ActionRequest.SetNameOrIP(name);
}

//------------------------------------------------------------------------------

const QString QNetTrajectory::getServerName(void)
{
    if( argumentCount() != 0 ) {
        context()->throwError("illegal number of arguments\nusage: NetTrajectory::getServerName()");
        return("");
    }
    return(QString(TrajClient.ActionRequest.GetName()));
}

//------------------------------------------------------------------------------

void QNetTrajectory::setServerPort(int port)
{
    if( argumentCount() != 1 ) {
        context()->throwError("illegal number of arguments\nusage: NetTrajectory::setServerPort()");
        return;
    }
    TrajClient.ActionRequest.SetPort(port);
}

//------------------------------------------------------------------------------

int QNetTrajectory::getServerPort(void)
{
    if( argumentCount() != 0 ) {
        context()->throwError("illegal number of arguments\nusage: NetTrajectory::getServerPort()");
        return(-1);
    }
    return(TrajClient.ActionRequest.GetPort());
}

//------------------------------------------------------------------------------

void QNetTrajectory::setServerPassword(const QString& password)
{
    if( argumentCount() != 1 ) {
        context()->throwError("illegal number of arguments\nusage: NetTrajectory::setServerPassword()");
        return;
    }
    TrajClient.ActionRequest.SetPassword(password);
}

//------------------------------------------------------------------------------

bool QNetTrajectory::setServerKey(const QString& name)
{
    if( argumentCount() != 1 ) {
        context()->throwError("illegal number of arguments\nusage: NetTrajectory::setServerKey(name)");
        return(false);
    }
    bool result = false;
    //FIXME
    //TrajClient.ActionRequest.ReadServerKey(name);
    return(result);
}

//------------------------------------------------------------------------------

bool QNetTrajectory::registerClient(void)
{
    if( argumentCount() != 0 ) {
        context()->throwError("illegal number of arguments\nusage: NetTrajectory::registerClient()");
        return(false);
    }
    TrajClient.RegisterClient(ClientID);
    return( ClientID != -1 );
}

//------------------------------------------------------------------------------

int QNetTrajectory::read(QObject* p_snap)
{
    if( argumentCount() != 1 ) {
        context()->throwError("illegal number of arguments\nusage: NetTrajectory::read(snapshot)");
        return(false);
    }
    QSnapshot* p_qsnap = dynamic_cast<QSnapshot*>(p_snap);
    if( p_qsnap == NULL ) {
        context()->throwError("illegal argument\nusage: NetTrajectory::read(snapshot)");
        return(false);
    }
    int result = TrajClient.GetSnapshot(ClientID,&p_qsnap->Restart,"next",false);
    return(result);
}

//------------------------------------------------------------------------------

int QNetTrajectory::readVelocities(QObject* p_snap)
{
    if( argumentCount() != 1 ) {
        context()->throwError("illegal number of arguments\nusage: NetTrajectory::readVelocities(snapshot)");
        return(false);
    }
    QSnapshot* p_qsnap = dynamic_cast<QSnapshot*>(p_snap);
    if( p_qsnap == NULL ) {
        context()->throwError("illegal argument\nusage: NetTrajectory::readVelocities(snapshot)");
        return(false);
    }
    int result = TrajClient.GetSnapshot(ClientID,&p_qsnap->Restart,"next",true);
    return(result);
}

//------------------------------------------------------------------------------

bool QNetTrajectory::unregisterClient(void)
{
    if( argumentCount() != 0 ) {
        context()->throwError("illegal number of arguments\nusage: NetTrajectory::unregisterClient()");
        return(false);
    }

    bool result = TrajClient.UnregisterClient(ClientID);
    ClientID = -1;
    return(result);
}

//------------------------------------------------------------------------------

int QNetTrajectory::getClientID(void)
{
    if( argumentCount() != 1 ) {
        context()->throwError("illegal number of arguments\nusage: NetTrajectory::unregisterClient()");
        return(-1);
    }
    return(ClientID);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



