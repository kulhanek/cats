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

#include "JoinMols.hpp"
#include "JoinMolsOptions.hpp"

// openbabel
#include "openbabel/mol.h"
#include "openbabel/obiter.h"
#include "openbabel/atom.h"
#include "openbabel/bond.h"
#include "openbabel/residue.h"

using namespace std;
using namespace OpenBabel;

//------------------------------------------------------------------------------

CJoinMols JoinMols;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CJoinMols::CJoinMols(void)
{
}

//------------------------------------------------------------------------------

CJoinMols::~CJoinMols(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CJoinMols::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CJoinMolsOptions
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
    MsgOut << "# joimols started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    for(int i=0; i < Options.GetNumberOfProgArgs() - 1; i++){
    MsgOut << "# Input structure name    : " << Options.GetProgArg(i) << endl;
    }
    MsgOut << "# Output structure name   : " << Options.GetProgArg(Options.GetNumberOfProgArgs()-1) << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CJoinMols::Run(void)
{
    MsgOut << endl;
    MsgOut << "::::::::::::::::::::::::::::::: Processing Data ::::::::::::::::::::::::::::::" << endl;

    // read molecules --------------------------------
    MsgOut << endl;
    MsgOut << "1) Reading molecules ..." << endl;
    MsgOut << "   # Name               = " << Options.GetProgArg(0) <<  endl;
    if(Str.ReadMol(Options.GetProgArg(0),Options.GetOptInFormat()) == false) return(false);
    MsgOut << "     Number of atoms    = " << Str.NumAtoms() << endl;
    MsgOut << "     Number of bonds    = " << Str.NumBonds() << endl;
    MsgOut << "     Number of residues = " << Str.NumResidues() << endl;

    Str.SetChainsPerceived(true);                   // keep atom names and residues
    Str.DeleteData(OBGenericDataType::PairData);

    for(int i=1; i < Options.GetNumberOfProgArgs() - 1; i++){
        CInfMol       AddStr;
        MsgOut << "   # Name               = " << Options.GetProgArg(i) <<  endl;
        if(AddStr.ReadMol(Options.GetProgArg(i),Options.GetOptInFormat()) == false) return(false);
        MsgOut << "     Number of atoms    = " << AddStr.NumAtoms() << endl;
        MsgOut << "     Number of bonds    = " << AddStr.NumBonds() << endl;
        MsgOut << "     Number of residues = " << AddStr.NumResidues() << endl;

        AddStr.SetChainsPerceived(true);                   // keep atom names and residues
        AddStr.DeleteData(OBGenericDataType::PairData);

        // copy atoms
        std::map<OBAtom*,OBAtom*> oldAtomToNew;
        FOR_ATOMS_OF_MOL(p_oa, AddStr) {
            OBAtom* p_na = Str.NewAtom();
            p_na->Duplicate(&*p_oa);
            oldAtomToNew[&*p_oa] = p_na;
        }

        // copy bonds
        FOR_BONDS_OF_MOL(p_ob, AddStr) {
            Str.AddBond(oldAtomToNew[p_ob->GetBeginAtom()]->GetIdx(),oldAtomToNew[p_ob->GetEndAtom()]->GetIdx(),
                    p_ob->GetBondOrder(),p_ob->GetFlags());
        }

        // clone residues
        FOR_RESIDUES_OF_MOL(p_or, AddStr) {

            OBResidue *p_nr = Str.NewResidue();

            p_nr->SetName(p_or->GetName());
            p_nr->SetNum(p_or->GetNum());
            p_nr->SetChain(p_or->GetChain());
            p_nr->SetChainNum(p_or->GetChainNum());
            p_nr->SetInsertionCode(p_or->GetInsertionCode());

            // add atoms
            FOR_ATOMS_OF_RESIDUE(p_oa,*p_or) {
                OBAtom* p_na = oldAtomToNew[&*p_oa];
                p_nr->AddAtom(p_na);
                p_nr->SetAtomID(p_na,p_or->GetAtomID(&*p_oa));          // name
                p_nr->SetHetAtom(p_na,p_or->IsHetAtom(&*p_oa));         // ATOM vs HETATM
                // p_nr->SetSerialNum(p_na,p_or->GetSerialNum(&*p_oa));    // serial number - ignoed
            }
        }
    }

    if( Options.GetOptUniqueResID() == true ){
        MsgOut << "   # Making residue indexes unique ..." << endl;
        unsigned idx = 1;
        FOR_RESIDUES_OF_MOL(p_nr, Str) {
            p_nr->SetNum(idx++);
        }
    }

    MsgOut << endl;
    MsgOut << "2) Saving joined structure ..." << endl;
    MsgOut << "   # Name               = " << Options.GetProgArg(Options.GetNumberOfProgArgs()-1) <<  endl;
    MsgOut << "     Number of atoms    = " << Str.NumAtoms() << endl;
    MsgOut << "     Number of bonds    = " << Str.NumBonds() << endl;
    MsgOut << "     Number of residues = " << Str.NumResidues() << endl;

    if( Options.GetOptOutH() != "keep" ){
    MsgOut << "     === Modifying hydrogen atoms: " << Options.GetOptOutH()  << endl;
        Str.AlterHydrogens(Options.GetOptOutH());
    MsgOut << "     Number of atoms    = " << Str.NumAtoms() << endl;
    MsgOut << "     Number of bonds    = " << Str.NumBonds() << endl;
    MsgOut << "     Number of residues = " << Str.NumResidues() << endl;
    }

    Str.WriteMol(Options.GetProgArg(Options.GetNumberOfProgArgs()-1),Options.GetOptOutFormat(),Options.GetOptOutOptions());

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CJoinMols::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# joimols terminated at " << dt.GetSDateAndTime() << endl;
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

