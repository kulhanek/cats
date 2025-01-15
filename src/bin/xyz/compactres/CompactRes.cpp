// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2025 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Transformation.hpp>
#include <iomanip>

#include "CompactRes.hpp"
#include "CompactResOptions.hpp"

// openbabel
#include "openbabel/mol.h"
#include "openbabel/atom.h"
#include "openbabel/residue.h"
#include "openbabel/obconversion.h"
#include "openbabel/generic.h"
#include "openbabel/obiter.h"


using namespace std;
using namespace OpenBabel;

//------------------------------------------------------------------------------

CCompactRes CompactRes;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCompactRes::CCompactRes(void)
{
}

//------------------------------------------------------------------------------

CCompactRes::~CCompactRes(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CCompactRes::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CCompactResOptions
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if(result != SO_CONTINUE) return(result);

    // set output stream
    MsgOut.Attach(cout);
    MsgOut.Verbosity(CVerboseStr::low);
    if( Options.GetOptVerbose() ) MsgOut.Verbosity(CVerboseStr::high);

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();
    MsgOut << high;
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# compactres started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# Input molecule name     : " << Options.GetArgInStrName() << " / " << Options.GetOptInIndex() << endl;
    MsgOut << "# Output molecule name    : " << Options.GetArgOutStrName() << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCompactRes::Run(void)
{
    MsgOut << endl;
    MsgOut << "::::::::::::::::::::::::::::::: Processing Data ::::::::::::::::::::::::::::::" << endl;

    MsgOut << endl;
    MsgOut << "2) Reading molecule #1 (in) ..." << endl;
    if(Str.ReadMol(Options.GetArgInStrName(),Options.GetOptInFormat(),Options.GetOptInIndex()) == false) return(false);

    Str.SetChainsPerceived(true);                   // keep atom names and residues

    MsgOut << "   Number of atoms    = " << Str.NumAtoms() << endl;
    MsgOut << "   Number of bonds    = " << Str.NumBonds() << endl;
    MsgOut << "   Number of residues = " << Str.NumResidues() << endl;

    if( Options.GetOptInH() != "keep" ){
    MsgOut << "   === Modifying hydrogen atoms: " << Options.GetOptInH()  << endl;
        Str.AlterHydrogens(Options.GetOptInH());
        MsgOut << "   Number of atoms    = " << Str.NumAtoms() << endl;
        MsgOut << "   Number of bonds    = " << Str.NumBonds() << endl;
        MsgOut << "   Number of residues = " << Str.NumResidues() << endl;
    }

    bool result = true;

    MsgOut << endl;
    MsgOut << "2) Re-indexing structure ..." << endl;

    size_t idx = 0;
    std::vector<OBAtom*> newatoms(Str.NumAtoms(), nullptr);
    FOR_RESIDUES_OF_MOL(p_r,Str){
        FOR_ATOMS_OF_RESIDUE(p_a,*p_r){
            newatoms[idx++] = &*p_a;
        }
    }
    Str.RenumberAtoms(newatoms);

    if( result == false ){
        ES_ERROR("Resorting error!");
        return(true);
    }

    MsgOut << endl;
    MsgOut << "3) Saving re-sorted structure ..." << endl;
    MsgOut << "   Number of atoms    = " << Str.NumAtoms() << endl;
    MsgOut << "   Number of bonds    = " << Str.NumBonds() << endl;
    MsgOut << "   Number of residues = " << Str.NumResidues() << endl;
    if( Options.GetOptOutH() != "keep" ){
    MsgOut << "   === Modifying hydrogen atoms: " << Options.GetOptOutH()  << endl;
        Str.AlterHydrogens(Options.GetOptOutH());
        MsgOut << "   Number of atoms    = " << Str.NumAtoms() << endl;
        MsgOut << "   Number of bonds    = " << Str.NumBonds() << endl;
        MsgOut << "   Number of residues = " << Str.NumResidues() << endl;
    }
    Str.WriteMol(Options.GetArgOutStrName(),Options.GetOptOutFormat());

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCompactRes::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# compactres terminated at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;

    if( ErrorSystem.IsError() || Options.GetOptVerbose() ){
        MsgOut << low;
        ErrorSystem.PrintErrors(stderr);
    }

    MsgOut << endl;
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

