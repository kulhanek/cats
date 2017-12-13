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

#include <FBDatabaseExt.hpp>
#include <FirebirdTransaction.hpp>
#include <FirebirdExecuteSQL.hpp>
#include <FirebirdItem.hpp>

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
    MsgOut.Attach(cout);
    if(Options.GetOptVerbose()) MsgOut.Verbosity(CVerboseStr::high);

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# vs-project-alter started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# Database key file  : " << Options.GetArgDatabaseKey() << endl;
    MsgOut << "# Name of project    : " << Options.GetArgProjectName() << endl;
    MsgOut << "# Action             : " << Options.GetArgCommand() << endl;
    MsgOut << "# Values             : " << Options.GetOptValue() << endl;
    MsgOut << "# ------------------------------------------------------------------------------" << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CAlterPrj::Run(void)
{
    MsgOut << endl;
    MsgOut << ":::::::::::::::::::::::::::::::: Altering project ::::::::::::::::::::::::::::::" << endl;

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
    MsgOut << "# Project operator             : " << oper << endl;
    MsgOut << "# Project description          : " << descr << endl;
    MsgOut << "# Number of dynamic properties : " << ndynprops << endl;
    MsgOut << "# Dynamic properties           : " << dynprops << endl;
    MsgOut << "# Number of results            : " << nresults << endl;
    MsgOut << "# -----------------------------------------------------" << endl;

    CFirebirdTransaction trans;

    if(trans.AssignToDatabase(&db) == false) {

        ES_ERROR("unable to initiate transaction");
        return(false);
    }

    if(trans.StartTransaction() == false) {
        ES_ERROR("unable to initiate transaction");
        return(false);
    }

    bool result;

    //------------------------------------------------------
    if(Options.GetArgCommand() == "softreset") {
        result = SoftReset(&trans);

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "hardreset") {
        result = HardReset(&trans);

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "finalizeall") {
        result = FinalizeAll(&trans);

        //------------------------------------------------------
    }  else if(Options.GetArgCommand() == "addindx") {
        result = AddIndex(&trans);

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "removeindx") {
        result = RemoveIndex(&trans);

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "rebuildindx") {
        result = RemoveIndex(&trans);
        result &= AddIndex(&trans);

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "deleteall") {
        result = DeleteAll(&trans);

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "addres") {
        result = AddResultColumn(&trans,nresults);

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "rmres") {
        result = RemoveResultColumn(&trans,nresults);

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "operator") {
        result = AlterOperator(&trans);

        //------------------------------------------------------
    } else if(Options.GetArgCommand() == "description") {
        result = AlterDescription(&trans);

        //------------------------------------------------------
    } else {
        ES_ERROR("not implemented");
        result = false;
    }

    if(result == true) {
        if(trans.CommitTransaction() == false) {
            ES_ERROR("unable to commit transaction");
            trans.RollbackTransaction();
            return(false);
        }
    } else {
        trans.RollbackTransaction();
    }

    db.Logout();

    return(result);
}

//------------------------------------------------------------------------------

