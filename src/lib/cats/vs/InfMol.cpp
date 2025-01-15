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
#include <InfMol.hpp>
#include <openbabel/obconversion.h>
#include <ErrorSystem.hpp>
#include <fstream>

using namespace std;
using namespace OpenBabel;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CInfMol::CInfMol(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CInfMol::ReadMol(const CSmallString& name,const CSmallString& format,int index)
{
    ifstream ifs;

    if(name != "-") {
        ifs.open(name);

        if(! ifs) {
            CSmallString error;
            error << "unable to open molecule: file " << name << "(format: " << format << ")";
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

    if(! conv.SetInFormat(obFormat)) {
        CSmallString error;
        error << "unable to select molecule format: file " << name << "(format: " << format << ")";
        ES_ERROR(error);
        ifs.close();
        return(false);
    }

    for(int i=1; i < index; i++){
        OBMol tmpmol;
        if( conv.Read(&tmpmol) == false ){
            CSmallString error;
            error << "unable to read molecule: file " << name << "(format: " << format << ") / index: " << i ;
            ES_ERROR(error);
            ifs.close();
            return(false);
        }
    }

    if(! conv.Read(this)) {
        CSmallString error;
        error << "unable to read molecule: file " << name << "(format: " << format << ")";
        ES_ERROR(error);
        ifs.close();
        return(false);
    }

    ifs.close();

    ConnectTheDots();

    return(true);
}

//------------------------------------------------------------------------------

bool CInfMol::WriteMol(const CSmallString& name,const CSmallString& format,const CSmallString opts)
{
    ofstream ofs(name);

    if(! ofs) {
        CSmallString error;
        error << "unable to open molecule file: '" << name << "' (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

// write molecule to file
    OBConversion   conv(NULL, &ofs);

    OpenBabel::OBFormat* obFormat;
    if(format == "auto") {
        obFormat = conv.FormatFromExt(name);
    } else {
        obFormat = conv.FindFormat(format);
    }

    if(! conv.SetOutFormat(obFormat)) {
        CSmallString error;
        error << "unable to select molecule format: file '" << name << "' (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    if( opts != NULL ){
        for(size_t i=0; i < opts.GetLength(); i++){
            CSmallString opt(opts.GetBuffer()[i]);
            conv.AddOption(opt,OBConversion::OUTOPTIONS);
        }
    }

    if(! conv.Write(this)) {
        CSmallString error;
        error << "unable to write molecule: file '" << name << "' (format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    ofs.close();

    return(true);
}

//------------------------------------------------------------------------------

void CInfMol::AlterHydrogens(const CSmallString& mode)
{
    if( mode == "keep" ) return;

    if( mode == "add" ){
        AddHydrogens();
        return;
    }
    if( mode == "addpolar" ){
        AddPolarHydrogens();
        return;
    }
    if( mode == "addnonpolar" ){
        AddNonPolarHydrogens();
        return;
    }

    if( (mode == "delete") || (mode == "remove") ){
        DeleteHydrogens();
        return;
    }
    if( (mode == "deletepolar") || (mode == "removepolar") ){
        DeletePolarHydrogens();
        return;
    }
    if( (mode == "deletenonpolar") || (mode == "removenonpolar") ){
        DeleteNonPolarHydrogens();
        return;
    }

    CSmallString error;
    error << "Unsupported alter hydrogen mode: " << mode;
    RUNTIME_ERROR(error);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

