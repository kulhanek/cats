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

#include <stdio.h>
#include <signal.h>
#include <ErrorSystem.hpp>
#include <SmallTime.hpp>
#include <CmdProcessorList.hpp>
#include <VSOperation.hpp>
#include <ExtraOperation.hpp>
#include <PrmUtils.hpp>
#include "VSServer.hpp"
#include "VSFactory.hpp"
#include <ExtraFactory.hpp>
#include <FileSystem.hpp>
#include <boost/format.hpp>

//------------------------------------------------------------------------------

using namespace std;
using namespace boost;

//------------------------------------------------------------------------------

CVSServer VSServer;

int CVSServer::NItems = 0;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVSServer::CVSServer(void)
{
    SetProtocolName("cheminfo");
    UseHiearchy = false;
    NumOfResults = 0;
    NumOfItems = 0;
    CommitTreshold = 100000;
    UnisID = "UNIS";
    SqlDB = NULL;
    SelectSTM = NULL;
    P1STM = NULL;
    P2STM = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CVSServer::Init(int argc,char* argv[])
{
// encode program options, all check procedures are done inside of CTSIntOpts
    int result = Options.ParseCmdLine(argc,argv);

// should we exit or was it error?
    if(result != SO_CONTINUE) return(result);

    // should we exit or was it error?
    if(result != SO_CONTINUE) return(result);

    // set output stream
    MsgOut.Attach(Console);
    if(Options.GetOptVerbose()) MsgOut.Verbosity(CVerboseStr::high);

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# vs-server started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << endl;

    if(Options.GetArgControlFile() != "-") {
        MsgOut << "# Control file name: " << Options.GetArgControlFile() << endl;
    } else {
        MsgOut << "# Control file name: - (standard input)" << endl;
    }

// process control file ----------------------------------
    if(Options.GetArgControlFile() != "-") {
        if(Controls.Read(Options.GetArgControlFile()) == false) {
            MsgOut << endl;
            MsgOut << "<red><b>>>> ERROR: Unable to open and read the control file!</b></red>" << endl;
            return(SO_USER_ERROR);
        }
    } else {
        if(Controls.Parse(stdin) == false) {
            MsgOut << endl;
            MsgOut << "<red><b>>>> ERROR: Unable to read the control file from the standard input!</b></red>" << endl;
            return(SO_USER_ERROR);
        }
    }

    if(ProcessServerControl(Controls) == false) {
        return(SO_USER_ERROR);
    }

// remove old server key
    CFileSystem::RemoveFile(GetServerKeyName());

    if(ProcessProjectControl(Controls) == false) {
        return(SO_USER_ERROR);
    }

    if(ProcessClientPkgControl(Controls) == false) {
        return(SO_USER_ERROR);
    }

    if(Controls.CountULines() > 0) {
        MsgOut << endl;
        MsgOut << "<red><b>>>> ERROR: Unprocessed items found in the control file!</b></red>" << endl;
        MsgOut << endl;
        Controls.Dump(stderr,true);
        return(SO_USER_ERROR);
    }

    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

bool CVSServer::ProcessProjectControl(CPrmFile& prmfile)
{
// project setup ---------------------------------
    MsgOut << endl;
    MsgOut << "=== [project] ==================================================================" << endl;

    if(prmfile.OpenSection("project") == false) {
        ES_ERROR("unable to open [project] section");
        return(false);
    }

    if(prmfile.GetStringByKey("project",ProjectName) == true) {
        MsgOut << "Project name (project)                         = " << ProjectName << endl;
    } else {
        ES_ERROR("project name (project) is not specified");
        return(false);
    }

    if(prmfile.GetStringByKey("strdir",StructureDir) == true) {
        MsgOut << "Structure database directory (strdir)          = " << StructureDir << endl;
    } else {
        ES_ERROR("structure database directory (strdir) is not specified");
        return(false);
    }

    if(prmfile.GetLogicalByKey("hiearchy",UseHiearchy) == true) {
        MsgOut << format("Use hiearchy in structure database (hiearchy)  = %6s") % bool_to_str(UseHiearchy) << endl;
    } else {
        MsgOut << format("Use hiearchy in structure database (hiearchy)  = %6s                (default)") % bool_to_str(UseHiearchy) << endl;
    }

    if(prmfile.GetIntegerByKey("selthr",CommitTreshold) == true) {
        MsgOut << format("Selection transaction commit interval (selthr) = %6d") % CommitTreshold << endl;
    } else {
        MsgOut << format("Selection transaction commit interval (selthr) = %6d                (default)") % CommitTreshold << endl;
    }

    if(prmfile.GetStringByKey("unisid",UnisID) == true) {
        MsgOut << format("Structure prefix ID (unisid)                   = %s") % UnisID << endl;
    } else {
        MsgOut << format("Structure prefix ID (unisid)                   = %6s                (default)") % UnisID << endl;
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CVSServer::ProcessClientPkgControl(CPrmFile& prmfile)
{
    // client pkg setup ---------------------------------
    ClientPackageAvailable = false;

    MsgOut << endl;
    MsgOut << "=== [client-package] ===========================================================" << endl;

    if(prmfile.OpenSection("client-package") == false) {
        MsgOut << "** INFO: The client package is not provided." << endl;
        return(true);
    }

    ClientPackageAvailable = true;
    ClientPackageDir = "client";
    if(prmfile.GetStringByKey("directory",ClientPackageDir) == true) {
        // strip down possible end slash character

        if((ClientPackageDir.GetLength() >= 1) && (ClientPackageDir[ClientPackageDir.GetLength()-1] == '/')) {
            ClientPackageDir = ClientPackageDir.GetSubString(0,ClientPackageDir.GetLength()-1);
        }

        MsgOut << format("Client package directory (directory)           = %s") % ClientPackageDir << endl;
    } else {
        MsgOut << format("Client package directory (directory)           = %s (default)") % ClientPackageDir << endl;
    }

    ClientAppName = "client-job";
    if(prmfile.GetStringByKey("appname",ClientAppName) == true) {
        MsgOut << format("Client application name (appname)              = %s") % ClientAppName << endl;
    } else {
        MsgOut << format("Client application name (appname)              = %s (default)") % ClientAppName << endl;
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSServer::Run(void)
{
    MsgOut << endl;
    MsgOut << ":::::::::::::::::::::::::::::::::: Input Setup :::::::::::::::::::::::::::::::::" << endl;

    MsgOut << "Project database    : " << ProjectName << endl;
    MsgOut << "Structure database  : " << StructureDir << endl;
    MsgOut << "Use hiearchy        : " << bool_to_str(UseHiearchy) << endl;

    MsgOut << endl;
    MsgOut << ":::::::::::::::::::::::::::::: Initializing database :::::::::::::::::::::::::::" << endl;

    // create database
    int rcode = 0;
    rcode = sqlite3_open_v2(ProjectName,&SqlDB,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,NULL);
    if( rcode != 0 ){
        ES_ERROR(sqlite3_errmsg(SqlDB));
        MsgOut << endl;
        MsgOut << "<red><b>>>> ERROR: Unable to open the '" << ProjectName << "' project database!</b></red>" << endl;
        return(false);
    }

    CSmallString sql;

    sql = "SELECT ID FROM PROJECT WHERE FLAG = 1 LIMIT 1";
    rcode = sqlite3_prepare_v2(SqlDB,sql,-1,&SelectSTM,NULL);
    if( rcode != SQLITE_OK ) {
        ES_ERROR(sql);
        ES_ERROR(sqlite3_errmsg(SqlDB));
        MsgOut << endl;
        MsgOut << "<red><b>>>> ERROR: Unable to prepare the SelectSTM SQL statement!</b></red>" << endl;
        return(false);
    }

    sql = "UPDATE PROJECT SET FLAG = 1 WHERE ID = ?";
    rcode = sqlite3_prepare_v2(SqlDB,sql,-1,&P1STM,NULL);
    if( rcode != SQLITE_OK ) {
        ES_ERROR(sql);
        ES_ERROR(sqlite3_errmsg(SqlDB));
        MsgOut << endl;
        MsgOut << "<red><b>>>> ERROR: Unable to prepare the P1STM SQL statement!</b></red>" << endl;
        return(false);
    }

    sql = "UPDATE PROJECT SET FLAG = 2 WHERE ID = ?";
    rcode = sqlite3_prepare_v2(SqlDB,sql,-1,&P2STM,NULL);
    if( rcode != SQLITE_OK ) {
        ES_ERROR(sql);
        ES_ERROR(sqlite3_errmsg(SqlDB));
        MsgOut << endl;
        MsgOut << "<red><b>>>> ERROR: Unable to prepare the P2STM SQL statement!</b></red>" << endl;
        return(false);
    }

    // number of total items
    int nitems = GetNumberFromSQL(SqlDB,"");
    if(nitems != -1) {
        cout << "Number of items              : " << nitems << endl;

        // number of unprocessed items
        nitems = GetNumberFromSQL(SqlDB,"WHERE \"Flag\" = 0");
    }
    if(nitems != -1) {
        cout << "Number of unprocessed items  : " << nitems << endl;

        // number of processing items
        nitems = GetNumberFromSQL(SqlDB,"WHERE \"Flag\" = 1");
    }
    if(nitems != -1) {
        cout << "Number of processing items   : " << nitems << endl;

        // number of processed items
        nitems = GetNumberFromSQL(SqlDB,"WHERE \"Flag\" = 2");
    }
    if(nitems != -1) {
        cout << "Number of processed items    : " << nitems << endl;
    } else {
        rcode = -1;
    }

    MsgOut << endl;
    MsgOut << ":::::::::::::::::::::::::::::::::::: VS Server :::::::::::::::::::::::::::::::::" << endl;

    // register operations
    CmdProcessorList.RegisterProcessor(Operation_GetVSData,&VSFactory);
    CmdProcessorList.RegisterProcessor(Operation_WriteVSData,&VSFactory);
    CmdProcessorList.RegisterProcessor(Operation_LoadStructure,&VSFactory);
    CmdProcessorList.RegisterProcessor(Operation_SaveStructure,&VSFactory);
    CmdProcessorList.RegisterProcessor(Operation_SelReset,&VSFactory);
    CmdProcessorList.RegisterProcessor(Operation_InstallPkg,&VSFactory);
    CmdProcessorList.RegisterProcessor(Operation_GetAppName,&VSFactory);

    // set SIGINT hadler to cleanly shutdown server ----------
    signal(SIGINT,CtrlCSignalHandler);

    // start server listening
    if(StartServer() == false) {
        return(false);
    }

    MsgOut << ":::::::::::::::::::::::::::::::: Server utilization ::::::::::::::::::::::::::::" << endl;

    MsgOut << endl;
    MsgOut << "Number of processed transactions: " << GetNumberOfTransactions() << endl;
    MsgOut << "Number of illegal transactions  : " << GetNumberOfIllegalTransactions() << endl;

    RegClients.PrintInfo();

    MsgOut << endl;
    MsgOut << "::::::::::::::::::::::::::::::::::: Finalization :::::::::::::::::::::::::::::::" << endl;
    MsgOut << endl;

    return(true);
}

//------------------------------------------------------------------------------

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    CVSServer::NItems = 0;
    if( (argc == 1) && (argv[0] != NULL) ){
        CVSServer::NItems = atoi(argv[0]);
    }
    return 0;
}

//------------------------------------------------------------------------------

int CVSServer::GetNumberFromSQL(sqlite3* sqldb,const CSmallString& cond)
{
    CSmallString sql;

    sql << "SELECT count(ID) FROM PROJECT " << cond;

    char*   zErrMsg;
    int     rcode;

    rcode = sqlite3_exec(sqldb, sql, callback, 0, &zErrMsg);
    if( rcode != SQLITE_OK ){
        ES_ERROR(zErrMsg);
        sqlite3_free(zErrMsg);
        MsgOut << endl;
        MsgOut << "<red><b>>>> ERROR: Unable to query the project information!</b></red>" << endl;
        return(-1);
    }

    return(NItems);
}

//------------------------------------------------------------------------------

bool CVSServer::Finalize(void)
{
    // clean database connection
    if( SelectSTM ) sqlite3_finalize(SelectSTM);
    if( P1STM ) sqlite3_finalize(P1STM);
    if( P2STM ) sqlite3_finalize(P2STM);
    if( SqlDB ) sqlite3_close(SqlDB);

    // remove current server key
    CFileSystem::RemoveFile(GetServerKeyName());

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# vs-server terminated at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;

    if(ErrorSystem.IsError() || Options.GetOptVerbose()) {
        ErrorSystem.PrintErrors(stderr);
    }

    MsgOut << endl;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CVSServer::CtrlCSignalHandler(int signal)
{
    printf("\nCtrl+C signal recieved.\n   Initiating server shutdown ... \n");
    if(VSServer.Options.GetOptVerbose()) printf("Waiting for server finalization ... \n");
    fflush(stdout);
    VSServer.TerminateServer();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
