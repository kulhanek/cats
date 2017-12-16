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

#include "AlterPrj.hpp"
#include "AlterPrjOptions.hpp"

#include <sqlite3.h>

using namespace std;

//------------------------------------------------------------------------------

CAlterPrj AlterPrj;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAlterPrj::CAlterPrj(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CAlterPrj::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CAlterPrjOptions
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
    MsgOut << "# svs-project-alter started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# Name of project    : " << Options.GetArgProjectName() << endl;
    MsgOut << "# Action             : " << Options.GetArgCommand() << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CAlterPrj::Run(void)
{
    MsgOut << endl;
    MsgOut << ":::::::::::::::::::::::::::::::: Altering project ::::::::::::::::::::::::::::::" << endl;

    sqlite3* sqldb;

    // open database
    int rcode = 0;
    rcode = sqlite3_open_v2(Options.GetArgProjectName(),&sqldb,SQLITE_OPEN_READWRITE,NULL);
    if( rcode != 0 ){
        ES_ERROR(sqlite3_errmsg(sqldb));
        MsgOut << endl;
        MsgOut << "<red><b>>>> ERROR: Unable to open the '" << Options.GetArgProjectName() << "' project database!</b></red>" << endl;
        return(false);
    }

    bool result;

    //------------------------------------------------------
    if(Options.GetArgCommand() == "softreset") {
        result = ExecSQL(sqldb,"UPDATE PROJECT SET FLAG = 0 WHERE FLAG = 1");

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "hardreset") {
        result = ExecSQL(sqldb,"UPDATE PROJECT SET FLAG = 0");

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "finalizeall") {
        result = ExecSQL(sqldb,"UPDATE PROJECT SET FLAG = 2");

        //------------------------------------------------------
    }  else if(Options.GetArgCommand() == "addindx") {
        result = ExecSQL(sqldb,"CREATE UNIQUE INDEX idindx ON PROJECT(ID)");

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "removeindx") {
        result = ExecSQL(sqldb,"DROP INDEX idindx");

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "rebuildindx") {
        result = ExecSQL(sqldb,"DROP INDEX idindx");
        result &= ExecSQL(sqldb,"CREATE UNIQUE INDEX idindx ON PROJECT(ID)");

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "deleteall") {
        result = ExecSQL(sqldb,"DELETE FROM PROJECT");

        //------------------------------------------------------
    }  else {
        ES_ERROR("not implemented");
        result = false;
    }

    // close database
    sqlite3_close(sqldb);

    if( result ) {
        MsgOut << "Done." << endl;
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CAlterPrj::ExecSQL(sqlite3* sqldb,const CSmallString& sql)
{
    char*   zErrMsg;
    int     rcode;

    rcode = sqlite3_exec(sqldb, sql, NULL, 0, &zErrMsg);
    if( rcode != SQLITE_OK ){
        ES_ERROR(sql);
        ES_ERROR(zErrMsg);
        sqlite3_free(zErrMsg);
        MsgOut << endl;
        MsgOut << "<red><b>>>> ERROR: Unable to execute the SQL statement!</b></red>" << endl;
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CAlterPrj::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# svs-project-alter terminated at " << dt.GetSDateAndTime() << endl;
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

