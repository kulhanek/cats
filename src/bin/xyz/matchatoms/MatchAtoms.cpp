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

#include "MatchAtoms.hpp"
#include "MatchAtomsOptions.hpp"

// openbabel
#include "openbabel/mol.h"
#include "openbabel/atom.h"
#include "openbabel/obconversion.h"
#include "openbabel/generic.h"
#include "openbabel/graphsym.h"
#include "openbabel/canon.h"

using namespace std;
using namespace OpenBabel;

//------------------------------------------------------------------------------

CMatchAtoms MatchAtoms;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMatchAtoms::CMatchAtoms(void)
{
}

//------------------------------------------------------------------------------

CMatchAtoms::~CMatchAtoms(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CMatchAtoms::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CMatchAtomsOptions
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
    MsgOut << "# matchatoms started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# Reference molecule name : " << Options.GetProgArg(0) << endl;
    MsgOut << "# Input molecule name     : " << Options.GetProgArg(1) << endl;
    MsgOut << "# Output molecule name    : " << Options.GetProgArg(2) << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CMatchAtoms::Run(void)
{
    MsgOut << endl;
    MsgOut << "::::::::::::::::::::::::::::::: Processing Data ::::::::::::::::::::::::::::::" << endl;

    // read molecules --------------------------------
    MsgOut << endl;
    MsgOut << "1) Reading molecule #1 (template) ..." << endl;

    if(Ref.ReadMol(Options.GetProgArg(0),Options.GetOptRefFormat()) == false) return(false);

    Ref.SetChainsPerceived(true);                   // keep atom names and residues
    Ref.DeleteData(OBGenericDataType::PairData);    // remove REMARKS and other data

    MsgOut << "   Number of atoms    = " << Ref.NumAtoms() << endl;
    MsgOut << "   Number of bonds    = " << Ref.NumBonds() << endl;
    MsgOut << "   Number of residues = " << Ref.NumResidues() << endl;

    if( Options.GetOptRefH() != "keep" ){
    MsgOut << "   === Modifying hydrogen atoms: " << Options.GetOptRefH()  << endl;
        Ref.AlterHydrogens(Options.GetOptRefH());
        MsgOut << "   Number of atoms    = " << Ref.NumAtoms() << endl;
        MsgOut << "   Number of bonds    = " << Ref.NumBonds() << endl;
        MsgOut << "   Number of residues = " << Ref.NumResidues() << endl;
    }

    MsgOut << endl;
    MsgOut << "2) Reading molecule #2 (in) ..." << endl;
    if(Str.ReadMol(Options.GetProgArg(1),Options.GetOptInFormat()) == false) return(false);

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

    if( Ref.NumAtoms() != Str.NumAtoms() ){
        ES_ERROR("The molecules do not contain the same number of atoms!");
        return(false);
    }

    bool result = true;

// map structure to reference
    MsgOut << endl;
    if( Options.GetOptIdentity() == false ){

    // setup canonical labeling - reference
        std::vector<unsigned int> symmetry_classes_ref;
        OBGraphSym gs_ref(&Ref);
        gs_ref.GetSymmetry(symmetry_classes_ref);

        std::vector<unsigned int> canon_labels_ref;
        CanonicalLabels(&Ref, symmetry_classes_ref, canon_labels_ref);

    // setup canonical labeling - structure
        std::vector<unsigned int> symmetry_classes_str;
        OBGraphSym gs_str(&Str);
        gs_str.GetSymmetry(symmetry_classes_str);

        std::vector<unsigned int> canon_labels_str;
        CanonicalLabels(&Str, symmetry_classes_str, canon_labels_str);

        MsgOut << "3) Canonical mapping ..." << endl;
        for(unsigned int rid = 0; rid < Ref.NumAtoms(); rid++ ){
            unsigned int crid = canon_labels_ref[rid];
            // find this canonical label in the structure
            for(unsigned int sid = 0; sid < Str.NumAtoms(); sid++ ){
                unsigned int csid = canon_labels_str[sid];
                if( csid == crid ){
                    MsgOut << "found: " << setw(6) << rid << " -> " << setw(6) << crid << " | " << setw(6) << sid << " -> " << setw(6) << csid;
                    OBAtom* p_ratm = Ref.GetAtom(rid+1);
                    OBAtom* p_satm = Str.GetAtom(sid+1);
                    if( p_ratm->GetAtomicNum() != p_satm->GetAtomicNum() ){
                        MsgOut << " x but not the same Z! " << p_ratm->GetAtomicNum() << " vs " << p_satm->GetAtomicNum() << endl;
                        result = false;
                    } else {
                       p_ratm->SetVector(p_satm->GetVector());
                    }
                    MsgOut << endl;

                }
            }
        }
    } else {
        MsgOut << "3) Identity mapping ..." << endl;
        for(unsigned int rid = 0; rid < Ref.NumAtoms(); rid++ ){
            for(unsigned int sid = 0; sid < Str.NumAtoms(); sid++ ){
                OBAtom* p_ratm = Ref.GetAtom(rid+1);
                OBAtom* p_satm = Str.GetAtom(sid+1);
                p_ratm->SetVector(p_satm->GetVector());
            }
        }
    }

    MsgOut << endl;
    MsgOut << "4) Saving matched structure ..." << endl;
    MsgOut << "   Number of atoms    = " << Ref.NumAtoms() << endl;
    MsgOut << "   Number of bonds    = " << Ref.NumBonds() << endl;
    MsgOut << "   Number of residues = " << Ref.NumResidues() << endl;
    if( Options.GetOptOutH() != "keep" ){
    MsgOut << "   === Modifying hydrogen atoms: " << Options.GetOptOutH()  << endl;
        Ref.AlterHydrogens(Options.GetOptOutH());
        MsgOut << "   Number of atoms    = " << Ref.NumAtoms() << endl;
        MsgOut << "   Number of bonds    = " << Ref.NumBonds() << endl;
        MsgOut << "   Number of residues = " << Ref.NumResidues() << endl;
    }
    Ref.WriteMol(Options.GetProgArg(2),Options.GetOptOutFormat());

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CMatchAtoms::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# matchatoms terminated at " << dt.GetSDateAndTime() << endl;
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

