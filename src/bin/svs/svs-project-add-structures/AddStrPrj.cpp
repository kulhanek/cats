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
#include <DirectoryEnum.hpp>
#include <FileSystem.hpp>
#include <sstream>
#include <iomanip>

#include "AddStrPrj.hpp"
#include "AddStrPrjOptions.hpp"

#include <sqlite3.h>

//------------------------------------------------------------------------------

using namespace std;

//------------------------------------------------------------------------------

CAddStrPrj AddStrPrj;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CAddStrPrj::CAddStrPrj(void)
{
    NumOfMols = 0;
    NumOfDuplicities = 0;
    NumOfErrors = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CAddStrPrj::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CAddStrPrjOptions
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
    MsgOut << "# svs-project-add-structures started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# Project database   : " << Options.GetArgProjectName() << endl;
    MsgOut << "# Structure database : " << Options.GetArgProjectName() << endl;
    MsgOut << "# ------------------------------------------------------------------------------" << endl;
    MsgOut << "# Use hiearchy       : " << bool_to_str(Options.GetOptUseHiearchy()) << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CAddStrPrj::Run(void)
{
    MsgOut << endl;
    MsgOut << ":::::::::::::::::::::::::::::::: Adding structures :::::::::::::::::::::::::::::" << endl;

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

    MsgOut << low;

    bool result;
    if(Options.GetOptUseHiearchy() == true) {
        result = AddHiearchy(sqldb,Options.GetArgStructurePath(),0);
    } else {
        result = AddStructures(sqldb,Options.GetArgStructurePath());
    }

    MsgOut << low;
    MsgOut << "Number of added molecules    : " << NumOfMols << endl;
    MsgOut << "Number of duplicit molecules : " << NumOfDuplicities << endl;
    MsgOut << "Number of errors             : " << NumOfErrors << endl;

    // close database
    sqlite3_close(sqldb);

    MsgOut << high;
    if( result ) {
        MsgOut << "Done." << endl;
    }
    return(result);
}

//------------------------------------------------------------------------------

bool CAddStrPrj::AddHiearchy(sqlite3* sqldb,const CFileName& dir,int level)
{
    if(level == 3) {
        return(AddStructures(sqldb,dir));
    }

    CDirectoryEnum  denum(dir);

    if(denum.StartFindFile("*") == false) {
        ES_ERROR("unable to StartFindFile for directories");
        return(false);
    }

    CFileName file;
    while(denum.FindFile(file)) {
        if(file == ".") continue;
        if(file == "..") continue;
        if(file.GetLength() != 2) continue;
        CFileName newdir = dir / file;
        if(CFileSystem::IsDirectory(newdir)) {
            if(AddHiearchy(sqldb,newdir,level+1) == false) {
                return(false);
            }

        }
    }

    denum.EndFindFile();
    return(true);
}

//------------------------------------------------------------------------------

bool CAddStrPrj::AddStructures(sqlite3* sqldb,const CFileName& dir)
{
    CDirectoryEnum  denum(dir);

    CSmallString filter;
    filter = "*." + Options.GetOptInputFormat();

    if(denum.StartFindFile(filter) == false) {
        ES_ERROR("unable to StartFindFile for structures");
        return(false);
    }

    CSmallString sql;

    sql << "INSERT INTO PROJECT (ID,FLAG) VALUES (?,?)";

    sqlite3_stmt* sqlstm;

    int rcode;
    rcode = sqlite3_prepare_v2(sqldb,sql,-1,&sqlstm,NULL);
    if( rcode != SQLITE_OK ) {
        ES_ERROR(sql);
        ES_ERROR(sqlite3_errmsg(sqldb));
        MsgOut << endl;
        MsgOut << "<red><b>>>> ERROR: Unable to prepare the SQL statement!</b></red>" << endl;
        return(false);
    }

    CFileName file;
    while(denum.FindFile(file)) {

        NumOfMols++;

        CSmallString name;
        name = file.GetSubString(0,12);

        if(Options.GetOptProgress()) {
            cout << setw(8) << NumOfMols << " " << name << endl;
        }

        if(name.GetLength() != 12) {
            CSmallString error;
            error << "Molecule ID (" << name << ") does not have 12 characters (UNIS id + 8 numbers)";
            ES_ERROR(error);
            NumOfErrors++;
            // reset bindings
            sqlite3_reset(sqlstm);
            continue;
        }

        if(UnisID == NULL) {
            UnisID = name.GetSubString(0,4);
        }

        if(name.GetSubString(0,4) != UnisID) {
            CSmallString error;
            error << name << " does not have UNIS ID: " << UnisID << " (UNIS ID of the first molecule)";
            ES_ERROR(error);
            NumOfErrors++;
            // reset bindings
            sqlite3_reset(sqlstm);
            continue;
        }

        sqlite3_bind_text(sqlstm,1,name.GetSubString(4,8),-1,SQLITE_TRANSIENT);
        sqlite3_bind_int(sqlstm,2,0);

        if( sqlite3_step(sqlstm) != SQLITE_DONE ) {
            CSmallString error;
            error << "unable to execute sql statement for : " << name.GetSubString(4,8);
            ES_WARNING(error);
            ES_WARNING(sqlite3_errmsg(sqldb));
            NumOfDuplicities++;
            // reset bindings
            sqlite3_reset(sqlstm);
            continue;
        }

        // reset bindings
        sqlite3_reset(sqlstm);
    }

    denum.EndFindFile();

    // release the statement
    sqlite3_finalize(sqlstm);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CAddStrPrj::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << high;
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# svs-project-add-structures terminated at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;

    if(ErrorSystem.IsError() ||  Options.GetOptVerbose() || (NumOfErrors > 0)) {
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

