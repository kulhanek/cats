#ifndef TSServerH
#define TSServerH
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

#include <ExtraServer.hpp>
#include <PrmFile.hpp>
#include <RegClientList.hpp>
#include <ResultFile.hpp>
#include <AmberTopology.hpp>
#include <AmberRestart.hpp>
#include <AmberTrajectory.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>
#include <vector>

#include "TSServerOptions.hpp"

//------------------------------------------------------------------------------

class CTSRegClient;

//------------------------------------------------------------------------------

class CTSServer : public CExtraServer {
public:
    // constructor
    CTSServer(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    bool Finalize(void);

    /// add data to global accumulator
    bool AddDataToTSAccu(CTSRegClient* p_client);

// section of private data ----------------------------------------------------
private:
    CTSServerOptions    Options;        // program options
    CTerminalStr        Console;
    CVerboseStr         vout;

    CPrmFile            Controls;           // controls
    CSmallString        TopologyName;       // topology name

    class CTrajPoolItem {
    public:
        CTrajPoolItem(void);
        CSmallString    Name;
        CSmallString    Format;
        int             NumOfSnapshots;
    };
    std::vector<CTrajPoolItem>  TrajectoryPool;
    int                         CurrentItem;

    // global data -------------------------------
    CAmberTopology      Topology;
    CAmberTrajectory    Trajectory;         // input trajectory
    CAmberRestart       Snapshot;
    CSimpleMutex        TrajectoryMutex;
    int                 SnapshotIndex;

    /// Ctrl+C signal handler
    static void CtrlCSignalHandler(int signal);

    /// process control file
    bool ProcessFileControl(CPrmFile& confile);

    /// trajectory helper methods
    bool AddTrajFile(const CSmallString& name,const CSmallString& fmt);
    ETrajectoryFormat DecodeFormat(const CSmallString& format);
    const CSmallString EncodeFormat(ETrajectoryFormat format);
    bool PrintTrajectoryInfo(void);
    bool ReadSnapshot(void);

    friend class CTSProcessor;
};

//------------------------------------------------------------------------------

extern CTSServer TSServer;

//------------------------------------------------------------------------------

#endif
