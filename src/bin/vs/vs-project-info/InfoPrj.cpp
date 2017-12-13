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

#include <FBDatabaseExt.hpp>
#include <FirebirdTransaction.hpp>
#include <FirebirdQuerySQL.hpp>
#include <FirebirdItem.hpp>

using namespace std;

//------------------------------------------------------------------------------

CInfoPrj InfoPrj;

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
    MsgOut.Attach(cout);
    if(Options.GetOptVerbose()) MsgOut.Verbosity(CVerboseStr::high);

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# vs-project-info started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# Database key file  : " << Options.GetArgDatabaseKey() << endl;
    MsgOut << "# Name of project    : " << Options.GetArgProjectName() << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CInfoPrj::Run(void)
{
    MsgOut << endl;
    MsgOut << ":::::::::::::::::::::::::::::::: Analysing project :::::::::::::::::::::::::::::" << endl;

    cout << "Project name                 : " << Options.GetArgProjectName() << endl;

    CFBDatabaseExt db;

    if(db.OpenByKey(Options.GetArgDatabaseKey()) == false) {
        ES_ERROR("unable to open database");
        return(false);
    }
    CSmallString    oper,descr,dynprops;
    int             ndynprops,nresults;

    if(db.GetProjectInfo(Options.GetArgProjectName(),oper,descr,ndynprops,dynprops,nresults) == false) {
        ES_ERROR("project does not exist");
        return(false);
    }
    cout << "Project operator             : " << oper << endl;
    cout << "Project description          : " << descr << endl;
    cout << "Number of dynamic properties : " << ndynprops << endl;
    cout << "Dynamic properties           : " << dynprops << endl;
    cout << "Number of results            : " << nresults << endl;
    cout << "------------------------------------------" << endl;

    CFirebirdTransaction trans;

    if(trans.AssignToDatabase(&db) == false) {
        ES_ERROR("unable to initiate transaction");
        return(false);
    }

    if(trans.StartTransaction() == false) {
        ;
        ES_ERROR("unable to initiate transaction");
        return(false);
    }

    // number of total intems
    int nitems = GetNumberFromSQL(&trans,"");
    if(nitems == -1) return(false);
    cout << "Number of items              : " << nitems << endl;

    // number of unprocessed intems
    nitems = GetNumberFromSQL(&trans,"WHERE \"Flag\" = 0");
    if(nitems == -1) return(false);
    cout << "Number of unprocessed items  : " << nitems << endl;

    // number of processing intems
    nitems = GetNumberFromSQL(&trans,"WHERE \"Flag\" = 1");
    if(nitems == -1) return(false);
    cout << "Number of processing items   : " << nitems << endl;

    // number of processed intems
    nitems = GetNumberFromSQL(&trans,"WHERE \"Flag\" = 2");
    if(nitems == -1) return(false);
    cout << "Number of processed items    : " << nitems << endl;

    if(trans.CommitTransaction() == false) {
        ES_ERROR("unable to commit transaction");
        trans.RollbackTransaction();
        return(false);
    }

    db.Logout();

    return(true);
}

//------------------------------------------------------------------------------

int CInfoPrj::GetNumberFromSQL(CFirebirdTransaction* p_trans,const CSmallString& cond)
{
    CSmallString sql;

    sql << "SELECT count(ID) FROM \"" << Options.GetArgProjectName() << "\" " << cond;

    CFirebirdQuerySQL query;

    if(query.AssignToTransaction(p_trans) == false) {
        ES_ERROR("unable to initiate sql statement");
        return(-1);
    }

    if(query.PrepareQuery(sql) == false) {
        ES_ERROR("unable to prepare sql query");
        return(-1);
    }

    if(query.ExecuteQueryOnce() == false) {
        ES_ERROR("unable to execute sql query");
        return(-1);
    }

    if(query.GetOutputItem(0) == NULL) {
        ES_ERROR("unable to get output query");
        return(-1);
    }

    return(query.GetOutputItem(0)->GetInt());
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CInfoPrj::Finalize(void)
{
    if(Options.GetOptVerbose()) {
        ErrorSystem.PrintErrors(stderr);
    }

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# vs-project-info terminated at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << endl;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

