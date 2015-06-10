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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ErrorSystem.hpp>
#include <SmallTimeAndDate.hpp>
#include "TopTrajExtract.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopTrajExtract::CTopTrajExtract(void)
{
    SnapshotID = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopTrajExtract::Init(int argc,char* argv[])
{
    // encode program options
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    // print header --------------------------------------------------------------
    if( Options.GetOptVerbose() ) {
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        printf("\n");
        printf("# ==============================================================================\n");
        printf("# toptrajextract (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        printf("# Topology name      : %s\n",(const char*)Options.GetArgTopName());
        printf("# Trajectory name    : %s\n",(const char*)Options.GetArgTrajName());
        printf("# Coordinate name    : %s\n",(const char*)Options.GetArgCrdName());
        printf("# Snapshot index     : %s\n",(const char*)Options.GetOptSnapshotNumber());
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopTrajExtract::Run(void)
{
    // load topology
    if( Topology.Load(Options.GetArgTopName()) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified topology: %s\n",
                (const char*)Options.GetArgTopName());
        return(false);
    }

    // open trajectory and init coordinates
    Coordinates.AssignTopology(&Topology);
    Coordinates.Create();
    Trajectory.AssignTopology(&Topology);
    Trajectory.AssignRestart(&Coordinates);
    if( Trajectory.OpenTrajectoryFile(Options.GetArgTrajName(),AMBER_TRAJ_UNKNOWN,AMBER_TRAJ_CXYZB,AMBER_TRAJ_READ) == false ) {
        fprintf(stderr,">>> ERROR: Unable to open specified trajectory: %s\n",
                (const char*)Options.GetArgTrajName());
        return(false);
    }

    SnapshotID = Options.GetOptSnapshotNumber().ToInt();
    if( Options.GetOptSnapshotNumber() == "first" ) {
        SnapshotID = 0;
    }
    if( Options.GetOptSnapshotNumber() == "last" ) {
        SnapshotID = -1;
    }

    int     i = 0;
    bool    finished = false;
    while( Trajectory.ReadSnapshot() == true) {
        i++;
        if( (i == 1) && (SnapshotID == 0) ) {
            finished = true;
            break;
        }
        if( i == SnapshotID ) {
            finished = true;
            break;
        }
    }
    if( (SnapshotID == -1) && (i > 0) ) {
        finished = true;
    }
    if( finished == false ) {
        fprintf(stderr,">>> ERROR: Snapshot is out-of-range (1-%d)\n",i);
        return(false);
    }

    if( Coordinates.Save(Options.GetArgCrdName()) == false ) {
        fprintf(stderr,">>> ERROR: Unable to save specified coordinates: %s\n",
                (const char*)Options.GetArgCrdName());
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopTrajExtract::Finalize(void)
{
    if( Options.GetOptVerbose() ) {
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        fprintf(stdout,"\n");
        fprintf(stdout,"# ==============================================================================\n");
        fprintf(stdout,"# %s terminated at %s\n",(const char*)Options.GetProgramName(),(const char*)dt.GetSDateAndTime());
        fprintf(stdout,"# ==============================================================================\n");
    }

    if( Options.GetOptVerbose() || ErrorSystem.IsError() ) {
        ErrorSystem.PrintErrors(stderr);
        fprintf(stdout,"\n");
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

