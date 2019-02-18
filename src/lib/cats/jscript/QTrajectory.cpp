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

#include <iostream>
#include <QScriptEngine>
#include <QTrajectory.hpp>
#include <moc_QTrajectory.cpp>
#include <QTopology.hpp>
#include <QSnapshot.hpp>
#include <iomanip>
#include <TerminalStr.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QTrajectory::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QTrajectory::New);
    QScriptValue metaObject = engine.newQMetaObject(&QTrajectory::staticMetaObject, ctor);
    engine.globalObject().setProperty("Trajectory", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QTrajectory::New(QScriptContext *context, QScriptEngine *engine)
{
    QCATsScriptable scriptable("Trajectory");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Trajectory object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new Trajectory(topology)" << endl;
        sout << "   new Trajectory(topology,name)" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("topology[,name]",1,2);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( scriptable.GetArgumentCount() == 1 ){
        QTopology* p_qtop;
        value = scriptable.GetArgAsObject<QTopology*>("topology","topology","Topology",1,p_qtop);
        if( value.isError() ) return(value);

        QTrajectory* p_obj = new QTrajectory(scriptable.GetArgument(1));
        return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
    }

    if( scriptable.GetArgumentCount() == 2 ){
        QTopology* p_qtop;
        value = scriptable.GetArgAsObject<QTopology*>("topology,name","topology","Topology",1,p_qtop);
        if( value.isError() ) return(value);

        QString name;
        value = scriptable.GetArgAsString("topology,name","name",2,name);
        if( value.isError() ) return(value);

        QTrajectory* p_obj = new QTrajectory(scriptable.GetArgument(1));

        p_obj->ProgressStarted = false;
        p_obj->CurrentSnapshot = 0;
        bool result = p_obj->Trajectory.OpenTrajectoryFile(name,AMBER_TRAJ_UNKNOWN,AMBER_TRAJ_CXYZB,AMBER_TRAJ_READ);
        if( result ){
            p_obj->Name = name;
            p_obj->Format = p_obj->Trajectory.GetFormat();
            p_obj->OpenMode = p_obj->Trajectory.GetOpenMode();
            return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
        } else {
            delete p_obj;
        }
    }

    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QTrajectory::QTrajectory(const QScriptValue& top)
    : QCATsScriptable("Trajectory")
{
    RegisterAsWeakObject(top);
    Trajectory.AssignTopology(&GetQTopology()->Topology);
    ProgressStarted = false;
    CurrentSnapshot = 0;
}

//------------------------------------------------------------------------------

void QTrajectory::CleanData(void)
{
    Trajectory.CloseTrajectoryFile();

    ProgressStarted = false;
    CurrentSnapshot = 0;
    Name = "";
    Format = AMBER_TRAJ_UNKNOWN;
    OpenMode = AMBER_TRAJ_READ;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QTrajectory::getTopology(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: topology Trajectory::getTopology()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Topology);
}

//------------------------------------------------------------------------------

QScriptValue QTrajectory::open(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Trajectory::open(name[,key1,key2,...])" << endl;
        sout << "       mode:   read  - open trajectory for reading" << endl;
        sout << "               write - open trajectory for writing" << endl;
        sout << "       format: ascii      - ASCII format" << endl;
        sout << "               ascii.gzip - compressed ASCII format" << endl;
        sout << "               ascii.bzip - compressed ASCII format" << endl;
        sout << "               netcdf     - NETCDF format" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckMinimumNumberOfArguments("name",1);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name[,key1,key2,...]","name",1,name);
    if( value.isError() ) return(value);

    ETrajectoryOpenMode emode = AMBER_TRAJ_READ;
    ETrajectoryFormat traj_format = AMBER_TRAJ_UNKNOWN;

    if( IsArgumentKeySelected("read") == true ){
        emode = AMBER_TRAJ_READ;
    }

    if( IsArgumentKeySelected("write") == true ){
        emode = AMBER_TRAJ_WRITE;
    }

    if( IsArgumentKeySelected("ascii") == true ){
        traj_format = AMBER_TRAJ_ASCII;
    }
    if( IsArgumentKeySelected("ascii.gzip") == true ){
        traj_format = AMBER_TRAJ_ASCII_GZIP;
    }
    if( IsArgumentKeySelected("ascii.bzip2") == true ){
        traj_format = AMBER_TRAJ_ASCII_BZIP2;
    }
    if( IsArgumentKeySelected("netcdf") == true ){
        traj_format = AMBER_TRAJ_NETCDF;
    }
    value = CheckArgumentsUsage("name[,key1,key2,...]");
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( (emode == AMBER_TRAJ_WRITE) && (traj_format == AMBER_TRAJ_UNKNOWN) ){
        traj_format = AMBER_TRAJ_NETCDF;
    }

    ProgressStarted = false;
    CurrentSnapshot = 0;
    bool result = Trajectory.OpenTrajectoryFile(name,traj_format,AMBER_TRAJ_CXYZB,emode);
    if( result ){
        Name = name;
        Format = Trajectory.GetFormat();
        OpenMode = Trajectory.GetOpenMode();
    }
    return(result);
}

//------------------------------------------------------------------------------

QScriptValue QTrajectory::read(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: snapshot Trajectory::read(snapshot)" << endl;
        sout << "       snapshot Trajectory::read()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("[snapshot]",0,1);
    if( value.isError() ) return(value);

    QSnapshot* p_qsnap = NULL;
    if( GetArgumentCount() == 1 ){
        value = GetArgAsObject<QSnapshot*>("snapshot","snapshot","Snapshot",1,p_qsnap);
        if( value.isError() ) return(value);
    } else {
        p_qsnap = new QSnapshot(Topology);
    }

// execute ---------------------------------------
    if( Trajectory.GetOpenMode() != AMBER_TRAJ_READ ){
        return( ThrowError("snapshot","trajectory is not opened for reading") );
    }
    bool result = Trajectory.ReadSnapshot(&p_qsnap->Restart);
    if( result ){
        CurrentSnapshot++;
    } else {
        if( GetArgumentCount() == 0 ){
            delete p_qsnap;
        }
        return( GetUndefinedValue() );
    }
    if( GetArgumentCount() == 1 ){
        return(GetArgument(1));
    } else {
        return(engine()->newQObject(p_qsnap, QScriptEngine::ScriptOwnership));
    }
}

//------------------------------------------------------------------------------

QScriptValue QTrajectory::write(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Trajectory::write(snapshot)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("snapshot",1);
    if( value.isError() ) return(value);

    QSnapshot* p_qsnap;
    value = GetArgAsObject<QSnapshot*>("snapshot","snapshot","Snapshot",1,p_qsnap);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( Trajectory.GetOpenMode() != AMBER_TRAJ_WRITE ){
        return( ThrowError("snapshot","trajectory is not opened for writing") );
    }
    return(Trajectory.WriteSnapshot(&p_qsnap->Restart));
}

//------------------------------------------------------------------------------

QScriptValue QTrajectory::close(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Trajectory::close()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    ProgressStarted = false;
    CurrentSnapshot = 0;

    Name = "";
    Format = AMBER_TRAJ_UNKNOWN;
    OpenMode = AMBER_TRAJ_READ;

    return(Trajectory.CloseTrajectoryFile());
}

//------------------------------------------------------------------------------

QScriptValue QTrajectory::rewind(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Trajectory::rewind()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( Trajectory.IsItOpened() == false ){
        return( ThrowError("","trajectory is not opened") );
    }

    if( OpenMode != AMBER_TRAJ_READ ){
        return( ThrowError("","trajectory is not opened for reading") );
    }

    Trajectory.CloseTrajectoryFile();
    ProgressStarted = false;
    CurrentSnapshot = 0;

    bool result = Trajectory.OpenTrajectoryFile(Name,Format,AMBER_TRAJ_CXYZB,OpenMode);
    if( ! result ){
        Name = "";
        Format = AMBER_TRAJ_UNKNOWN;
        OpenMode = AMBER_TRAJ_READ;
    }
    return( result );
}

//------------------------------------------------------------------------------

QScriptValue QTrajectory::isOpened(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool Trajectory::isOpened()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Trajectory.IsItOpened());
}

//------------------------------------------------------------------------------

QScriptValue QTrajectory::setTitle(const QScriptValue& tmp)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Trajectory::setTitle(title)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("title",1);
    if( value.isError() ) return(value);

    QString title;
    value = GetArgAsString("title","title",1,title);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( Trajectory.IsItOpened() ){
        return( ThrowError("title","trajectory is already opened") );
    }
    Trajectory.SetTitle(title);
    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QTrajectory::getTitle(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string Trajectory::getTitle()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(QString(Trajectory.GetTitle()));
}

//------------------------------------------------------------------------------

QScriptValue QTrajectory::getFormat(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string Trajectory::getFormat()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    ETrajectoryFormat form = Trajectory.GetFormat();
    return(encodeFormat(form));
}

//------------------------------------------------------------------------------

QScriptValue QTrajectory::printInfo(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Trajectory::printInfo()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    cout << "=== Trajectory" << endl;
    if( Name.isEmpty() ) {
        cout << " Name   : " << "-not opened-" << endl;
    } else {
        cout << " Name   : " << Name.toStdString() << endl;
        if( Trajectory.GetTitle() == NULL ){
        cout << " Title  : " << "-none-" << endl;
        } else {
        cout << " Title  : " << Trajectory.GetTitle() << endl;
        }
        cout << " Format : " << encodeFormat(Trajectory.GetFormat()).toStdString() << endl;
        cout << " Mode   : " << encodeOpenMode(Trajectory.GetOpenMode()).toStdString() << endl;
        cout << " Number of atoms     : " << Trajectory.GetNumberOfAtoms() << endl;
        if( Trajectory.GetNumberOfSnapshots() >= 0 ){
        cout << " Number of snapshots : " << Trajectory.GetNumberOfSnapshots() << endl;
        } else {
        cout << " Number of snapshots : " << "0" << endl;
        }
    }

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue  QTrajectory::printProgress(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: Trajectory::printProgress()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( Trajectory.IsItOpened() == false ){
        return(ThrowError("","trajectory is not opened"));
    }

    if( ProgressStarted == false ){
        cout << setw(15) << left << Name.toStdString() << " |";
        ProgressStarted = true;
        ProgressSnapshot = 0;
    }
    if( ProgressStarted ){
        if( ProgressSnapshot > Trajectory.GetNumberOfSnapshots() ) return(value);

        for(int i=ProgressSnapshot;i < CurrentSnapshot; i++){
            if( Trajectory.GetNumberOfSnapshots() > 80 ){
                if( i % (Trajectory.GetNumberOfSnapshots()/80) == 0 ){
                    cout << "=";
                }
            }
            if( i == Trajectory.GetNumberOfSnapshots()/4 ){
                cout << " 25% ";
            }
            if( i == Trajectory.GetNumberOfSnapshots()/2 ){
                cout << " 50% ";
            }
            if( i == 3*Trajectory.GetNumberOfSnapshots()/4 ){
                cout << " 75% ";
            }
        }
        ProgressSnapshot = CurrentSnapshot;
        if( CurrentSnapshot == Trajectory.GetNumberOfSnapshots() ){
                cout << "|" << endl;
        }
        cout.flush();
    }

    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

ETrajectoryFormat QTrajectory::decodeFormat(const QString& format)
{
    if( format == "ascii" ){
        return(AMBER_TRAJ_ASCII);
    } else if ( format == "ascii.gzip" )  {
        return(AMBER_TRAJ_ASCII_GZIP);
    } else if ( format == "ascii.bzip2" )  {
        return(AMBER_TRAJ_ASCII_BZIP2);
    } else if ( format == "netcdf" )  {
        return(AMBER_TRAJ_NETCDF);
    } else {
        return(AMBER_TRAJ_UNKNOWN);
    }
}

//------------------------------------------------------------------------------

const QString QTrajectory::encodeFormat(ETrajectoryFormat format)
{
    switch(format) {
        case AMBER_TRAJ_ASCII:
            return("ascii");
        case AMBER_TRAJ_ASCII_GZIP:
            return("ascii.gzip");
        case AMBER_TRAJ_ASCII_BZIP2:
            return("ascii.bzip2");
        case AMBER_TRAJ_NETCDF:
            return("netcdf");
        default:
        case AMBER_TRAJ_UNKNOWN:
            return("unknown");
    }
}

//------------------------------------------------------------------------------

const QString QTrajectory::encodeOpenMode(ETrajectoryOpenMode mode)
{
    switch(mode) {
        case AMBER_TRAJ_READ:
            return("read");
        case AMBER_TRAJ_WRITE:
            return("write");
        default:
            return("unknown");
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


