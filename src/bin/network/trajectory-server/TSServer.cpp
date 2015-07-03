// =============================================================================
// PMFLib - Library Supporting Potential of Mean Force Calculations
// -----------------------------------------------------------------------------
//    Copyright (C) 2015 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu
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

#include <stdio.h>
#include <signal.h>
#include <ErrorSystem.hpp>
#include <SmallTime.hpp>
#include <CmdProcessorList.hpp>
#include <CATsOperation.hpp>
#include <PrmUtils.hpp>
#include "TSServer.hpp"
#include "TSProcessor.hpp"
#include "TSFactory.hpp"
#include <iomanip>
#include <FileSystem.hpp>
#include <FileName.hpp>
#include <boost/format.hpp>

using namespace std;
using boost::format;

//------------------------------------------------------------------------------

CTSServer TSServer;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTSServer::CTrajPoolItem::CTrajPoolItem(void)
{
    NumOfSnapshots = -1;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTSServer::CTSServer(void)
{
    SnapshotIndex = 0;
    CurrentItem = -1;
    SetProtocolName("trj");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTSServer::Init(int argc,char* argv[])
{
// encode program options, all check procedures are done inside of CABFIntOpts
    int result = Options.ParseCmdLine(argc,argv);

// should we exit or was it error?
    if(result != SO_CONTINUE) return(result);

// attach verbose stream to terminal stream and set desired verbosity level
    vout.Attach(Console);
    if( Options.GetOptVerbose() ) {
        vout.Verbosity(CVerboseStr::debug);
    } else {
        vout.Verbosity(CVerboseStr::high);
    }

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << high;
    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# trajectory-server started at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;
    vout << "#" << endl;
    if( Options.GetArgControlFile() != "-" ) {
        vout << "# Control file name: " << Options.GetArgControlFile() << endl;
    } else {
        vout << "# Control file name: - (standard input)" << endl;
    }
    vout << "#" << endl;
    vout << "# ------------------------------------------------------------------------------" << endl;

    // process control file ----------------------------------
    if( Options.GetArgControlFile() != "-" ) {
        if( Controls.Read(Options.GetArgControlFile()) == false ) {
            vout << ">>> ERROR: Unable to open and read control file!" << endl;
            return( SO_USER_ERROR );
        }
    } else {
        if( Controls.Parse(stdin) == false ) {
            CSmallString error;
            vout << ">>> ERROR: Unable to read control file from standard input!" << endl;
            return( SO_USER_ERROR );
        }
    }

    if( ProcessServerControl(Controls) == false ) {
        return( SO_USER_ERROR );
    }

    if( ProcessFileControl(Controls) == false ) {
        return( SO_USER_ERROR );
    }

    if( Controls.CountULines() > 0 ) {
        Controls.Dump(stdout,true);
        return( SO_USER_ERROR );
    }

    return( SO_CONTINUE );
}

//------------------------------------------------------------------------------

bool CTSServer::ProcessFileControl(CPrmFile& confile)
{
    // files setup ---------------------------------

    vout << "" << endl;
    vout << "=== [files] ====================================================================" << endl;
    if( confile.OpenSection("files") == false ) {
        vout << ">>> ERROR: Unable to open [files] section in control file.\n";
        return(false);
    }

    if( confile.GetStringByKey("topology",TopologyName) == true ) {
        vout << "topology                             = " << TopologyName << endl;
    } else {
        vout << ">>> ERROR: Topology name is required in control file!\n";
        return(false);
    }

    if( Topology.Load(TopologyName) == false ) {
        CSmallString error;
        error << "unable to open topology << '" << TopologyName << "'";
        ES_ERROR(error);
    }

    CSmallString trajectory_name;
    if( confile.GetStringByKey("trajectory",trajectory_name) == true ) {
        vout << "trajectory                           = " << trajectory_name << endl;
        if( AddTrajFile(trajectory_name,AMBER_TRAJ_UNKNOWN) == false ){
            vout << ">>> ERROR: Unable to add specified trajectory file!\n";
            return(false);
        }
    } else {
        CFileName path;
        CFileSystem::GetCurrentDir(path);
        confile.GetStringByKey("path",path);
        vout << "path                                = " << path << endl;
        CSmallString tmpname = "prod%03d.traj";
        confile.GetStringByKey("template",tmpname);
        vout << "template                            = " << tmpname << endl;
        int from=0;
        int to=0;
        confile.GetIntegerByKey("from",from);
        vout << "from                                = " << from << endl;
        confile.GetIntegerByKey("to",to);
        vout << "to                                  = " << to << endl;
        CSmallString fmtname = "unknown";
        confile.GetStringByKey("format",fmtname);
        vout << "format                              = " << fmtname << endl;

        for(int i=from; i <= to; i++){
            stringstream str;
            CFileName namefmt = path / tmpname;
            str << format(namefmt) % i;

            if( AddTrajFile(str.str().c_str(),fmtname) == false ){
                CSmallString error;
                if( fmtname == "unknown" ){
                    error << "unable to add file '" << str.str() << "'";
                } else {
                    error << "unable to add file '" << str.str() << "' with format '" << fmtname << "'";
                }
                vout << ">>> ERROR: " << error << endl;
                return(false);
            }
        }

    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTSServer::Run(void)
{
    vout << "" << endl;
    vout << ":::::::::::::::::::::::::::::::::: Input Data ::::::::::::::::::::::::::::::::::" << endl;

    vout << "" << endl;
    vout << "=== Topology info" << endl;
    Topology.PrintInfo(true);

    Snapshot.AssignTopology(&Topology);
    Snapshot.Create();
    Trajectory.AssignTopology(&Topology);
    Trajectory.AssignRestart(&Snapshot);

    if( Options.GetOptNoTrajInfo() == false ) {
        if( PrintTrajectoryInfo() == false ) return(false);
    }

    vout << "" << endl;
    vout << ":::::::::::::::::::::::::::::::: Trajectory Server :::::::::::::::::::::::::::::" << endl;

    // register operations
    CmdProcessorList.RegisterProcessor(Operation_GetSnapshot,&TSFactory);

    // set SIGINT hadler to cleanly shutdown server ----------
    signal(SIGINT,CtrlCSignalHandler);

    if( StartServer() == false ) {
        return(false);
    }

    vout << ":::::::::::::::::::::::::::::::: Server utilization ::::::::::::::::::::::::::::" << endl;

    vout << endl;
    vout << "Number of processed transactions: " << GetNumberOfTransactions() << endl;
    vout << "Number of illegal transactions  : " << GetNumberOfIllegalTransactions() << endl;

    RegClients.PrintInfo();

    vout << "" << endl;
    vout << "::::::::::::::::::::::::::::::::::: Finalization :::::::::::::::::::::::::::::::" << endl;
    vout << "" << endl;
    vout << "Closing trajectory ..." << endl;
    Trajectory.CloseTrajectoryFile();

    return(true);
}

//------------------------------------------------------------------------------

bool CTSServer::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << "" << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# " << Options.GetProgramName() << " terminated at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;

    if( Options.GetOptVerbose() || ErrorSystem.IsError() ){
        ErrorSystem.PrintErrors(stderr);
    }

    vout << endl;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTSServer::AddTrajFile(const CSmallString& name,const CSmallString& fmt)
{
    CTrajPoolItem item;
    item.Name = name;
    item.Format = fmt;

    CAmberTrajectory traj;
    traj.AssignTopology(&Topology);
    if( traj.OpenTrajectoryFile(name,DecodeFormat(fmt),AMBER_TRAJ_CXYZB,AMBER_TRAJ_READ) == false ){
        return(false);
    }
    item.Format = EncodeFormat(traj.GetFormat());
    item.NumOfSnapshots = traj.GetNumberOfSnapshots();
    traj.CloseTrajectoryFile();

    TrajectoryPool.push_back(item);

    return(true);
}

//------------------------------------------------------------------------------

ETrajectoryFormat CTSServer::DecodeFormat(const CSmallString& format)
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

const CSmallString CTSServer::EncodeFormat(ETrajectoryFormat format)
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

bool CTSServer::PrintTrajectoryInfo(void)
{
// execute ---------------------------------------
    cout << "=== Trajectory pool" << endl;
    cout << "# Number of items : " << TrajectoryPool.size() << endl;
    cout << "# Number of atoms : " << Trajectory.GetNumberOfAtoms() << endl;
    cout << "#" << endl;
    cout << "# Snapshots    Format   Name" << endl;
    cout << "# ---------- ---------- -----------------------------------------------------------" << endl;

    int tot_snapshots = 0;
    for(unsigned int i=0; i < TrajectoryPool.size(); i++){
        if( TrajectoryPool[i].NumOfSnapshots >= 0 ){
        if( tot_snapshots >= 0 ) tot_snapshots += TrajectoryPool[i].NumOfSnapshots;
        cout << "  " << setw(10) << right << TrajectoryPool[i].NumOfSnapshots;
        } else {
        cout << "            ";
        tot_snapshots = -1;
        }
        cout << left << " " << setw(10) << left << TrajectoryPool[i].Format;
        cout << " " << left << TrajectoryPool[i].Name << endl;
    }
    cout << "# ---------------------------------------------------------------------------------" << endl;
    cout << "# Total number of snapshots : " << tot_snapshots << endl;

    return(true);
}

//------------------------------------------------------------------------------

bool CTSServer::ReadSnapshot(void)
{
    // is pool opened
    if( CurrentItem < 0 ){
        CurrentItem = 0;
        if( CurrentItem >= (int)TrajectoryPool.size() ){
            // no items in the pool
            return(false);
        }
        if( Trajectory.OpenTrajectoryFile(TrajectoryPool[CurrentItem].Name,
                DecodeFormat(TrajectoryPool[CurrentItem].Format), AMBER_TRAJ_CXYZB,  AMBER_TRAJ_READ) == false ){
            return(false);
        }
    }

    bool result = Trajectory.ReadSnapshot(&Snapshot);
    if( result == false ) {
        Trajectory.CloseTrajectoryFile();
        CurrentItem++;
        if( CurrentItem >= (int)TrajectoryPool.size() ){
            // no items in the pool
            return(false);
        }
        if( Trajectory.OpenTrajectoryFile(TrajectoryPool[CurrentItem].Name,
                                      DecodeFormat(TrajectoryPool[CurrentItem].Format),
                                      AMBER_TRAJ_CXYZB,
                                      AMBER_TRAJ_READ) == false ){
            return(false);
        }
        // try to read again
        result = Trajectory.ReadSnapshot(&Snapshot);
    }
    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTSServer::CtrlCSignalHandler(int signal)
{
    TSServer.vout << endl;
    TSServer.vout << "Ctrl+C signal recieved." << endl;
    TSServer.vout << "Initiating server shutdown ... " << endl;
    if( TSServer.Options.GetOptVerbose() ) TSServer.vout << "Waiting for server finalization ... " << endl;
    TSServer.TerminateServer();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
