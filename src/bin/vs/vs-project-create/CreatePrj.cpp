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

#include "CreatePrj.hpp"
#include "CreatePrjOptions.hpp"

#include <FBDatabaseExt.hpp>
#include <FirebirdTransaction.hpp>
#include <FirebirdExecuteSQL.hpp>
#include <FirebirdQuerySQL.hpp>
#include <FirebirdItem.hpp>

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
    MsgOut.Attach(cout);
    if(Options.GetOptVerbose()) MsgOut.Verbosity(CVerboseStr::high);

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# vs-project-create started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# Database key file  : " << Options.GetArgDatabaseKey() << endl;
    MsgOut << "# Name of project    : " << Options.GetArgProjectName() << endl;
    MsgOut << "# ------------------------------------------------------------------------------" << endl;
    MsgOut << "# Description        : " << Options.GetOptDescription() << endl;
    MsgOut << "# Operator           : " << Options.GetOptOperator() << endl;
    MsgOut << "# Dynamic properties : " << Options.GetOptDynamicProperties() << endl;
    MsgOut << "# Number of results  : " << Options.GetOptNumOfResults() << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCreatePrj::Run(void)
{
    MsgOut << endl;
    MsgOut << ":::::::::::::::::::::::::::::::: Creating project ::::::::::::::::::::::::::::::" << endl;

    // ********* PREPARE TRANSACTION **************
    CFBDatabaseExt db;

    if(db.OpenByKey(Options.GetArgDatabaseKey()) == false) {
        ES_ERROR("unable to open database");
        return(false);
    }

    CFirebirdTransaction trans;

    if(trans.AssignToDatabase(&db) == false) {
        ES_ERROR("unable to initiate transaction");
        return(false);
    }

    if(trans.StartTransaction() == false) {
        ES_ERROR("unable Options.GetArgProjectNameto initiate transaction");
        return(false);
    }

    // ********* CHECK FOR PROJECT WITH THE SAME NAME **************
    CSmallString sql;

    sql << "SELECT \"Name\" FROM \"ROOT\" WHERE \"Name\" = ?";

    CFirebirdQuerySQL query;

    if(query.AssignToTransaction(&trans) == false) {
        ES_ERROR("unable to initiate sql statement");
        return(false);
    }

    if(query.PrepareQuery(sql) == false) {
        ES_ERROR("unable to prepare sql query");
        return(false);
    }

    query.GetInputItem(0)->SetString(Options.GetArgProjectName());

    if(query.ExecuteQueryOnce() == true) {
        // record found - project exist -> exit
        ES_ERROR("project already exists");
        return(false);
    }

    // -------------------------------------------
    CFirebirdExecuteSQL exsql;
    if(exsql.AssignToTransaction(&trans) == false) {
        ES_ERROR("unable to initiate sqlexec statement");
        return(false);
    }

    // get number of dynamic properties
    int ndynprops = 0;
    CSmallString props1(Options.GetOptDynamicProperties());
    char* p_str = strtok(props1.GetBuffer(),",");
    while(p_str != NULL) {
        ndynprops++;
        p_str = strtok(NULL,",");
    }

    // ********* UPDATE ROOT TABLE **************
    sql = "";
    sql << "INSERT INTO \"ROOT\" (\"Name\",\"Description\",\"Operator\",\"NDynProps\",\"DynProps\",\"NResults\") ";
    sql << "VALUES (?,?,?,?,?,?)";

    if(exsql.AllocateInputItems(6) == false) {
        ES_ERROR("unable to allocate items for exsql");
        return(false);
    }

    exsql.GetInputItem(0)->SetString(Options.GetArgProjectName());
    exsql.GetInputItem(1)->SetString(Options.GetOptDescription());
    exsql.GetInputItem(2)->SetString(Options.GetOptOperator());
    exsql.GetInputItem(3)->SetInt(ndynprops);
    exsql.GetInputItem(4)->SetString(Options.GetOptDynamicProperties());
    exsql.GetInputItem(5)->SetInt(Options.GetOptNumOfResults());

    if(exsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql statement");
        return(false);
    }

    // ********* CREATE PROJECT TABLE **************
    sql = "";
    sql << "CREATE TABLE \"" << Options.GetArgProjectName() << "\" (";

    // static items
    sql << "  \"ID\"        char(8) NOT NULL, ";             // molecule ID
    sql << "  \"Flag\"      integer, ";              // processing flag

    // static properties
    sql << "  \"InChiKey\"  char(29), ";            // InChi identifier Key
    sql << "  \"NA\"        integer, ";             // number of atoms
    sql << "  \"TC\"        integer, ";             // total charge
    sql << "  \"MW\"        double precision ";    // molecular weight

    // dynamic properties
    CSmallString props2(Options.GetOptDynamicProperties());
    p_str = strtok(props2.GetBuffer(),",");
    while(p_str != NULL) {
        sql << ", \"" << p_str << "\" double precision";
        p_str = strtok(NULL,",");
    }

    // results
    for(int i=0; i < Options.GetOptNumOfResults(); i++) {
        sql << ", \"R" << i+1 << "\" double precision";
    }
    sql << ")";

    if(exsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql statement");
        return(false);
    }

    if(trans.CommitTransaction() == false) {
        ES_ERROR("unable to commit transaction");
        trans.RollbackTransaction();
        return(false);
    }

    db.Logout();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCreatePrj::Finalize(void)
{
    if(Options.GetOptVerbose()) {
        ErrorSystem.PrintErrors(stderr);
    }

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# vs-project-create terminated at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << endl;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

