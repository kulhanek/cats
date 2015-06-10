// =============================================================================
// PMFLib - Library Supporting Potential of Mean Force Calculations
// -----------------------------------------------------------------------------
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

using namespace std;

//------------------------------------------------------------------------------

CTSServer TSServer;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTSServer::CTSServer(void)
{
    SnapshotIndex = 0;
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
    vout << "" << endl;

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

    if( confile.GetStringByKey("trajectory",TrajectoryName) == true ) {
        vout << "trajectory                           = " << TrajectoryName << endl;
    } else {
        vout << ">>> ERROR: Trajectory name is required in control file!\n";
        return(false);
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
    vout << "Topology : " << TopologyName << endl;

    if( Topology.Load(TopologyName) == false ) {
        CSmallString error;
        error << "unable to open topology << '" << TopologyName << "'";
        ES_ERROR(error);
    }
    Topology.PrintInfo(stdout);

    Snapshot.AssignTopology(&Topology);
    Snapshot.Create();
    Trajectory.AssignTopology(&Topology);
    Trajectory.AssignRestart(&Snapshot);

    vout << "Trajectory : " << TrajectoryName << endl;

    if( Options.GetOptTrajInfo() == true ) {
        if( Trajectory.PrintInfo(TrajectoryName,AMBER_TRAJ_UNKNOWN,AMBER_TRAJ_CXYZB,stdout) == false ) {
            ES_ERROR("unable to get trajectory info");
        }
    }

    if( Trajectory.OpenTrajectoryFile(TrajectoryName,AMBER_TRAJ_UNKNOWN,AMBER_TRAJ_CXYZB,AMBER_TRAJ_READ) == false ) {
        CSmallString error;
        error << "unable to open trajectory '" << TrajectoryName <<"'";
        ES_ERROR(error);
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
