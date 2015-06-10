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

#include <stdio.h>
#include <ErrorSystem.hpp>
#include <SmallTimeAndDate.hpp>
#include <AmberTopology.hpp>
#include "TopSolSol.hpp"
#include <errno.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopSolSol::CTopSolSol(void)
{
    MyResidues = NULL;
    OutputMask = NULL;

    DirectlyBoundSolvent = 0;
    FirstShell = 0;
    SecondShell = 0;
    ThirdShell = 0;
    FourthShell = 0;
    FreeSolvent = 0;
    CavitySolvent = 0;
}

//------------------------------------------------------------------------------

CTopSolSol::~CTopSolSol(void)
{
    CleanAdditionalTopInfo();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopSolSol::Init(int argc,char* argv[])
{
    // encode program options
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    if( Options.GetOptVerbose() ) {
        // print header --------------------------------------------------------------
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        printf("\n");
        printf("# ==============================================================================\n");
        printf("# topsolsol (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        printf("# Topology name                : %s\n",(const char*)Options.GetArgTopologyName());
        printf("# Delaunay triangulation       : %s\n",(const char*)Options.GetArgDelaunayTriangulation());
        printf("# Solute mask                  : %s\n",(const char*)Options.GetArgSoluteMask());
        printf("# Solvent mask                 : %s\n",(const char*)Options.GetArgSolventMask());
        printf("# Output masks                 : %s\n",(const char*)Options.GetArgOutputMasks());
        printf("# ------------------------------------------------------------------------------\n");
        printf("# Directly bound solvent ratio : %f\n",Options.GetOptDirectlyBoundSolventRatio());
        printf("# Do not use direct as solvent : %s\n",
               bool_to_str(Options.GetOptDoNotUseDirectAsSolvent()));
        printf("# ------------------------------------------------------------------------------\n");
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopSolSol::Run(void)
{
    if( Options.GetOptVerbose() ) printf("Loading topology, please wait ...\n");

    if( Topology.Load(Options.GetArgTopologyName()) == false ) {
        fprintf(stderr,"\n");
        fprintf(stderr,">>> ERROR: Unable to load specified topology: %s\n",
                (const char*)Options.GetArgTopologyName());
        return(false);
    }
    if( Options.GetOptVerbose() ) printf("Topology was succesfully loaded!\n");
    if( Options.GetOptVerbose() ) Topology.PrintInfo(true);

    // init masks ----------------------------------------------------------------
    // all mask are residue based mask

    SoluteMask.AssignTopology(&Topology);
    SolventMask.AssignTopology(&Topology);

    if( SoluteMask.SetMask(Options.GetArgSoluteMask()) == false ) {
        if( Options.GetOptVerbose() ) printf("\n");
        return(false);
    }
    if( Options.GetOptVerbose() ) {
        printf("Solute mask definition\n");
        printf("--------------------------------------\n");
        SoluteMask.PrintInfo(true);
    }

    if( SolventMask.SetMask(Options.GetArgSolventMask()) == false ) {
        if( Options.GetOptVerbose() ) printf("\n");
        return(false);
    }
    if( Options.GetOptVerbose() ) {
        printf("Solvent mask definition\n");
        printf("--------------------------------------\n");
        SolventMask.PrintInfo(true);
    }

    // prepare additional topology info ------------------------------------------
    if( Options.GetOptVerbose() ) printf("Preparing additional topology information, please wait ...\n");
    if( PrepareAdditionalTopInfo() == false ) {
        return(false);
    }
    if( Options.GetOptVerbose() ) printf("Additional topology information was successfully prepared!\n\n");

    // assign delaunay triangulation ---------------------------------------------
    if( Options.GetOptVerbose() ) printf("Loading Delaunay triangulation, please wait ...\n");
    if( LoadDelaunayInfo(Options.GetArgDelaunayTriangulation()) == false ) {
        return(false);
    }
    if( Options.GetOptVerbose() ) printf("Delaunay triangulation was successfully loaded!\n\n");

    // search for individual solvent molecules

    if( Options.GetOptVerbose() ) {
        printf("Analysing solut/solvent relationship ...\n");
        printf("======================================================================\n\n");
    }

    // Stage 01 - mark solvent residues in direct contact with solute
    // all solvent residues that have at least one contact are marked
    MarkDirectSolventResidues();

    // Stage 02 - mark second solvation shell
    // eg. all residues that have at least one contact with direct bounded solvent residues
    MarkSecondShellResidues();

    // Stage 03 - mark third solvation shell
    // eg. all residues that have at least one contact with second shell
    MarkThirdShellResidues();

    // Stage 04 - mark fourth solvation shell
    // eg. all residues that have at least one contact with third shell
    MarkFourthShellResidues();

    // Stage 05 - find free solvent molecules
    // in iterative process, it marks all solvent residues
    // that has contact with free solvent or fourth solvation shell
    MarkFreeSolventResidues();

    // Stage 06 - find cavity solvent residues
    MarkCavitySolventResidues();

    // ------------
    // Stage 07 - mark such solvent residues that are in direct contact and
    // they have also contact with second shell
    // such solvent residues belongs to first solvation shell
    MarkFirstShellResidues();

    // Stage 08 - focus boundary between shells
    // focusing is not further supported

    // Stage 09 - anihilate directly bound solvent residues
    // in this stage, real directly bound solvent residues are determined
    AnihilateDirectlyBoundSolventResidues();

    if( Options.GetOptVerbose() ) {
        printf("======================================================================\n");
        printf("All is done.\n\n");
    }

    if( Options.GetArgOutputMasks() != "-" ) {
        if( (OutputMask = fopen(Options.GetArgOutputMasks(),"w")) == NULL ) {
            fprintf(stderr,">>> ERROR: Unable to open output file (%s)!\n",strerror(errno));
            return(false);
        }
    } else {
        OutputMask = stdout;
    }

    // Stage 10 - write final result
    PrintDirectlyBoundSolventResidues();
    PrintFirstShellSolventResidues();
    PrintSecondShellSolventResidues();
    PrintThirdShellSolventResidues();
    PrintFourthShellSolventResidues();

    if( Options.GetOptVerbose() ) PrintDirectlyBoundSolventStatistics();

    if( Options.GetArgOutputMasks() != "-" ) {
        fclose(OutputMask);
    }

    // clean up all --------------------------------------------------------------
    CleanAdditionalTopInfo();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopSolSol::Finalize(void)
{
    if( Options.GetOptVerbose() ) {
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        fprintf(stdout,"\n");
        fprintf(stdout,"# ==============================================================================\n");
        fprintf(stdout,"# %s terminated at %s\n",(const char*)Options.GetProgramName(),(const char*)dt.GetSDateAndTime());
        fprintf(stdout,"# ==============================================================================\n");
    }

    if( Options.GetOptVerbose() || ErrorSystem.IsError() ) {
        ErrorSystem.PrintErrors(stderr);
        fprintf(stdout,"\n");
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopSolSol::PrepareAdditionalTopInfo(void)
{
    MyResidues = new CMyResidue[Topology.ResidueList.GetNumberOfResidues()];
    if( MyResidues == NULL ) return(false);

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        CMyResidue* p_mr = &MyResidues[i];
        p_mr->AmberResidue = Topology.ResidueList.GetResidue(i);
        p_mr->MainType = 0;
        p_mr->SubType  = 0;
        p_mr->Processed = false;

        if( SoluteMask.IsResidueSelected(i) == true ) {
            p_mr->MainType = 1;
        }

        if( SolventMask.IsResidueSelected(i) == true ) {
            if( p_mr->MainType != 0 ) {
                printf(" WARNING: Mask overlap!!!\n");
            }
            p_mr->MainType = 2;
        }
    }

    return(true);
}

//---------------------------------------------------------------------------

bool CTopSolSol::CleanAdditionalTopInfo(void)
{
    if( MyResidues != NULL ) delete[] MyResidues;
    MyResidues = NULL;

    return(true);
}

//------------------------------------------------------------------------------

bool CTopSolSol::LoadDelaunayInfo(const char* p_fname)
{
    FILE* p_delaunay;

    if( (p_delaunay = fopen(p_fname,"r")) == NULL ) {
        fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Unable to open output file '%s' with delaunay triangulation!\n",p_fname);
        fprintf(stderr,"\n");
        return(false);
    }

    int num_regions = 0;

    fscanf(p_delaunay,"%d",&num_regions);

    for(int i = 0; i < num_regions; i++) {
        int ia,ib,ic,id;
        if( fscanf(p_delaunay,"%d %d %d %d",&ia,&ib,&ic,&id) != 4 ) {
            fprintf(stderr,"\n");
            fprintf(stderr," ERROR: Unable to load delaunay edges!\n");
            fprintf(stderr,"\n");
            return(false);
        }

        // assign edges
        CAmberAtom* p_aatom = NULL;
        CMyResidue* p_amol  = NULL;

        CAmberAtom* p_batom = NULL;
        CMyResidue* p_bmol  = NULL;

        CAmberAtom* p_catom = NULL;
        CMyResidue* p_cmol  = NULL;

        CAmberAtom* p_datom = NULL;
        CMyResidue* p_dmol  = NULL;

        if( ia < Topology.AtomList.GetNumberOfAtoms() ) {
            p_aatom = Topology.AtomList.GetAtom(ia);
            p_amol = &MyResidues[p_aatom->GetResidue()->GetIndex()];
        }
        if( ib < Topology.AtomList.GetNumberOfAtoms() ) {
            p_batom = Topology.AtomList.GetAtom(ib);
            p_bmol = &MyResidues[p_batom->GetResidue()->GetIndex()];
        }
        if( ic < Topology.AtomList.GetNumberOfAtoms() ) {
            p_catom = Topology.AtomList.GetAtom(ic);
            p_cmol = &MyResidues[p_catom->GetResidue()->GetIndex()];
        }
        if( id < Topology.AtomList.GetNumberOfAtoms() ) {
            p_datom = Topology.AtomList.GetAtom(id);
            p_dmol = &MyResidues[p_datom->GetResidue()->GetIndex()];
        }

        if( (p_amol != NULL) && (p_bmol != NULL) ) p_amol->AssignNeighbour(p_bmol,p_aatom,p_batom);
        if( (p_amol != NULL) && (p_cmol != NULL) ) p_amol->AssignNeighbour(p_cmol,p_aatom,p_catom);
        if( (p_amol != NULL) && (p_dmol != NULL) ) p_amol->AssignNeighbour(p_dmol,p_aatom,p_datom);
        if( (p_bmol != NULL) && (p_cmol != NULL) ) p_bmol->AssignNeighbour(p_cmol,p_batom,p_catom);
        if( (p_bmol != NULL) && (p_dmol != NULL) ) p_bmol->AssignNeighbour(p_dmol,p_batom,p_datom);
        if( (p_cmol != NULL) && (p_dmol != NULL) ) p_cmol->AssignNeighbour(p_dmol,p_catom,p_datom);
    }

    fclose(p_delaunay);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTopSolSol::MarkDirectSolventResidues(void)
{
    int  num_mol = 0;

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        CMyResidue*  p_res = &MyResidues[i];
        if( p_res->MainType == 1 ) { // if it is solute then explore neighbours residues

            CSimpleIterator<CMyNeighbour> R(p_res->Neighbours);
            CMyNeighbour* p_n;
            while((p_n = R.Current())!=NULL) {
                CMyResidue* p_nres = p_n->GetNeighbourResidue(p_res);
                if( (p_nres->MainType == 2) && (p_nres->SubType != -1)  ) { // it is solvent ->
                    p_nres->SubType = -1;
                    num_mol++;
                }
                R++;
            }
        }
    }

    if( Options.GetOptVerbose() ) {
        printf("Solvent residues directly bound to solut      : %6d\n",num_mol);
    }

    DirectlyBoundSolvent = num_mol;
}

//---------------------------------------------------------------------------

void CTopSolSol::MarkSecondShellResidues(void)
{
    int  num_mol = 0;

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        CMyResidue*  p_res = &MyResidues[i];
        if( (p_res->MainType == 2) && (p_res->SubType == 0) ) {
            // if it is solvent then explore neighbours residues

            CSimpleIterator<CMyNeighbour> R(p_res->Neighbours);
            CMyNeighbour* p_n;

            bool correct_type = false;

            while((p_n = R.Current())!=NULL) {
                CMyResidue* p_nres = p_n->GetNeighbourResidue(p_res);
                if( (p_nres->MainType == 2) && (p_nres->SubType == -1)  ) { // it is solvent second shell
                    correct_type = true;
                    break;
                }
                R++;
            }

            if( correct_type == true ) {
                p_res->SubType = 2;
                num_mol++;
            }
        }
    }

    if( Options.GetOptVerbose() ) {
        printf("Second shell residues                         : %6d\n",num_mol);
    }

    SecondShell = num_mol;
}

//---------------------------------------------------------------------------

void CTopSolSol::MarkThirdShellResidues(void)
{
    int  num_mol = 0;

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        CMyResidue*  p_res = &MyResidues[i];
        if( (p_res->MainType == 2) && (p_res->SubType == 0) ) {
            // if it is solvent then explore neighbours residues

            CSimpleIterator<CMyNeighbour> R(p_res->Neighbours);
            CMyNeighbour* p_n;

            bool correct_type = false;

            while((p_n = R.Current())!=NULL) {
                CMyResidue* p_nres = p_n->GetNeighbourResidue(p_res);
                if( (p_nres->MainType == 2) && (p_nres->SubType == 2)  ) {
                    correct_type = true;
                    break;
                }
                R++;
            }

            if( correct_type == true ) {
                p_res->SubType = 3;
                num_mol++;
            }
        }
    }

    if( Options.GetOptVerbose() ) {
        printf("Third shell residues                          : %6d\n",num_mol);
    }

    ThirdShell = num_mol;
}

//---------------------------------------------------------------------------

void CTopSolSol::MarkFourthShellResidues(void)
{
    int  num_mol = 0;

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        CMyResidue*  p_res = &MyResidues[i];
        if( (p_res->MainType == 2) && (p_res->SubType == 0) ) {
            // if it is solvent then explore neighbours residues

            CSimpleIterator<CMyNeighbour> R(p_res->Neighbours);
            CMyNeighbour* p_n;

            bool correct_type = false;

            while((p_n = R.Current())!=NULL) {
                CMyResidue* p_nres = p_n->GetNeighbourResidue(p_res);
                if( (p_nres->MainType == 2) && (p_nres->SubType == 3)  ) {
                    correct_type = true;
                    break;
                }
                R++;
            }

            if( correct_type == true ) {
                p_res->SubType = 4;
                num_mol++;
            }
        }
    }

    if( Options.GetOptVerbose() ) {
        printf("Fourth shell residues                         : %6d\n",num_mol);
    }

    FourthShell = num_mol;
}

//---------------------------------------------------------------------------

void CTopSolSol::MarkFreeSolventResidues(void)
{
    if( Options.GetOptVerbose() ) {
        printf("\n");
        printf("Iterative determination of free solvent.\n");
        printf("----------------------------------------------------------------------\n");
    }

    int num_res = 0;
    int tot_res = 0;
    int pass = 1;
    do {
        MarkFreeSolventInPass(pass,num_res);
        tot_res += num_res;
        pass++;
    } while(num_res != 0);

    if( Options.GetOptVerbose() ) {
        printf("Free solvent residues                         : %6d\n",tot_res);
        printf("\n");
    }

    FreeSolvent = tot_res;
}

//---------------------------------------------------------------------------

void CTopSolSol::MarkFreeSolventInPass(int pass,int& num_res)
{
    num_res = 0;

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        CMyResidue*  p_res = &MyResidues[i];
        if( (p_res->MainType == 2) && (p_res->SubType == 0) ) {
            // if it is solvent then explore neighbours residues

            CSimpleIterator<CMyNeighbour> R(p_res->Neighbours);
            CMyNeighbour* p_n;

            bool correct_type = false;

            while((p_n = R.Current())!=NULL) {
                CMyResidue* p_nres = p_n->GetNeighbourResidue(p_res);
                if( (p_nres->MainType == 2) && ((p_nres->SubType == 4)||(p_nres->SubType == 5))  ) {
                    correct_type = true;
                    break;
                }
                R++;
            }

            if( correct_type == true ) {
                p_res->SubType = 5;
                num_res++;
            }
        }
    }

    if( Options.GetOptVerbose() ) {
        printf("Free solvent residues for pass   %6d       : %6d\n",pass,num_res);
    }
}

//---------------------------------------------------------------------------

void CTopSolSol::MarkCavitySolventResidues(void)
{
    // in present stage chain, DirectlyBoundSolvent contains also first solvation shell

    int found_solvent = DirectlyBoundSolvent + SecondShell + ThirdShell + FourthShell + FreeSolvent;

    if( found_solvent == SolventMask.GetNumberOfSelectedResidues() ) {
        if( Options.GetOptVerbose() ) {
            printf("All solvent is marked. No cavity solvent exists.\n");
            printf("\n");
        }
        return;
    }

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        CMyResidue*  p_res = &MyResidues[i];
        if( (p_res->MainType == 2) && (p_res->SubType == 0) ) {
            // unmarked solvent belong probably to some cavity
            p_res->SubType = 6;
            CavitySolvent++;
        }
    }

    if( Options.GetOptVerbose() ) {
        printf("Cavity solvent residues                       : %d6\n",CavitySolvent);
        printf("\n");
    }

    found_solvent += CavitySolvent;

    if( found_solvent == SolventMask.GetNumberOfSelectedResidues() ) {
        if( Options.GetOptVerbose() ) {
            printf("All solvent is marked.\n");
            printf("\n");
        }
        return;
    } else {
        if( Options.GetOptVerbose() ) {
            printf("WARNING: Some solvent residues were not marked (%d)!\n",
                   SolventMask.GetNumberOfSelectedResidues() - found_solvent );
            printf("\n");
        }
        return;
    }
}

//---------------------------------------------------------------------------

void CTopSolSol::MarkFirstShellResidues(void)
{
    int  num_mol = 0;

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        CMyResidue*  p_res = &MyResidues[i];
        if( (p_res->MainType == 2) && (p_res->SubType == -1) ) {
            // if it is solvent then explore neighbours residues

            CSimpleIterator<CMyNeighbour> R(p_res->Neighbours);
            CMyNeighbour* p_n;

            bool correct_type = false;

            while((p_n = R.Current())!=NULL) {
                CMyResidue* p_nres = p_n->GetNeighbourResidue(p_res);
                if( (p_nres->MainType == 2) && (p_nres->SubType == 2)  ) {
                    correct_type = true;
                    break;
                }
                R++;
            }

            if( correct_type == true ) {
                p_res->SubType = 1;
                num_mol++;
            }
        }
    }

    if( Options.GetOptVerbose() ) {
        printf("First shell residues                          : %6d\n",num_mol);
    }

    ThirdShell = num_mol;
    DirectlyBoundSolvent -= num_mol;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTopSolSol::AnihilateDirectlyBoundSolventResidues(void)
{
    int  num_mol = 0;

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        CMyResidue*  p_res = &MyResidues[i];
        if( (p_res->MainType == 2) && (p_res->SubType == -1) ) {
            num_mol++;
        }
    }

    if( Options.GetOptVerbose() ) {
        printf("\n");
        printf("Iterative anihilation of directly bound solvent residues.\n");
        printf("----------------------------------------------------------------------\n");
        printf("Solvent residues directly bound to solut      : %6d (initial guess)\n",num_mol);
        printf("Threshold for solvent/total contact           : %f\n",
               Options.GetOptDirectlyBoundSolventRatio());
        if( Options.GetOptDoNotUseDirectAsSolvent() == true ) {
            printf("-> Contacts with directly bound solvent are excluded from treshold. <-\n");
        }
        printf("----------------------------------------------------------------------\n");
    }

    int pass = 1;
    int num_sol = 0;

    do {
        Anihilate(pass,num_sol);
        pass++;
    } while( num_sol > 0 );

    if( Options.GetOptVerbose() ) {
        printf("\n");
    }
}

