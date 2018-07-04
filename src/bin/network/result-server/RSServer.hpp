#ifndef RSServerH
#define RSServerH
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
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>

#include "RSServerOptions.hpp"

//------------------------------------------------------------------------------

class CRSRegClient;

//------------------------------------------------------------------------------

class CRSServer : public CExtraServer {
public:
    // constructor
    CRSServer(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    bool Finalize(void);

    /// add data to global accumulator
    bool AddDataToRSAccu(CRSRegClient* p_client);

// section of private data ----------------------------------------------------
private:
    CRSServerOptions    Options;        // program options
    CPrmFile            Controls;       // controls
    bool                Statistics;         // is statistics on?
    CSmallString        ResultsFileName;    // results file name
    CSmallString        StatisticsFileName; // output file name

    // global data -------------------------------
    CRegClientList      RegClients;         // registered clients
    CResultFile         ResultFile;         // results

    /// Ctrl+C signal handler
    static void CtrlCSignalHandler(int signal);

    /// process control file
    bool ProcessFileControl(CPrmFile& conflie);

    friend class CRSProcessor;
};

//------------------------------------------------------------------------------

extern CRSServer RSServer;

//------------------------------------------------------------------------------

#endif