bool CAlterPrj::SoftReset(CFirebirdTransaction* p_trans)
{
    CSmallString sql;

    sql << "UPDATE \"" << Options.GetArgProjectName() << "\" SET \"Flag\" = 0 WHERE  \"Flag\" = 1";

    CFirebirdExecuteSQL execsql;

    if(execsql.AssignToTransaction(p_trans) == false) {
        ES_ERROR("unable to initiate sql statement");
        return(false);
    }

    if(execsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql query");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CAlterPrj::HardReset(CFirebirdTransaction* p_trans)
{
    CSmallString sql;

    sql << "UPDATE \"" << Options.GetArgProjectName() << "\" SET \"Flag\" = 0";

    CFirebirdExecuteSQL execsql;

    if(execsql.AssignToTransaction(p_trans) == false) {
        ES_ERROR("unable to initiate sql statement");
        return(false);
    }

    if(execsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql query");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CAlterPrj::FinalizeAll(CFirebirdTransaction* p_trans)
{
    CSmallString sql;

    sql << "UPDATE \"" << Options.GetArgProjectName() << "\" SET \"Flag\" = 2";

    CFirebirdExecuteSQL execsql;

    if(execsql.AssignToTransaction(p_trans) == false) {
        ES_ERROR("unable to initiate sql statement");
        return(false);
    }

    if(execsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql query");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CAlterPrj::AddIndex(CFirebirdTransaction* p_trans)
{
    CSmallString sql;

    sql << "CREATE UNIQUE INDEX \"" << Options.GetArgProjectName() << "_indx\" ON \"" << Options.GetArgProjectName() << "\" ( \"ID\")";

    CFirebirdExecuteSQL execsql;

    if(execsql.AssignToTransaction(p_trans) == false) {
        ES_ERROR("unable to initiate sql statement");
        return(false);
    }

    if(execsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql query");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CAlterPrj::RemoveIndex(CFirebirdTransaction* p_trans)
{
    CSmallString sql;

    sql << "DROP INDEX \"" << Options.GetArgProjectName() << "_indx\"";

    CFirebirdExecuteSQL execsql;

    if(execsql.AssignToTransaction(p_trans) == false) {
        ES_ERROR("unable to initiate sql statement");
        return(false);
    }

    if(execsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql query");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CAlterPrj::DeleteAll(CFirebirdTransaction* p_trans)
{
    CSmallString sql;

    sql << "DELETE FROM \"" << Options.GetArgProjectName() << "\"";

    CFirebirdExecuteSQL execsql;

    if(execsql.AssignToTransaction(p_trans) == false) {
        ES_ERROR("unable to initiate sql statement");
        return(false);
    }

    if(execsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql query");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CAlterPrj::AddResultColumn(CFirebirdTransaction* p_trans,int old_num_of_results)
{
    CSmallString sql;

    sql << "ALTER TABLE \"" << Options.GetArgProjectName() << "\" ADD \"R" << old_num_of_results + 1 << "\" double precision";

    CFirebirdExecuteSQL execsql;

    if(execsql.AssignToTransaction(p_trans) == false) {
        ES_ERROR("unable to initiate sql statement");
        return(false);
    }

    if(execsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql query (add column)");
        return(false);
    }

    sql = "";
    old_num_of_results++;

    sql << "UPDATE ROOT SET \"NResults\" = " << old_num_of_results << " WHERE \"Name\" = '" << Options.GetArgProjectName() << "'";

    if(execsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql query (update root)");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CAlterPrj::RemoveResultColumn(CFirebirdTransaction* p_trans,int old_num_of_results)
{
    CSmallString sql;

    sql << "ALTER TABLE \"" << Options.GetArgProjectName() << "\" DROP \"R" << old_num_of_results << "\"";

    CFirebirdExecuteSQL execsql;

    if(execsql.AssignToTransaction(p_trans) == false) {
        ES_ERROR("unable to initiate sql statement");
        return(false);
    }

    if(execsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql query (drop column)");
        return(false);
    }

    sql = "";
    old_num_of_results--;

    sql << "UPDATE ROOT SET \"NResults\" = " << old_num_of_results << " WHERE \"Name\" = '" << Options.GetArgProjectName() << "'";

    if(execsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql query (update root)");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CAlterPrj::AlterOperator(CFirebirdTransaction* p_trans)
{
    CSmallString sql;

    CFirebirdExecuteSQL execsql;

    if(execsql.AssignToTransaction(p_trans) == false) {
        ES_ERROR("unable to initiate sql statement");
        return(false);
    }

    sql << "UPDATE ROOT SET \"Operator\" = \'" << Options.GetOptValue() << "\' WHERE \"Name\" = '" << Options.GetArgProjectName() << "'";

    if(execsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql query (update root)");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CAlterPrj::AlterDescription(CFirebirdTransaction* p_trans)
{
    CSmallString sql;

    CFirebirdExecuteSQL execsql;

    if(execsql.AssignToTransaction(p_trans) == false) {
        ES_ERROR("unable to initiate sql statement");
        return(false);
    }

    sql << "UPDATE ROOT SET \"Description\" = \'" << Options.GetOptValue() << "\' WHERE \"Name\" = '" << Options.GetArgProjectName() << "'";

    if(execsql.ExecuteSQL(sql) == false) {
        ES_ERROR("unable to execute sql query (update root)");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CAlterPrj::Finalize(void)
{
    if(Options.GetOptVerbose()) {
        ErrorSystem.PrintErrors(stderr);
    }

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# vs-project-alter terminated at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << endl;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