//---------------------------------------------------------------------------

void CTopSolSol::Anihilate(int pass,int& num_mol)
{
    CMyResidue*    p_res;

    num_mol = 0;

    for(int i=1; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        p_res = &MyResidues[i];
        if( p_res->SubType == -1 ) { // analyse direct
            // determine number of solute, semi and solvent contacts
            CSimpleIterator<CMyNeighbour> N(p_res->Neighbours);
            CMyNeighbour* p_n;
            CMyResidue*   p_nres;

            int solute = 0;
            int direct = 0;
            int solvent= 0;
            int total  = 0;

            while( (p_n = N.Current()) != NULL ) {
                p_nres = p_n->GetNeighbourResidue(p_res);
                total += p_n->NumberOfContacts;

                if( p_nres->MainType == 1 ) {
                    solute += p_n->NumberOfContacts;
                }
                if( p_nres->MainType == 2 ) {
                    if( p_nres->SubType == -1 ) {
                        direct += p_n->NumberOfContacts;
                    } else {
                        solvent += p_n->NumberOfContacts;
                    }
                }
                N++;
            }

            int sol_contact;
            if( Options.GetOptDoNotUseDirectAsSolvent() == true ) {
                sol_contact = solvent;
            } else {
                sol_contact = solvent + direct;
            }
            if( (double)sol_contact/(double)total > Options.GetOptDirectlyBoundSolventRatio() ) {
                p_res->SubType = 1; // first shell
                num_mol++;
            }
        }
    }
    if( Options.GetOptVerbose() ) {
        printf("Number of anihilated solvent residues in pass %d is %d\n",pass,num_mol);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTopSolSol::PrintDirectlyBoundSolventResidues(void)
{
    CMyResidue*    p_res;
    int            num_mol = 0;
    bool           first = true;

    fprintf(OutputMask,":");

    for(int i=1; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        p_res = &MyResidues[i];
        if( p_res->SubType == -1 ) {
            num_mol++;
            if( first == false ) fprintf(OutputMask,",");
            fprintf(OutputMask,"%d",p_res->AmberResidue->GetIndex()+1);
            first = false;
        }
    }
    fprintf(OutputMask,"\n");

    if( Options.GetOptVerbose() ) {
        printf("Directly bound solvent residues                : %d\n",num_mol);
    }
}

//---------------------------------------------------------------------------

void CTopSolSol::PrintDirectlyBoundSolventStatistics(void)
{
    CMyResidue*    p_res;

    printf("\n");
    printf("Statistics of directly bound solvent residues ...\n");
    printf("\n");
    printf("   Residue     Solvent   Direct   Solut   Sol+Dir/Total   Solvent/Total \n");
    printf("------------- --------- -------- ------- --------------- ---------------\n");

    for(int i=1; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        p_res = &MyResidues[i];
        if( p_res->SubType == -1 ) { // analyse direct
            // determine number of solute, semi and solvent contacts
            CSimpleIterator<CMyNeighbour> N(p_res->Neighbours);
            CMyNeighbour* p_n;
            CMyResidue*   p_nres;

            int solute = 0;
            int direct = 0;
            int solvent= 0;
            int total  = 0;

            while( (p_n = N.Current()) != NULL ) {
                p_nres = p_n->GetNeighbourResidue(p_res);
                total += p_n->NumberOfContacts;
                if( p_nres->MainType == 1 ) {
                    solute += p_n->NumberOfContacts;
                }
                if( p_nres->MainType == 2 ) {
                    if( p_nres->SubType == -1 ) {
                        direct += p_n->NumberOfContacts;
                    } else {
                        solvent += p_n->NumberOfContacts;
                    }
                }
                N++;
            }
            double ratio1 = (double)(solvent+direct)/(double)total;
            double ratio2 = (double)(solvent)/(double)total;
            printf(" %5d  %4s %9d %8d %7d %15.4f %15.4f\n",
                   p_res->AmberResidue->GetIndex()+1,p_res->AmberResidue->GetName(),
                   solvent,direct,solute,ratio1,ratio2);
        }
    }
}

//---------------------------------------------------------------------------

void CTopSolSol::PrintFirstShellSolventResidues(void)
{
    CMyResidue*    p_res;
    int            num_mol = 0;
    bool           first = true;

    fprintf(OutputMask,":");

    for(int i=1; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        p_res = &MyResidues[i];
        if( p_res->SubType == 1 ) {
            num_mol++;
            if( first == false ) fprintf(OutputMask,",");
            fprintf(OutputMask,"%d",p_res->AmberResidue->GetIndex()+1);
            first = false;
        }
    }
    fprintf(OutputMask,"\n");

    if( Options.GetOptVerbose() ) {
        printf("Solvent residues of the first solvation shell  : %d\n",num_mol);
    }
}

//---------------------------------------------------------------------------

void CTopSolSol::PrintSecondShellSolventResidues(void)
{
    CMyResidue*    p_res;
    int            num_mol = 0;
    bool           first = true;

    fprintf(OutputMask,":");

    for(int i=1; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        p_res = &MyResidues[i];
        if( p_res->SubType == 2 ) {
            num_mol++;
            if( first == false ) fprintf(OutputMask,",");
            fprintf(OutputMask,"%d",p_res->AmberResidue->GetIndex()+1);
            first = false;
        }
    }
    fprintf(OutputMask,"\n");

    if( Options.GetOptVerbose() ) {
        printf("Solvent residues of the second solvation shell : %d\n",num_mol);
    }
}

//---------------------------------------------------------------------------

void CTopSolSol::PrintThirdShellSolventResidues(void)
{
    CMyResidue*    p_res;
    int            num_mol = 0;
    bool           first = true;

    fprintf(OutputMask,":");

    for(int i=1; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        p_res = &MyResidues[i];
        if( p_res->SubType == 3 ) {
            num_mol++;
            if( first == false ) fprintf(OutputMask,",");
            fprintf(OutputMask,"%d",p_res->AmberResidue->GetIndex()+1);
            first = false;
        }
    }
    fprintf(OutputMask,"\n");

    if( Options.GetOptVerbose() ) {
        printf("Solvent residues of the third solvation shell  : %d\n",num_mol);
    }
}

//---------------------------------------------------------------------------

void CTopSolSol::PrintFourthShellSolventResidues(void)
{
    CMyResidue*    p_res;
    int            num_mol = 0;
    bool           first = true;

    fprintf(OutputMask,":");

    for(int i=1; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        p_res = &MyResidues[i];
        if( p_res->SubType == 4 ) {
            num_mol++;
            if( first == false ) fprintf(OutputMask,",");
            fprintf(OutputMask,"%d",p_res->AmberResidue->GetIndex()+1);
            first = false;
        }
    }
    fprintf(OutputMask,"\n");

    if( Options.GetOptVerbose() ) {
        printf("Solvent residues of the fourth solvation shell : %d\n",num_mol);
    }

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMyNeighbour::CMyNeighbour(void)
{
    FirstResidue = NULL;
    SecondResidue = NULL;
    NumberOfContacts = 0;
}

//------------------------------------------------------------------------------

CMyResidue* CMyNeighbour::GetNeighbourResidue(CMyResidue* p_res)
{
    if( p_res == FirstResidue ) return(SecondResidue);
    if( p_res == SecondResidue ) return(FirstResidue);

    fprintf(stderr," ERROR: mismatch in GetNeighbourResidue !\n");

    exit(1);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMyResidue::CMyResidue(void)
{
    AmberResidue = NULL;
    MainType = 0;
    SubType  = 0;
    Processed  = false;
}

//------------------------------------------------------------------------------

void CMyResidue::AssignNeighbour(CMyResidue* p_nres,CAmberAtom* p_myatom,CAmberAtom* p_neatom)
{
    if( p_nres == this ) return;  // self contact is eliminated

    // check if record exists
    CSimpleIterator<CMyNeighbour> N(Neighbours);
    CMyNeighbour* p_mn = NULL;

    while( (p_mn = N.Current()) != NULL ) {
        if( p_mn->FirstResidue == p_nres ) break;
        if( p_mn->SecondResidue == p_nres ) break;
        N++;
    }

    if( p_mn == NULL ) {
        // create new record
        p_mn = new CMyNeighbour;
        p_mn->FirstResidue = this;
        p_mn->SecondResidue = p_nres;
        Neighbours.InsertToEnd(p_mn,0,true);
        p_nres->Neighbours.InsertToEnd(p_mn);
    }

    // update atom maps
    bool new_connection = false;
    if( p_mn->AtomsInContact.IsInList(p_myatom) == false ) {
        new_connection = true;
        p_mn->AtomsInContact.InsertToEnd(p_myatom);
    }
    if( p_mn->AtomsInContact.IsInList(p_neatom) == false ) {
        new_connection = true;
        p_mn->AtomsInContact.InsertToEnd(p_neatom);
    }
    if( new_connection == true ) p_mn->NumberOfContacts++;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


