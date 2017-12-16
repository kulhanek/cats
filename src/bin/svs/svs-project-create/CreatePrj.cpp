// =============================================================================
// VScreen - Virtual Screening Tools
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
#include <ErrorSystem.hpp>
#include <SmallTime.hpp>
#include <SmallTimeAndDate.hpp>
#include <FileSystem.hpp>

#include "CreatePrj.hpp"
#include "CreatePrjOptions.hpp"

#include <sqlite3.h>

using namespace std;

//------------------------------------------------------------------------------

CCreatePrj CreatePrj;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCreatePrj::CCreatePrj(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CCreatePrj::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CCreatePrjOptions
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if(result != SO_CONTINUE) return(result);

    // set output stream
    MsgOut.Attach(Console);
    MsgOut.Verbosity(CVerboseStr::low);
    if(Options.GetOptVerbose()) MsgOut.Verbosity(CVerboseStr::high);

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();
    MsgOut << high;
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# svs-project-create started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# Name of project    : " << Options.GetArgProjectName() << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCreatePrj::Run(void)
{
    MsgOut << endl;
    MsgOut << ":::::::::::::::::::::::::::::::: Creating project ::::::::::::::::::::::::::::::" << endl;

    if( CFileSystem::IsFile(Options.GetArgProjectName()) == true ){
        ES_ERROR("project already exists");
        MsgOut << endl;
        MsgOut << "<red><b>>>> ERROR: The file '" << Options.GetArgProjectName() << "' already exists!</b></red>" << endl;
        return(false);
    }

    sqlite3* sqldb;

    // create database
    int rcode = 0;
    rcode = sqlite3_open_v2(Options.GetArgProjectName(),&sqldb,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,NULL);
    if( rcode != 0 ){
        ES_ERROR(sqlite3_errmsg(sqldb));
        MsgOut << endl;
        MsgOut << "<red><b>>>> ERROR: Unable to create the '" << Options.GetArgProjectName() << "' project database!</b></red>" << endl;
        return(false);
    }

    // ********* CREATE PROJECT TABLE **************
    CSmallString sql = "";
    sql << "CREATE TABLE PROJECT (ID char(8) NOT NULL, FLAG integer)";

    char* zErrMsg;
    rcode = sqlite3_exec(sqldb, sql, NULL, 0, &zErrMsg);
    if( rcode != SQLITE_OK ){
        ES_ERROR(zErrMsg);
        sqlite3_free(zErrMsg);
        MsgOut << endl;
        MsgOut << "<red><b>>>> ERROR: Unable to create the project table!</b></red>" << endl;
    }

    if( rcode == SQLITE_OK ) {
        // ********* ADD PRIMARY INDEX
        sql = "CREATE UNIQUE INDEX idindx ON PROJECT(ID)";
        rcode = sqlite3_exec(sqldb, sql, NULL, 0, &zErrMsg);
        if( rcode != SQLITE_OK ){
            ES_ERROR(zErrMsg);
            sqlite3_free(zErrMsg);
            MsgOut << endl;
            MsgOut << "<red><b>>>> ERROR: Unable to set primary index!</b></red>" << endl;
        }
    }

    // close database
    sqlite3_close(sqldb);

    if( rcode == SQLITE_OK ) {
        MsgOut << "Done." << endl;
    }

    return(rcode == SQLITE_OK);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCreatePrj::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# svs-project-create terminated at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;

    if(ErrorSystem.IsError() || Options.GetOptVerbose()) {
        ErrorSystem.PrintErrors(stderr);
        fprintf(stderr,"\n");
    } else{
        MsgOut << endl;
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

