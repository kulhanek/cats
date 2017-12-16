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
#include <sstream>

#include "InfoPrj.hpp"
#include "InfoPrjOptions.hpp"

#include <sqlite3.h>

using namespace std;

//------------------------------------------------------------------------------

CInfoPrj InfoPrj;

int CInfoPrj::NItems = 0;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CInfoPrj::CInfoPrj(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CInfoPrj::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CInfoPrjOptions
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if(result != SO_CONTINUE) return(result);

    // set output stream
    MsgOut.Attach(&Console);
    MsgOut.Verbosity(CVerboseStr::low);
    if(Options.GetOptVerbose()) MsgOut.Verbosity(CVerboseStr::high);

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();
    MsgOut << high;
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# svs-project-info started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# Name of project    : " << Options.GetArgProjectName() << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CInfoPrj::Run(void)
{
    MsgOut << endl;
    MsgOut << ":::::::::::::::::::::::::::::::::: Project info ::::::::::::::::::::::::::::::::" << endl;

    sqlite3* sqldb;

    // open database
    int rcode = 0;
    rcode = sqlite3_open_v2(Options.GetArgProjectName(),&sqldb,SQLITE_OPEN_READONLY,NULL);
    if( rcode != 0 ){
        ES_ERROR(sqlite3_errmsg(sqldb));
        MsgOut << endl;
        MsgOut << "<red><b>>>> ERROR: Unable to open the '" << Options.GetArgProjectName() << "' project database!</b></red>" << endl;
        return(false);
    }

    MsgOut << low;

    // number of total items
    int nitems = GetNumberFromSQL(sqldb,"");
    if(nitems != -1) {
        cout << "Number of items              : " << nitems << endl;

        // number of unprocessed items
        nitems = GetNumberFromSQL(sqldb,"WHERE \"Flag\" = 0");
    }
    if(nitems != -1) {
        cout << "Number of unprocessed items  : " << nitems << endl;

        // number of processing items
        nitems = GetNumberFromSQL(sqldb,"WHERE \"Flag\" = 1");
    }
    if(nitems != -1) {
        cout << "Number of processing items   : " << nitems << endl;

        // number of processed items
        nitems = GetNumberFromSQL(sqldb,"WHERE \"Flag\" = 2");
    }
    if(nitems != -1) {
        cout << "Number of processed items    : " << nitems << endl;
    } else {
        rcode = -1;
    }

    // close database
    sqlite3_close(sqldb);

    return(rcode == SQLITE_OK);
}

//------------------------------------------------------------------------------

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    CInfoPrj::NItems = 0;
    if( (argc == 1) && (argv[0] != NULL) ){
        CInfoPrj::NItems = atoi(argv[0]);
    }
    return 0;
}

//------------------------------------------------------------------------------

int CInfoPrj::GetNumberFromSQL(sqlite3* sqldb,const CSmallString& cond)
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

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CInfoPrj::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << high;
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# svs-project-info terminated at " << dt.GetSDateAndTime() << endl;
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

