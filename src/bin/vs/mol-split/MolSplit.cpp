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
#include <FileName.hpp>
#include <sstream>

#include "MolSplit.hpp"
#include "MolSplitOptions.hpp"

// openbabel
#include "openbabel/mol.h"
#include "openbabel/obconversion.h"
#include "openbabel/generic.h"

using namespace std;
using namespace OpenBabel;

//------------------------------------------------------------------------------

CMolSplit MolSplit;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMolSplit::CMolSplit(void)
{
    NumOfMols = 0;
    NumOfDuplicities = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CMolSplit::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CMolSplitOptions
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
    MsgOut << "# mol-split started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# Input molecule name  : " << Options.GetArgInputFile() << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CMolSplit::Run(void)
{
    MsgOut << endl;
    MsgOut << ":::::::::::::::::::::::::::::::: Processing Data :::::::::::::::::::::::::::::::" << endl;

    bool result;
    if(Options.GetOptLineEntries() == true) {
        result = SplitLineStream();
    } else {
        result = SplitOpenBabelStream();
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CMolSplit::SplitLineStream(void)
{
    ifstream ifs;

    CSmallString name;
    CSmallString format;

    name = Options.GetArgInputFile();
    format = Options.GetOptInFormat();

    bool result;

    if(name != "-") {
        ifs.open(name);

        if(! ifs) {
            CSmallString error;
            error << "unable to open molecule: file " << name << " (format: " << format << ")";
            ES_ERROR(error);
            ifs.close();
            return(false);
        }

        result = ProcessLineStream(ifs);
    } else {
        result = ProcessLineStream(cin);
    }

    cout << "Number of converted molecules : " << NumOfMols << endl;
    cout << "Number of duplicit molecules  : " << NumOfDuplicities << endl;

    return(result);
}

//------------------------------------------------------------------------------

bool CMolSplit::ProcessLineStream(istream& is)
{
    while(is) {
        string sname;
        string smile;

        is >> sname >> smile;

        if(sname.empty()) break;
        if(smile.empty()) break;

        NumOfMols++;

        if(Options.GetOptIDFormat() == "UNIS") {
            if(WriteUNISLine(sname.c_str(),smile.c_str()) == false) return(false);
        } else if(Options.GetOptIDFormat() == "CAS") {
            if(WriteCASLine(sname.c_str(),smile.c_str()) == false) return(false);
        } else {
            CSmallString error;
            error << "unsupported ID format : " << Options.GetOptIDFormat();
            ES_ERROR(error);
            return(false);
        }



    }

    return(true);
}

//------------------------------------------------------------------------------

bool CMolSplit::WriteUNISLine(const CSmallString& name,const CSmallString& line)
{
    if(name.GetLength() != 12) {
        CSmallString error;
        error << name << " does not have 12 characters";
        ES_ERROR(error);
        return(false);
    }

    if(UnisID == NULL) {
        UnisID = name.GetSubString(0,4);
    }

    if(name.GetSubString(0,4) != UnisID) {
        CSmallString error;
        error << name << " does not have UNIS ID: " << UnisID << " (UNIS ID of the first molecule)";
        ES_ERROR(error);
        return(false);
    }

    CSmallString lname = name;

    if(Options.GetOptCreateHiearchy()) {
        CFileName dir;
        dir = CFileName(name.GetSubString(4,2)) / CFileName(name.GetSubString(6,2))
              / CFileName(name.GetSubString(8,2));
        if(CFileSystem::CreateDir(dir) == false) {
            CSmallString error;
            error << "unable to create directory (" << dir << ")";
            ES_ERROR(error);
            return(false);
        }

        lname = dir / name;
    }

    CSmallString format = Options.GetOptInFormat();
    lname = lname + "." + format;

    if(CFileSystem::IsFile(lname)) {
        if(Options.GetOptPrintDuplicit()) {
            cout << "Duplicit ID: " << lname << endl;
        }
        NumOfDuplicities++;
        return(true);
    }

    ofstream ofs(lname);

    if(! ofs) {
        CSmallString error;
        error << "unable to open molecule: file " << lname << " (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    ofs << line;

    ofs.close();

    return(true);
}

//------------------------------------------------------------------------------

bool CMolSplit::WriteCASLine(const CSmallString& name,const CSmallString& line)
{
    int a;
    int b;
    int c;

    if(sscanf(name,"%d-%d-%d",&a,&b,&c) != 3) {
        CSmallString error;
        error << "three numbers expected (" << name << ")";
        ES_ERROR(error);
        return(false);
    }

    char cas_name_str[13];

    sprintf(cas_name_str,"%07d-%02d-%1d",a,b,c);
    cas_name_str[12] = '\0';

    CSmallString    cas_name(cas_name_str);
    CSmallString    lname = name;

    CSmallString format = Options.GetOptInFormat();

    if(Options.GetOptCreateHiearchy()) {
        CFileName dir;
        dir = CFileName(cas_name.GetSubString(0,1)) / CFileName(cas_name.GetSubString(1,2))
              / CFileName(cas_name.GetSubString(3,2)) / CFileName(cas_name.GetSubString(5,2));
        if(CFileSystem::CreateDir(dir) == false) {
            CSmallString error;
            error << "unable to create directory (" << dir << ")";
            ES_ERROR(error);
            return(false);
        }

        lname = dir / cas_name + "." + format;
    } else {
        lname = cas_name + "." + format;
    }

    if(CFileSystem::IsFile(lname)) {
        if(Options.GetOptPrintDuplicit()) {
            cout << "Duplicit ID: " << lname << endl;
        }
        NumOfDuplicities++;
        return(true);
    }

    ofstream ofs(lname);

    if(! ofs) {
        CSmallString error;
        error << "unable to open molecule: file " << lname << " (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    ofs << line;

    ofs.close();

    return(true);
}

//------------------------------------------------------------------------------

bool CMolSplit::SplitOpenBabelStream(void)
{
    CSmallString format,name;

    name = Options.GetArgInputFile();
    format = Options.GetOptInFormat();

    ifstream ifs;

    if(name != "-") {
        ifs.open(name);

        if(! ifs) {
            CSmallString error;
            error << "unable to open molecule: file " << name << " (format: " << format << ")";
            ES_ERROR(error);
            ifs.close();
            return(false);
        }
    }

    // read molecule from file to babel internal
    OBConversion   conv(&ifs, NULL);

    if(name == "-") {
        conv.SetInStream(&cin);
    }

    OpenBabel::OBFormat* obFormat;

    if(format == "auto") {
        obFormat = conv.FormatFromExt(name);
    } else {
        obFormat = conv.FindFormat(format);
    }

    if(obFormat == NULL) {
        CSmallString error;
        error << "unable to select molecule format: file " << name << " (format: " << format << ")";
        ES_ERROR(error);
        ifs.close();
        return(false);
    }

    if(! conv.SetInFormat(obFormat)) {
        CSmallString error;
        error << "unable to select molecule format: file " << name << " (format: " << format << ")";
        ES_ERROR(error);
        ifs.close();
        return(false);
    }

    OBMol mol;

    // read molecules in a loop
    while(conv.Read(&mol)) {
        if(Options.GetOptIDFormat() == "UNIS") {
            if(WriteUNISMol(mol) == false) return(false);
        } else if(Options.GetOptIDFormat() == "CAS") {
            if(WriteCASMol(mol) == false) return(false);
        } else {
            CSmallString error;
            error << "unsupported ID format : " << Options.GetOptIDFormat();
            ES_ERROR(error);
            return(false);
        }
        NumOfMols++;
    }

    cout << "Number of converted molecules : " << NumOfMols << endl;
    cout << "Number of duplicit molecules  : " << NumOfDuplicities << endl;

    ifs.close();

    return(true);
}

//------------------------------------------------------------------------------

bool CMolSplit::WriteUNISMol(OBMol& mol)
{
    CFileName format,name;

    name = mol.GetTitle();

    if(name.GetLength() != 12) {
        CSmallString error;
        error << mol.GetTitle() << " does not have 12 characters";
        ES_ERROR(error);
        return(false);
    }

    if(UnisID == NULL) {
        UnisID = name.GetSubString(0,4);
    }

    if(name.GetSubString(0,4) != UnisID) {
        CSmallString error;
        error << mol.GetTitle() << " does not have UNIS ID: " << UnisID << "(UNIS ID of the first molecule)";
        ES_ERROR(error);
        return(false);
    }

    if(Options.GetOptCreateHiearchy()) {
        CFileName dir;
        dir = CFileName(name.GetSubString(4,2)) / CFileName(name.GetSubString(6,2))
              / CFileName(name.GetSubString(8,2));
        if(CFileSystem::CreateDir(dir) == false) {
            CSmallString error;
            error << "unable to create directory (" << dir << ")";
            ES_ERROR(error);
            return(false);
        }

        name = dir / name;
    }

    format = "mol2";
    name = name + "." + format;


    if(CFileSystem::IsFile(name)) {
        if(Options.GetOptPrintDuplicit()) {
            cout << "Duplicit ID: " << mol.GetTitle() << endl;
        }
        NumOfDuplicities++;
        return(true);
    }

    ofstream ofs(name);

    if(! ofs) {
        CSmallString error;
        error << "unable to open molecule: file " << name << " (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    // write molecule to file
    OBConversion   conv(NULL, &ofs);
    OpenBabel::OBFormat* obFormat = conv.FormatFromExt(name);

    if(! conv.SetOutFormat(obFormat)) {
        CSmallString error;
        error << "unable to select molecule format: file " << name << " (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    if(! conv.Write(&mol)) {
        CSmallString error;
        error << "unable to write molecule: file " << name << " (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    ofs.close();

    return(true);
}

//------------------------------------------------------------------------------

bool CMolSplit::WriteCASMol(OBMol& mol)
{
    CFileName format,tname;

    tname = mol.GetTitle();

    int a;
    int b;
    int c;

    if(sscanf(tname,"%d-%d-%d",&a,&b,&c) != 3) {
        CSmallString error;
        error << "three numbers expected (" << tname << ")";
        ES_ERROR(error);
        return(false);
    }

    char cas_name_str[13];

    sprintf(cas_name_str,"%07d-%02d-%1d",a,b,c);
    cas_name_str[12] = '\0';

    CSmallString    cas_name(cas_name_str);
    CSmallString    name;

    format = "mol2";

    if(Options.GetOptCreateHiearchy()) {
        CFileName dir;
        dir = CFileName(cas_name.GetSubString(0,1)) / CFileName(cas_name.GetSubString(1,2))
              / CFileName(cas_name.GetSubString(3,2)) / CFileName(cas_name.GetSubString(5,2));
        if(CFileSystem::CreateDir(dir) == false) {
            CSmallString error;
            error << "unable to create directory (" << dir << ")";
            ES_ERROR(error);
            return(false);
        }

        name = dir / cas_name + "." + format;
    } else {
        name = cas_name + "." + format;
    }

    if(CFileSystem::IsFile(name)) {
        if(Options.GetOptPrintDuplicit()) {
            cout << "Duplicit ID: " << mol.GetTitle() << endl;
        }
        NumOfDuplicities++;
        return(true);
    }

    ofstream ofs(name);

    if(! ofs) {
        CSmallString error;
        error << "unable to open molecule: file " << name << " (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    // write molecule to file
    OBConversion   conv(NULL, &ofs);
    OpenBabel::OBFormat* obFormat = conv.FormatFromExt(name);

    if(! conv.SetOutFormat(obFormat)) {
        CSmallString error;
        error << "unable to select molecule format: file " << name << " (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    if(! conv.Write(&mol)) {
        CSmallString error;
        error << "unable to write molecule: file " << name << " (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    ofs.close();

    return(true);
}

//------------------------------------------------------------------------------

bool CMolSplit::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# mol-split terminated at " << dt.GetSDateAndTime() << endl;
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

