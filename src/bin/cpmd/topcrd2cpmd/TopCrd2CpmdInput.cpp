// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <errno.h>
#include <string.h>
#include <iomanip>
#include <ErrorSystem.hpp>
#include <PeriodicTable.hpp>
#include <FileName.hpp>

#include <SmallTimeAndDate.hpp>
#include <AmberMaskAtoms.hpp>

#include <boost/format.hpp>

#include "TopCrd2Cpmd.hpp"

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2Cpmd::WriteCPMDFiles(void)
{
    vout << endl;
    vout << "# ==============================================================================" <<  endl;
    vout << "# X) Writing CPMD input files ..." << endl;
    vout << "# ==============================================================================" <<  endl;
    vout << endl;

    if( WriteCPMDInputFile() == false ) return(false);

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Cpmd::WriteCPMDInputFile(void)
{
    CFileName full_name;
    full_name = Options.GetArgPrefixName() + "_first.inp";

    vout << "# Writing " << full_name << " file ..." << endl;
    ofstream ofs;

    ofs.open(full_name);
    if( ! ofs ){
        CSmallString error;
        error << "unable to open file for writing " << full_name;
        ES_ERROR(error);
        return(false);
    }

    if( WriteCPMDTitle(ofs) == false ) return(false);
    if( WriteCPMDSystem(ofs) == false ) return(false);
    if( WriteCPMDDFT(ofs) == false ) return(false);
    if( WriteCPMDAtoms(ofs) == false ) return(false);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2Cpmd::WriteCPMDTitle(std::ostream& sout)
{
    sout << "&TITLE" << endl;

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    sout << "   Prepared by topcrd2cpmd at " << dt.GetSDateAndTime() << endl;
    sout << "   Input topology    : " << Options.GetArgTopologyName() << endl;
    sout << "   Input coordinates : " << Options.GetArgCrdName() << endl;
    sout << "   QM zone           : " << Options.GetArgMaskSpec() << endl;
    sout << "   Purpose           : test input" << endl;
    sout << "&END" << endl;
    sout << endl;

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Cpmd::WriteCPMDSystem(std::ostream& sout)
{
    sout << "&SYSTEM" << endl;
    sout << "&END" << endl;
    sout << endl;

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Cpmd::WriteCPMDDFT(std::ostream& sout)
{
    sout << "&DFT" << endl;
    sout << "   FUNCTIONAL " << Options.GetOptFunctional() << endl;
    sout << "&END" << endl;
    sout << endl;

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Cpmd::WriteCPMDAtoms(std::ostream& sout)
{
    sout << "&ATOMS" << endl;

    // first link atoms
    boost::format intro_format;
    try {
        intro_format.parse(string(Options.GetOptPSPFormat()));
    } catch(...) {
        ES_ERROR("unable to parse format of pseudopotential entries");
        return(false);
    }

    intro_format % "H" % Options.GetOptFunctional();
    sout << "*" << intro_format.str() << " DUMMY" << endl;
    sout << "   LMAX=S" << endl;
    sout << "   " << setw(5) << QMLinks.size() << endl;

    sout << "  ";
    for(size_t i=0 ; i < QMLinks.size(); i++) {
        sout << " " << setw(5) << QMLinks[i].ID;
    }
    sout << endl;

    // make list of unique z
    set<int>    unique_z;
    for(int i=0 ; i < Mask.GetNumberOfTopologyAtoms(); i++) {
        CAmberAtom* p_atm = Mask.GetSelectedAtom(i);
        if( p_atm == NULL ) continue;
        int z = p_atm->GetAtomicNumber();
        if( z == 0 ) z = p_atm->GuessZ();
        unique_z.insert(z);
    }

    // for each unique z print data
    set<int>::iterator it = unique_z.begin();
    set<int>::iterator ie = unique_z.end();

    while( it != ie ){
        sout << endl;
        intro_format.clear();
        intro_format % PeriodicTable.GetSymbol(*it) % Options.GetOptFunctional();
        sout << "*" << intro_format.str() << endl;
        if( *it <= 2 ) {
            sout << "   LMAX=S" << endl;
        } else if( *it <= 10 ) {
            sout << "   LMAX=S" << endl;
        } else {
            sout << "   LMAX=D" << endl;
        }
        int count = 0;
        for(int i=0 ; i < Mask.GetNumberOfTopologyAtoms(); i++) {
            CAmberAtom* p_atm = Mask.GetSelectedAtom(i);
            if( p_atm == NULL ) continue;
            int z = p_atm->GetAtomicNumber();
            if( z == 0 ) z = p_atm->GuessZ();
            if( z != *it ) continue;
            count++;
        }
        sout << "   " << setw(5) << count << endl;
        sout << "  ";
        for(int i=0 ; i < Mask.GetNumberOfTopologyAtoms(); i++) {
            CAmberAtom* p_atm = Mask.GetSelectedAtom(i);
            if( p_atm == NULL ) continue;
            int z = p_atm->GetAtomicNumber();
            if( z == 0 ) z = p_atm->GuessZ();
            if( z != *it ) continue;
            sout << " " << setw(5) << i + 1;
        }
        sout << endl;
        it++;
    }

    sout << "&END" << endl;
    sout << endl;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

