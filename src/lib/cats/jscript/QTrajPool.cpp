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
#include <QTrajPool.hpp>
#include <QTopology.hpp>
#include <QSnapshot.hpp>
#include <iomanip>
#include <boost/format.hpp>
#include <sstream>
#include <ErrorSystem.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

#include <QTrajPool.moc>
using namespace std;
using boost::format;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QTrajPool::CTrajPoolItem::CTrajPoolItem(void)
{
    NumOfSnapshots = -1;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QTrajPool::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QTrajPool::New);
    QScriptValue metaObject = engine.newQMetaObject(&QTrajPool::staticMetaObject, ctor);
    engine.globalObject().setProperty("TrajPool", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QTrajPool::New(QScriptContext *context,
                              QScriptEngine *engine)
{
    QCATsScriptable scriptable("TrajPool");
    QScriptValue    value;

// print help ------------------------------------
    if( scriptable.IsHelpRequested() ){
        CTerminalStr sout;
        sout << "Trajectory pool object" << endl;
        sout << endl;
        sout << "Constructors:" << endl;
        sout << "   new TrajPool(topology)" << endl;
        return(scriptable.GetUndefinedValue());
    }

// check arguments -------------------------------
    value = scriptable.IsCalledAsConstructor();
    if( value.isError() ) return(value);

    value = scriptable.CheckNumberOfArguments("topology",1);
    if( value.isError() ) return(value);

    QTopology* p_qtop;
    value = scriptable.GetArgAsObject<QTopology*>("topology","topology","Topology",1,p_qtop);
    if( value.isError() ) return(value);

    QTrajPool* p_obj = new QTrajPool(scriptable.GetArgument(1));
    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QTrajPool::QTrajPool(const QScriptValue& top)
    : QCATsScriptable("TrajPool")
{
    RegisterAsWeakObject(top);
    Trajectory.AssignTopology(&GetQTopology()->Topology);
    CurrentItem = -1;
    ProgressStarted = false;
    CurrentSnapshot = 0;
    PrevCurrSnapshot = -1;
    DefaultTmpName = "prod%03d.traj";
}

//------------------------------------------------------------------------------

void QTrajPool::CleanData(void)
{
    Items.clear();
    CurrentItem = -1;
    ProgressStarted = false;
    CurrentSnapshot = 0;
    PrevCurrSnapshot = -1;
    Trajectory.CloseTrajectoryFile();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QTrajPool::getTopology(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: topology TrajPool::getTopology()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(Topology);
}

//------------------------------------------------------------------------------

QScriptValue QTrajPool::addTrajectory(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool TrajPool::addTrajectory(name[,format])" << endl;
        sout << "       format: ascii      - ASCII format" << endl;
        sout << "               ascii.gzip - compressed ASCII format" << endl;
        sout << "               ascii.bzip - compressed ASCII format" << endl;
        sout << "               netcdf     - NETCDF format" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("name",1,2);
    if( value.isError() ) return(value);

    QString name;
    value = GetArgAsString("name[,format]","name",1,name);
    if( value.isError() ) return(value);

    QString format = "unknown";
    if( GetArgumentCount() == 2 ){
        value = GetArgAsString("name,format","format",2,format);
        if( value.isError() ) return(value);
    }

// execute ---------------------------------------
    if( addTrajFile(name,format) == false ){
        CSmallString error;
        if( format == "unknown" ){
            error << "unable to add file '" << CSmallString(name) << "'";
        } else {
            error << "unable to add file '" << CSmallString(name) << "' with format '" << CSmallString(format) << "'";
        }
        return( ThrowError("name[,format]",error));
    }
    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QTrajPool::addTrajList(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool TrajPool::addTrajList(first,last[,tmpname,format])" << endl;
        sout << "       format: ascii      - ASCII format" << endl;
        sout << "               ascii.gzip - compressed ASCII format" << endl;
        sout << "               ascii.bzip - compressed ASCII format" << endl;
        sout << "               netcdf     - NETCDF format" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("first,last[,tmpname,format]",2,4);
    if( value.isError() ) return(value);

    int first;
    value = GetArgAsInt("first,last[,tmpname,format]","first",1,first);
    if( value.isError() ) return(value);

    int last;
    value = GetArgAsInt("first,last[,tmpname,format]","last",2,last);
    if( value.isError() ) return(value);

    QString tmpname = DefaultTmpName;
    QString tformat = "unknown";
    if( GetArgumentCount() > 2 ){
        value = GetArgAsString("first,last,tmpname,format","tmpname",3,tmpname);
        if( value.isError() ) return(value);

        value = GetArgAsString("first,last,tmpname,format","format",4,tformat);
        if( value.isError() ) return(value);
    }

// execute ---------------------------------------
    for(int i=first; i <= last; i++){
        stringstream str;
        str << format(tmpname.toStdString()) % i;
        if( addTrajFile(str.str().c_str(),tformat) == false ){
            CSmallString error;
            if( tformat == "unknown" ){
                error << "unable to add file '" << str.str() << "'";
            } else {
                error << "unable to add file '" << str.str() << "' with format '" << CSmallString(tformat) << "'";
            }
            return( ThrowError("first,last[,tmpname,format]",error) );
        }
    }
    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QTrajPool::addTrajListFrom(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: bool TrajPool::addTrajListFrom(first,last,path[,tmpname,format])" << endl;
        sout << "       format: ascii      - ASCII format" << endl;
        sout << "               ascii.gzip - compressed ASCII format" << endl;
        sout << "               ascii.bzip - compressed ASCII format" << endl;
        sout << "               netcdf     - NETCDF format" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("first,last,path[,tmpname,format]",3,5);
    if( value.isError() ) return(value);

    int first;
    value = GetArgAsInt("first,last,path[,tmpname,format]","first",1,first);
    if( value.isError() ) return(value);

    int last;
    value = GetArgAsInt("first,last,path[,tmpname,format]","last",2,last);
    if( value.isError() ) return(value);

    QString path;
    value = GetArgAsString("first,last,path[,tmpname,format]","path",3,path);
    if( value.isError() ) return(value);

    QString tmpname = DefaultTmpName;
    QString tformat = "unknown";
    if( GetArgumentCount() > 2 ){
        value = GetArgAsString("first,last,path[,tmpname,format]","tmpname",4,tmpname);
        if( value.isError() ) return(value);

        value = GetArgAsString("first,last,path[,tmpname,format]","format",5,tformat);
        if( value.isError() ) return(value);
    }


// execute ---------------------------------------
    for(int i=first; i <= last; i++){
        stringstream str;
        QString namefmt = path + "/" + tmpname;
        str << format(namefmt.toStdString()) % i;

        if( addTrajFile(str.str().c_str(),tformat) == false ){
            CSmallString error;
            if( tformat == "unknown" ){
                error << "unable to add file '" << str.str() << "'";
            } else {
                error << "unable to add file '" << str.str() << "' with format '" << CSmallString(tformat) << "'";
            }
            return( ThrowError("first,last,path[,tmpname,format]",error) );
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool QTrajPool::addTrajFile(const QString& name,const QString& fmt)
{
    CTrajPoolItem item;
    item.Name = name;
    item.Format = fmt;

    CAmberTrajectory traj;
    traj.AssignTopology(Trajectory.GetTopology());
    if( traj.OpenTrajectoryFile(name,decodeFormat(fmt),AMBER_TRAJ_CXYZB,AMBER_TRAJ_READ) == false ){
        return(false);
    }
    item.Format = encodeFormat(traj.GetFormat());
    item.NumOfSnapshots = traj.GetNumberOfSnapshots();
    traj.CloseTrajectoryFile();

    Items.push_back(item);

    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QTrajPool::clear(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TrajPool::clear()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    Items.clear();
    CurrentItem = -1;
    ProgressStarted = false;
    CurrentSnapshot = 0;
    PrevCurrSnapshot = -1;
    Trajectory.CloseTrajectoryFile();

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QTrajPool::rewind(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TrajPool::rewind()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    CurrentItem = -1;
    ProgressStarted = false;
    CurrentSnapshot = 0;
    PrevCurrSnapshot = -1;
    Trajectory.CloseTrajectoryFile();

    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QTrajPool::read(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: snapshot TrajPool::read(snapshot)" << endl;
        sout << "       snapshot TrajPool::read()" << endl;
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

    // is pool opened
    if( CurrentItem < 0 ){
        CurrentItem = 0;
        if( CurrentItem >= (int)Items.size() ){
            // no items in the pool
            return(false);
        }
        CurrentSnapshot = 0;
        if( Trajectory.OpenTrajectoryFile(Items[CurrentItem].Name,
                decodeFormat(Items[CurrentItem].Format), AMBER_TRAJ_CXYZB,  AMBER_TRAJ_READ) == false ){
            if( GetArgumentCount() == 0 ){
                delete p_qsnap;
            }
            return( GetUndefinedValue() );
        }
    }

    bool result = Trajectory.ReadSnapshot(&p_qsnap->Restart);
    if( result ){
        CurrentSnapshot++;
    } else {
        PrevCurrSnapshot = CurrentSnapshot;
        Trajectory.CloseTrajectoryFile();
        CurrentItem++;
        if( CurrentItem >= (int)Items.size() ){
            // no items in the pool
            if( GetArgumentCount() == 0 ){
                delete p_qsnap;
            }
            return( GetUndefinedValue() );
        }
        if( Trajectory.OpenTrajectoryFile(Items[CurrentItem].Name,
                                      decodeFormat(Items[CurrentItem].Format),
                                      AMBER_TRAJ_CXYZB,
                                      AMBER_TRAJ_READ) == false ){
            if( GetArgumentCount() == 0 ){
                delete p_qsnap;
            }
            return( GetUndefinedValue() );
        }
        // try to read again
        CurrentSnapshot = 0;
        result = Trajectory.ReadSnapshot(&p_qsnap->Restart);
        if( result ){
            CurrentSnapshot++;
        }
    }
    if( result == false ){
        if( GetArgumentCount() == 0 ){
            delete p_qsnap;
        }
        return( GetUndefinedValue() );
    } else {
        if( GetArgumentCount() == 1 ){
            return(GetArgument(1));
        } else {
            return(engine()->newQObject(p_qsnap, QScriptEngine::ScriptOwnership));
        }
    }
}


//------------------------------------------------------------------------------

ETrajectoryFormat QTrajPool::decodeFormat(const QString& format)
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

const QString QTrajPool::encodeFormat(ETrajectoryFormat format)
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

QScriptValue QTrajPool::printInfo(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TrajPool::printInfo()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    cout << "=== Trajectory pool" << endl;
    cout << "# Number of items : " << Items.size() << endl;
    cout << "# Number of atoms : " << Trajectory.GetNumberOfAtoms() << endl;
    cout << "#" << endl;
    cout << "# Snapshots    Format   Name" << endl;
    cout << "# ---------- ---------- -----------------------------------------------------------" << endl;

    int tot_snapshots = 0;
    for(unsigned int i=0; i < Items.size(); i++){
        if( Items[i].NumOfSnapshots >= 0 ){
        if( tot_snapshots >= 0 ) tot_snapshots += Items[i].NumOfSnapshots;
        cout << "  " << setw(10) << right << Items[i].NumOfSnapshots;
        } else {
        cout << "            ";
        tot_snapshots = -1;
        }
        cout << left << " " << setw(10) << left << Items[i].Format.toStdString();
        cout << " " << left << Items[i].Name.toStdString() << endl;
    }
    cout << "# ---------------------------------------------------------------------------------" << endl;
    cout << "# Total number of snapshots : " << tot_snapshots << endl;

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QTrajPool::printProgress(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: TrajPool::printProgress()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( (PrevCurrSnapshot >= 0) && (ProgressStarted == true) ){
        // finish previous progress
        if( ProgressSnaphost != PrevCurrSnapshot ){
            for(int i=ProgressSnaphost;i < PrevCurrSnapshot; i++){
                if( i % (PrevCurrSnapshot/80) == 0 ){
                    cout << "=";
                }
                if( i == PrevCurrSnapshot/4 ){
                    cout << " 25% ";
                }
                if( i == PrevCurrSnapshot/2 ){
                    cout << " 50% ";
                }
                if( i == 3*PrevCurrSnapshot/4 ){
                    cout << " 75% ";
                }
            }
            cout << "|" << endl;
            cout.flush();
        }
        PrevCurrSnapshot = -1;
        ProgressStarted = false;
    }
    if( CurrentItem >= (int)Items.size() ) return(value);

    if( ProgressStarted == false ){
        QString name = Items[CurrentItem].Name.section('/', -1);
        cout << setw(15) << left << name.toStdString() << " |";
        ProgressStarted = true;
        ProgressSnaphost = 0;
    }
    if( ProgressStarted ){
        if( ProgressSnaphost > Trajectory.GetNumberOfSnapshots() ) return(value);

        for(int i=ProgressSnaphost;i < CurrentSnapshot; i++){
            if( i % (Trajectory.GetNumberOfSnapshots()/80) == 0 ){
                cout << "=";
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
        ProgressSnaphost = CurrentSnapshot;
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


