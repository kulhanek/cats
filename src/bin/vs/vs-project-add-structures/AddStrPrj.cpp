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
#include <InfMol.hpp>
#include <iomanip>

#include "AddStrPrj.hpp"
#include "AddStrPrjOptions.hpp"

#include <FBDatabaseExt.hpp>
#include <FirebirdTransaction.hpp>
#include <FirebirdExecuteSQL.hpp>
#include <FirebirdItem.hpp>

#include <openbabel/obconversion.h>
#include <openbabel/oberror.h>

using namespace std;
using namespace OpenBabel;

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
    MsgOut.Attach(cout);
    if(Options.GetOptVerbose()) MsgOut.Verbosity(CVerboseStr::high);

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# vs-project-add-structures started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# Database key file      : " << Options.GetArgDatabaseKey() << endl;
    MsgOut << "# Name of project        : " << Options.GetArgProjectName() << endl;
    MsgOut << "# ------------------------------------------------------------------------------" << endl;
    MsgOut << "# Use hiearchy           : " << bool_to_str(Options.GetOptUseHiearchy()) << endl;
    MsgOut << "# Init static properties : " << bool_to_str(Options.GetOptInitStaticProperties()) << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CAddStrPrj::Run(void)
{
    MsgOut << endl;
    MsgOut << ":::::::::::::::::::::::::::::::: Adding structures :::::::::::::::::::::::::::::" << endl;


    CFBDatabaseExt db;

    if(db.OpenByKey(Options.GetArgDatabaseKey()) == false) {
        ES_ERROR("unable to open database");
        return(false);
    }

    cout << "Project name                 : " << Options.GetArgProjectName() << endl;

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
        ES_ERROR("unable to initiate transaction");
        return(false);
    }

    bool result;
    if(Options.GetOptUseHiearchy() == true) {
        result = AddHiearchy(&trans,".",0);
    } else {
        result = AddStructures(&trans,".");
    }

    cout << "Number of added molecules    : " << NumOfMols << endl;
    cout << "Number of duplicit molecules : " << NumOfDuplicities << endl;
    cout << "Number of errors             : " << NumOfErrors << endl;

    if(result == true) {
        if(trans.CommitTransaction() == false) {
            ES_ERROR("unable to commit transaction");
            return(false);
        }
    } else {
        if(trans.RollbackTransaction() == false) {
            ES_ERROR("unable to rollback transaction");
            trans.RollbackTransaction();
            return(false);
        }
    }

    db.Logout();

    return(result);
}

//------------------------------------------------------------------------------

bool CAddStrPrj::AddHiearchy(CFirebirdTransaction* p_tran,const CFileName& dir,int level)
{
    if(level == 3) {
        return(AddStructures(p_tran,dir));
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
            if(AddHiearchy(p_tran,newdir,level+1) == false) {
                return(false);
            }

        }
    }

    denum.EndFindFile();
    return(true);
}

//------------------------------------------------------------------------------

bool CAddStrPrj::AddStructures(CFirebirdTransaction* p_tran,const CFileName& dir)
{
    CDirectoryEnum  denum(dir);

    CSmallString filter;
    filter = "*." + Options.GetOptInputFormat();

    if(denum.StartFindFile(filter) == false) {
        ES_ERROR("unable to StartFindFile for structures");
        return(false);
    }

    CFirebirdExecuteSQL exsql;

    if(exsql.AssignToTransaction(p_tran) == false) {
        ES_ERROR("unable to initiate sql statement");
        return(false);
    }

    CSmallString sql;

    obErrorLog.SetOutputStream(&MsgOut);

    if(Options.GetOptInitStaticProperties()) {
        sql << "INSERT INTO \"" << Options.GetArgProjectName() << "\" (\"ID\",\"Flag\",\"InChiKey\",\"NA\",\"TC\",\"MW\") ";
        sql << "VALUES (?,?,?,?,?,?)";

        if(exsql.AllocateInputItems(6) == false) {
            ES_ERROR("unable to allocate items");
            return(false);
        }
    } else {
        sql << "INSERT INTO \"" << Options.GetArgProjectName() << "\" (\"ID\",\"Flag\") ";
        sql << "VALUES (?,?)";

        if(exsql.AllocateInputItems(2) == false) {
            ES_ERROR("unable to allocate items");
            return(false);
        }
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
            continue;
        }

        if(Options.GetOptInitStaticProperties()) {
            CInfMol mol;

            if(mol.ReadMol(dir/file,"auto") == false) {
                ES_ERROR("unable to load structure");
                NumOfErrors++;
                cout << " error-> " << name << endl;
                continue;
            }

            exsql.GetInputItem(0)->SetString(name.GetSubString(4,8));
            exsql.GetInputItem(1)->SetInt(0);

            OBConversion   conv;

            OpenBabel::OBFormat* outFormat;
            outFormat = conv.FindFormat("inchi");

            if(outFormat == NULL) {
                ES_ERROR("unable to find inchi output format");
                return(false);
            }

            if(! conv.SetOutFormat(outFormat)) {
                ES_ERROR("unable to select inchi output format");
                return(false);
            }

            conv.SetOptions("K",OBConversion::OUTOPTIONS);
            string inchi_key;
            inchi_key = conv.WriteString(&mol);

            exsql.GetInputItem(2)->SetString(inchi_key.c_str());
            exsql.GetInputItem(3)->SetInt(mol.NumAtoms());

            OBMolAtomIter  ita;
            double         pcharge = 0.0;

            for(ita = mol; ita == true ; ita++) {
                OBAtom* p_atom = &(*ita);
                pcharge += p_atom->GetPartialCharge();
            }
            // mol.GetTotalCharge() - does not work as expected
            exsql.GetInputItem(4)->SetInt(pcharge);

            CSmallString mw;
            mw.DoubleToStr(mol.GetMolWt(),"%.3f");
            exsql.GetInputItem(5)->SetString(mw);
        } else {
            exsql.GetInputItem(0)->SetString(name.GetSubString(4,8));
            exsql.GetInputItem(1)->SetInt(0);
        }

        if(exsql.ExecuteSQL(sql) == false) {
            CSmallString error;
            error << "unable to execute sql statement for : " << name.GetSubString(4,8);
            ES_ERROR(error);
            NumOfDuplicities++;
            continue;
        }

        // commit transaction
        p_tran->CommitTransaction(true);
    }

    denum.EndFindFile();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CAddStrPrj::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# vs-project-add-structures terminated at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;

    if(ErrorSystem.IsError() ||  Options.GetOptVerbose() || (NumOfErrors > 0)) {
        ErrorSystem.PrintErrors(stderr);
    }

    MsgOut << endl;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

