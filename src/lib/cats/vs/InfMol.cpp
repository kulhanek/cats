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

bool CInfMol::ReadMol(const CSmallString& name,const CSmallString& format)
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

bool CInfMol::WriteMol(const CSmallString& name,const CSmallString& format)
{
    ofstream ofs(name);

    if(! ofs) {
        CSmallString error;
        error << "unable to open molecule: file " << name << "(format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

// write molecule to file
    OBConversion   conv(NULL, &ofs);
    OpenBabel::OBFormat* obFormat = conv.FormatFromExt(name);

    if(! conv.SetOutFormat(obFormat)) {
        CSmallString error;
        error << "unable to select molecule format: file " << name << "(format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    if(! conv.Write(this)) {
        CSmallString error;
        error << "unable to write molecule: file " << name << "(format: " << format << ")";
        ES_ERROR(error);
        ofs.close();
        return(false);
    }

    ofs.close();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

