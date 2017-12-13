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

/* Firebird Thread-Safety

http://kinterbasdb.sourceforge.net/dist_docs/firebird-client-library-thread-safety-overview.html

There are two database connections:
a) selection of unprocessed structures
b) database manipulation

The acces to both of them is protected by two independent mutexes.
*/

//------------------------------------------------------------------------------

CVSServer VSServer;

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
    UnisID = "ZINC";
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

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    printf("\n");
    printf("# ==============================================================================\n");
    printf("# vs-server started at %s\n",(const char*)dt.GetSDateAndTime());
    printf("# ==============================================================================\n");
    printf("#\n");
    if(Options.GetArgControlFile() != "-") {
        printf("# Control file name: %s\n",(const char*)Options.GetArgControlFile());
    } else {
        printf("# Control file name: - (standard input)\n");
    }

// process control file ----------------------------------
    if(Options.GetArgControlFile() != "-") {
        if(Controls.Read(Options.GetArgControlFile()) == false) {
            CSmallString error;
            error << ">>> ERROR: Unable to open and read control file!\n";
            fprintf(stderr,"%s",(const char*)error);
            return(SO_USER_ERROR);
        }
    } else {
        if(Controls.Parse(stdin) == false) {
            CSmallString error;
            error << ">>> ERROR: Unable to read control file from standard input!\n";
            fprintf(stderr,"%s",(const char*)error);
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
        fprintf(stderr,"\n>> ERROR: Unprocessed items found in control file!\n");
        Controls.Dump(stderr,true);
        return(SO_USER_ERROR);
    }

    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

bool CVSServer::ProcessProjectControl(CPrmFile& prmfile)
{
// project setup ---------------------------------

    printf("\n=== [project] ==================================================================\n");
    if(prmfile.OpenSection("project") == false) {
        ES_ERROR("unable to open [project] section");
        return(false);
    }

    if(prmfile.GetStringByKey("databasekey",DatabaseKey) == true) {
        printf("Database key file name (databasekey)           = %s\n",(const char*)DatabaseKey);
    } else {
        ES_ERROR("database key (databasekey) is not specified");
        return(false);
    }

    if(prmfile.GetStringByKey("project",ProjectName) == true) {
        printf("Project name (project)                         = %s\n",(const char*)ProjectName);
    } else {
        ES_ERROR("project name (project) is not specified");
        return(false);
    }

    if(prmfile.GetStringByKey("strdir",StructureDir) == true) {
        printf("Structure database directory (strdir)          = %s\n",(const char*)StructureDir);
    } else {
        ES_ERROR("structure database directory (strdir) is not specified");
        return(false);
    }

    if(prmfile.GetLogicalByKey("hiearchy",UseHiearchy) == true) {
        printf("Use hiearchy in structure database (hiearchy)  = %6s\n",bool_to_str(UseHiearchy));
    } else {
        printf("Use hiearchy in structure database (hiearchy)  = %6s                (default)\n",bool_to_str(UseHiearchy));
    }

    if(prmfile.GetIntegerByKey("selthr",CommitTreshold) == true) {
        printf("Selection transaction commit interval (selthr) = %6d\n",CommitTreshold);
    } else {
        printf("Selection transaction commit interval (selthr) = %6d                (default)\n",CommitTreshold);
    }

    if(prmfile.GetStringByKey("filter",Filter) == true) {
        printf("Selection filter (filter)                      = %s\n",(const char*)Filter);
    } else {
        printf("Selection filter (filter)                      = %6s                (default)\n",(const char*)Filter);
    }

    if(prmfile.GetStringByKey("unisid",UnisID) == true) {
        printf("Structure prefix ID (unisid)                   = %s\n",(const char*)UnisID);
    } else {
        printf("Structure prefix ID (unisid)                   = %6s                (default)\n",(const char*)UnisID);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CVSServer::ProcessClientPkgControl(CPrmFile& prmfile)
{
    // client pkg setup ---------------------------------
    ClientPackageAvailable = false;
    printf("\n=== [client-package] ===========================================================\n");
    if(prmfile.OpenSection("client-package") == false) {
        printf("** INFO: Client package is not provided.\n");
        return(true);
    }

    ClientPackageAvailable = true;
    ClientPackageDir = "client";
    if(prmfile.GetStringByKey("directory",ClientPackageDir) == true) {
        // strip down possible end slash character

        if((ClientPackageDir.GetLength() >= 1) && (ClientPackageDir[ClientPackageDir.GetLength()-1] == '/')) {
            ClientPackageDir = ClientPackageDir.GetSubString(0,ClientPackageDir.GetLength()-1);
        }

        printf("Client package directory (directory)           = %s\n",(const char*)ClientPackageDir);
    } else {
        printf("Client package directory (directory)           = %s (default)\n",(const char*)ClientPackageDir);
    }

    ClientAppName = "client-job";
    if(prmfile.GetStringByKey("appname",ClientAppName) == true) {
        printf("Client application name (appname)              = %s\n",(const char*)ClientAppName);
    } else {
        printf("Client application name (appname)              = %s (default)\n",(const char*)ClientAppName);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSServer::Run(void)
{
    printf("\n");
    printf(":::::::::::::::::::::::::::::::::: Input Setup :::::::::::::::::::::::::::::::::\n");
    printf("Database key                  : %s\n",(const char*)DatabaseKey);

    if(SelDBCon.OpenByKey(DatabaseKey) == false) {
        ES_ERROR("unable to connect to database");
        return(false);
    }

    if(ManipDBCon.OpenByKey(DatabaseKey) == false) {
        ES_ERROR("unable to connect to database");
        return(false);
    }

    printf("Database                      : %s\n",(const char*)SelDBCon.GetDatabaseName());
    printf("Structure database directory  : %s\n",(const char*)StructureDir);
    printf("Use hiearchy in str database  : %s\n",bool_to_str(UseHiearchy));

    printf("\n");
    printf(":::::::::::::::::::::::::::::::::: Project info ::::::::::::::::::::::::::::::::\n");

    printf("Project                       : %s\n",(const char*)ProjectName);

    CSmallString    oper,descr;
    int             ndynprops;

    if(SelDBCon.GetProjectInfo(ProjectName,oper,descr,ndynprops,DynamicProperties,NumOfResults) == false) {
        ES_ERROR("project does not exist");
        return(false);
    }
    printf("Project operator              : %s\n",(const char*)oper);
    printf("Project description           : %s\n",(const char*)descr);
    printf("Number of dynamic properties  : %d\n",ndynprops);
    printf("Dynamic properties            : %s\n",(const char*)DynamicProperties);
    printf("Number of results             : %d\n",NumOfResults);

    printf("\n");
    printf(":::::::::::::::::::::::::::::: Initializing database :::::::::::::::::::::::::::\n");

    if(ManipTransaction.AssignToDatabase(&ManipDBCon) == false) {
        ES_ERROR("unable to assign manip transaction");
        return(false);
    }

    if(SelTransaction.AssignToDatabase(&SelDBCon) == false) {
        ES_ERROR("unable to assign sel transaction");
        return(false);
    }

    if(SelTransaction.StartTransaction() == false) {
        ES_ERROR("unable to initiate seelction transaction");
        return(false);
    }

    CSmallString sql;

    sql << "SELECT \"ID\",\"InChiKey\",\"NA\",\"TC\",\"MW\"";

    // dynamic properties
    CSmallString props2(DynamicProperties);
    char* p_str = strtok(props2.GetBuffer(),",");
    while(p_str != NULL) {
        sql << ",\"" << p_str << "\"";
        p_str = strtok(NULL,",");
    }

    // results
    for(int i=0; i < NumOfResults; i++) {
        sql << ",\"R" << i+1 << "\"";
    }

    sql << " FROM \"" << VSServer.ProjectName << "\" WHERE \"Flag\" = 0";

    if(Filter != NULL) {
        sql << " AND ( " << Filter << " )";
    }

    if(SelQuery.AssignToTransaction(&SelTransaction) == false) {
        ES_ERROR("unable to initiate sql statement");
        SelTransaction.RollbackTransaction();
        return(false);
    }

    if(SelQuery.OpenQuery(sql) == false) {
        ES_ERROR("unable to prepare sql query");
        SelTransaction.RollbackTransaction();
        return(false);
    }

    printf("\n");
    printf(":::::::::::::::::::::::::::::::::::: VS Server :::::::::::::::::::::::::::::::::\n");

    // register operations
    CmdProcessorList.RegisterProcessor(Operation_GetData,&VSFactory);
    CmdProcessorList.RegisterProcessor(Operation_WriteData,&VSFactory);
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

    printf(":::::::::::::::::::::::::::::::: Server utilization ::::::::::::::::::::::::::::\n");

    printf("\n");
    printf("Number of processed transactions: %d\n",GetNumberOfTransactions());
    printf("Number of illegal transactions  : %d\n",GetNumberOfIllegalTransactions());

    RegClients.PrintInfo();

    printf("\n");
    printf("::::::::::::::::::::::::::::::::::: Finalization :::::::::::::::::::::::::::::::\n");
    printf("\n");

    if(SelQuery.CloseQuery() == false) {
        ES_ERROR("unable to close sql query");
    }

    if(SelTransaction.CommitTransaction() == false) {
        ES_ERROR("unable to commit seclection transaction");
        SelTransaction.RollbackTransaction();
    }

    SelDBCon.Logout();
    ManipDBCon.Logout();
    return(true);
}

//------------------------------------------------------------------------------

bool CVSServer::Finalize(void)
{
    // remove current server key
    CFileSystem::RemoveFile(GetServerKeyName());

    if(Options.GetOptVerbose()) ErrorSystem.PrintErrors(stderr);

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    fprintf(stdout,"\n");
    fprintf(stdout,"# ==============================================================================\n");
    fprintf(stdout,"# vs-server terminated at %s\n",(const char*)dt.GetSDateAndTime());
    fprintf(stdout,"# ==============================================================================\n");
    fprintf(stdout,"\n");

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

//------------------------------------------------------------------------------

bool CVSServer::CommitSelTrans(bool force)
{
    NumOfItems++;
    if((force == false) && (NumOfItems % CommitTreshold != 0)) return(true);

    SelQuery.CloseQuery();

    if(SelTransaction.CommitTransaction() == false) {
        ES_ERROR("unable to commit seclection transaction");
        SelTransaction.RollbackTransaction();
    }

    if(SelTransaction.StartTransaction() == false) {
        ES_ERROR("unable to initiate seelction transaction");
        return(false);
    }

    CSmallString sql;

    sql << "SELECT \"ID\",\"InChiKey\",\"NA\",\"TC\",\"MW\"";

    // dynamic properties
    CSmallString props2(DynamicProperties);
    char* p_str = strtok(props2.GetBuffer(),",");
    while(p_str != NULL) {
        sql << ",\"" << p_str << "\"";
        p_str = strtok(NULL,",");
    }

    // results
    for(int i=0; i < NumOfResults; i++) {
        sql << ",\"R" << i+1 << "\"";
    }

    sql << " FROM \"" << VSServer.ProjectName << "\" WHERE \"Flag\" = 0";

    if(Filter != NULL) {
        sql << " AND ( " << Filter << " )";
    }

    if(SelQuery.AssignToTransaction(&SelTransaction) == false) {
        ES_ERROR("unable to initiate sql statement");
        SelTransaction.RollbackTransaction();
        return(false);
    }

    if(SelQuery.OpenQuery(sql) == false) {
        ES_ERROR("unable to prepare sql query");
        SelTransaction.RollbackTransaction();
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
