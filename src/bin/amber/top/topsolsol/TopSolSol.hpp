#ifndef TopSolSolH
#define TopSolSolH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu
//    Copyright (C) 2005 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "TopSolSolOptions.hpp"
#include <AmberTopology.hpp>
#include <AmberMaskResidues.hpp>
#include <SimpleList.hpp>
#include <AmberAtom.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>

//---------------------------------------------------------------------------

/*
main molecule types:
    0 - unknown
    1 - solut
    2 - solvent

subtypes
   -2 - anihilated
   -1 - direct contact
    0 - unknown
    1 - first solvatation shell
    2 - second solvatation shell
    3 - third solvatation shell
    4 - fourth solvatation shell
    5 - free solvent
    6 - cavity solvent
*/

//---------------------------------------------------------------------------

class CMyResidue;
class CAmberResidue;

//---------------------------------------------------------------------------

class CMyNeighbour {
public:
    CMyResidue*                 FirstResidue;
    CMyResidue*                 SecondResidue;
    CSimpleList<CAmberAtom>     AtomsInContact;
    int                         NumberOfContacts;
    // it does not have to be same as AtomsInContact.NumOfMembers()
    // because of shared atoms !
    // 1-2 3-2 -> AtomsInContact.NumOfMembers() = 3
    // NumberOfContacts = 2

public:
    CMyNeighbour(void);
    CMyResidue* GetNeighbourResidue(CMyResidue* p_res);
};

//---------------------------------------------------------------------------

class CMyResidue {
public:
    CAmberResidue*              AmberResidue;
    CSimpleList<CMyNeighbour>   Neighbours;
    int                         MainType;
    int                         SubType;
    bool                        Processed;

public:
    CMyResidue(void);
    void AssignNeighbour(CMyResidue* p_neighbour,CAmberAtom* p_myatom,CAmberAtom* p_neatom);
};

//------------------------------------------------------------------------------

class CTopSolSol {
public:
    // constructor
    CTopSolSol(void);
    ~CTopSolSol(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    bool Finalize(void);

// section of private data ----------------------------------------------------
private:
    CTopSolSolOptions       Options;            // program options
    CAmberTopology          Topology;
    CAmberMaskResidues      SoluteMask;
    CAmberMaskResidues      SolventMask;
    FILE*                   OutputMask;

    CMyResidue*             MyResidues;
    int                     DirectlyBoundSolvent;
    int                     FirstShell;
    int                     SecondShell;
    int                     ThirdShell;
    int                     FourthShell;
    int                     FreeSolvent;
    int                     CavitySolvent;

    // determine if solvent residue is anihilated in refinement of directly bound solvent residues
    // if ratio solvent contacts / total contacts is higher than TightSolventRatio then
    // solvent residue is marked as first shell solvent
    // double                  DirectlyBoundSolventRatio;
    // if true, contacts with directly bound solvent residues are not counted as solvent contacts
    // bool                    DoNotUseDirectAsSolvent;

    bool PrepareAdditionalTopInfo(void);
    bool CleanAdditionalTopInfo(void);
    bool LoadDelaunayInfo(const char* p_fname);

    void MarkDirectSolventResidues(void);
    void MarkSecondShellResidues(void);
    void MarkThirdShellResidues(void);
    void MarkFourthShellResidues(void);
    void MarkFreeSolventResidues(void);
    void MarkFreeSolventInPass(int pass,int& num_res);
    void MarkCavitySolventResidues(void);
    void MarkFirstShellResidues(void);

    void AnihilateDirectlyBoundSolventResidues(void);
    void Anihilate(int pass,int& num_mol);

    void PrintDirectlyBoundSolventResidues(void);
    void PrintDirectlyBoundSolventStatistics(void);
    void PrintFirstShellSolventResidues(void);
    void PrintSecondShellSolventResidues(void);
    void PrintThirdShellSolventResidues(void);
    void PrintFourthShellSolventResidues(void);
};

//------------------------------------------------------------------------------

#endif
