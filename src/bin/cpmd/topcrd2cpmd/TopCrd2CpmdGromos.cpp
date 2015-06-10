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

bool CTopCrd2Cpmd::WriteGromosFiles(void)
{
    vout << endl;
    vout << "# ==============================================================================" <<  endl;
    vout << "# X) Writing GROMOS input files ..." << endl;
    vout << "# ==============================================================================" <<  endl;
    vout << endl;

    if( WriteGromosCoordinates() == false ) return(false);

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Cpmd::WriteGromosCoordinates(void)
{
    CFileName full_name;
    full_name = Options.GetArgPrefixName() + ".g96";

    vout << "# Writing " << full_name << " file ..." << endl;
    ofstream ofs;

    ofs.open(full_name);
    if( ! ofs ){
        CSmallString error;
        error << "unable to open file for writing " << full_name;
        ES_ERROR(error);
        return(false);
    }

    // write title
    ofs << "TITLE" << endl;
    ofs << "coordinates generated from amber coord file:" << endl;
    ofs << Options.GetArgCrdName() << endl;
    ofs << "END" << endl;

    // possitions - solute
    ofs << "POSITION" << endl;
    ofs << "# first 24 chars ignored" << endl;

    int resid = 0;
    int atomid = 0;
    CAmberResidue* p_prev_res = NULL;

    for(int i=0; i < Topology.BoxInfo.GetNumberOfSoluteAtoms(); i++ ){
        CAmberAtom* p_atom = Topology.AtomList.GetAtom(i);
        CAmberResidue* p_res = p_atom->GetResidue();
        if( p_res != p_prev_res ){
            p_prev_res = p_res;
            resid++;
        }
        atomid++;

        //    1 MET   N          1   10.484003067    2.845767260    4.472509861
        //----- ----- ----- ------ -------------- -------------- --------------

        ofs << setw(5) << resid % 100000 << " ";
        ofs << setw(5) << p_res->GetName() << " ";
        ofs << setw(5) << p_atom->GetName() << " ";
        ofs << setw(6) << atomid % 1000000 << " ";
        ofs << fixed;
        CPoint pos = Coordinates.GetPosition(i);
        // convert to nanometers
        pos /= 10.0;
        ofs << setw(14) << setprecision(9) << pos.x << " ";
        ofs << setw(14) << setprecision(9) << pos.y << " ";
        ofs << setw(14) << setprecision(9) << pos.z << endl;
    }

    // positions - capping
    for(size_t i=0; i < QMLinks.size(); i++){
        resid++;
        atomid++;
        ofs << setw(5) << resid % 100000 << " ";
        ofs << setw(5) << "DUM" << " ";
        ofs << setw(5) << "DUM" << " ";
        ofs << setw(6) << atomid % 1000000 << " ";
        ofs << fixed;
        CPoint pos = QMLinks[i].HLinkPos;
        // convert to nanometers
        pos /= 10.0;
        ofs << setw(14) << setprecision(9) << pos.x << " ";
        ofs << setw(14) << setprecision(9) << pos.y << " ";
        ofs << setw(14) << setprecision(9) << pos.z << endl;
    }

    // positions - solvent
    resid = 0;
    p_prev_res = NULL;
    for(int i=Topology.BoxInfo.GetNumberOfSoluteAtoms(); i < Topology.AtomList.GetNumberOfAtoms(); i++ ){
        CAmberAtom* p_atom = Topology.AtomList.GetAtom(i);
        CAmberResidue* p_res = p_atom->GetResidue();
        if( p_res != p_prev_res ){
            p_prev_res = p_res;
            resid++;
        }
        atomid++;

        ofs << setw(5) << resid % 100000 << " ";
        ofs << setw(5) << "SOLV" << " ";
        ofs << setw(5) << p_atom->GetName() << " ";
        ofs << setw(6) << atomid % 1000000 << " ";
        ofs << fixed;
        CPoint pos = Coordinates.GetPosition(i);
        // convert to nanometers
        pos /= 10.0;
        ofs << setw(14) << setprecision(9) << pos.x << " ";
        ofs << setw(14) << setprecision(9) << pos.y << " ";
        ofs << setw(14) << setprecision(9) << pos.z << endl;
    }
    ofs << "END" << endl;

    // box info
    ofs << "BOX" << endl;
    CPoint box = Coordinates.GetBox();
    box /= 10.0; // convert to nm
    //    8.571606550    7.942534520    8.399419790
    // -------------- -------------- --------------
    ofs << " ";
    ofs << setw(14) << setprecision(9) << box.x << " ";
    ofs << setw(14) << setprecision(9) << box.y << " ";
    ofs << setw(14) << setprecision(9) << box.z << endl;
    ofs << "END" << endl;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

