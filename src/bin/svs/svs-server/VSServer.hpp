#ifndef VSServerH
#define VSServerH
// =============================================================================
// ChemInfo - Chemoinformatics Tools
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

#include <ExtraServer.hpp>
#include <PrmFile.hpp>
#include <RegClientList.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>
#include <sqlite3.h>

#include "VSServerOptions.hpp"

//------------------------------------------------------------------------------

class CVSServer : public CExtraServer {
public:
    // constructor
    CVSServer(void);

// main methods ---------------------------------------------------------------
    //! init options
    int Init(int argc,char* argv[]);

    //! main part of program
    bool Run(void);

    //! finalize program
    bool Finalize(void);

// section of public data ------------------------------------------------------
public:
    CVSServerOptions    Options;        // program options
    static int          NItems;

// section of private data ----------------------------------------------------
private:
    CTerminalStr        Console;
    CVerboseStr         MsgOut;     // output messages

    CPrmFile            Controls;       // controls
    CSmallString        ProjectName;    // project name
    CSmallString        StructureDir;   // where structures are located
    bool                UseHiearchy;    // use hiearchy storage of structures
    int                 NumOfResults;   // number of results

    // client package -----------------------------
    bool                ClientPackageAvailable;
    CSmallString        ClientPackageDir;
    CSmallString        ClientAppName;

    // DB access -------------------------------
    CSimpleMutex        DBMutex;
    sqlite3*            SqlDB;
    sqlite3_stmt*       SelectSTM;
    sqlite3_stmt*       P1STM;
    sqlite3_stmt*       P2STM;
    int                 NumOfItems;
    CSmallString        UnisID;

    //! Ctrl+C signal handler
    static void CtrlCSignalHandler(int signal);

    //! process project control file
    bool ProcessProjectControl(CPrmFile& confile);

    //! process client package control section
    bool ProcessClientPkgControl(CPrmFile& prmfile);

    //! get project db stat
    int GetNumberFromSQL(sqlite3* sqldb,const CSmallString& cond);

    friend class CVSProcessor;
};

//------------------------------------------------------------------------------

extern CVSServer VSServer;

//------------------------------------------------------------------------------

#endif
