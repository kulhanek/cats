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

#include <ErrorSystem.hpp>
#include <SmallTimeAndDate.hpp>
#include "Top2Delphi.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTop2Delphi::CTop2Delphi(void)
{
    RadiiFile   = NULL;
    ChargesFile = NULL;
}

//------------------------------------------------------------------------------

CTop2Delphi::~CTop2Delphi(void)
{
    CloseOutputFiles();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTop2Delphi::Init(int argc,char* argv[])
{
    // encode program options
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    // print header --------------------------------------------------------------
    if( Options.GetOptVerbose() ) {
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        printf("\n");
        printf("# ==============================================================================\n");
        printf("# top2delphi (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        if( Options.GetArgTopologyName() == "-" ) {
            printf("# Topology file name       : -stdin- (standard input)");
        } else {
            printf("# Topology file name       : %s\n",(const char*)Options.GetArgTopologyName());
        }
        printf("# Delphi radii file name   : %s\n",(const char*)Options.GetArgRadiiName());
        printf("# Delphi charges file name : %s\n",(const char*)Options.GetArgChargesName());
        printf("# ------------------------------------------------------------------------------\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTop2Delphi::Run(void)
{
    // load topology
    if( LoadTopology() == false ) return(false);

    // open output files
    if( OpenOutputFiles() == false ) return(false);

    bool result = true;

    // print charges
    result &= PrintCharges();

    // print radii
    result &= PrintRadii();

    // close output files
    CloseOutputFiles();

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTop2Delphi::Finalize(void)
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

bool CTop2Delphi::LoadTopology(void)
{
    if( Topology.Load(Options.GetArgTopologyName(),true) == false ) {
        fprintf(stderr,"\n");
        fprintf(stderr,">>> ERROR: Unable to load specified topology: %s\n",
                (const char*)Options.GetArgTopologyName());
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTop2Delphi::OpenOutputFiles(void)
{
    if( (RadiiFile = fopen(Options.GetArgRadiiName(),"w")) == NULL ) {
        fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Unable to open output file for delphi radii '%s'!\n",(const char*)Options.GetArgRadiiName());
        fprintf(stderr,"\n");
        return(false);
    }

    if( (ChargesFile = fopen(Options.GetArgChargesName(),"w")) == NULL ) {
        fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Unable to open output file for delphi charges '%s'!\n",(const char*)Options.GetArgChargesName());
        fprintf(stderr,"\n");
        return(false);
    }

    CSmallTimeAndDate time;
    time.GetActualTimeAndDate();

    fprintf(RadiiFile,"!-----------------------------------------------------------------------------------------\n");
    fprintf(RadiiFile,"!                          ===== List of the atomic radii =====                           \n");
    fprintf(RadiiFile,"!-----------------------------------------------------------------------------------------\n");
    fprintf(RadiiFile,"! file generated with top2delphi\n");
    fprintf(RadiiFile,"! generated from topology : %s\n",(const char*)Options.GetArgTopologyName());
    fprintf(RadiiFile,"! date : %s\n",(const char*)time.GetSDateAndTime());
    fprintf(RadiiFile,"!\n");
    fprintf(RadiiFile,"! ***************************** THIS FILE IS NOT PORTABLE ******************************* \n");
    fprintf(RadiiFile,"!\n");
    fprintf(RadiiFile,"!-----------------------------------------------------------------------------------------\n");
    fprintf(RadiiFile,"atom__res_radius_\n");

    fprintf(ChargesFile,"!-----------------------------------------------------------------------------------------\n");
    fprintf(ChargesFile,"!                         ===== List of the atomic charges =====                          \n");
    fprintf(ChargesFile,"!-----------------------------------------------------------------------------------------\n");
    fprintf(ChargesFile,"! file generated with top2delphi                                                          \n");
    fprintf(ChargesFile,"! generated from topology : %s\n",(const char*)Options.GetArgTopologyName());
    fprintf(ChargesFile,"! date : %s\n",(const char*)time.GetSDateAndTime());
    fprintf(ChargesFile,"!\n");
    fprintf(ChargesFile,"! ***************************** THIS FILE IS NOT PORTABLE ******************************* \n");
    fprintf(ChargesFile,"!\n");
    fprintf(ChargesFile,"!-----------------------------------------------------------------------------------------\n");
    fprintf(ChargesFile,"atom__resnumbc_charge_\n");

    return(true);
}

//-----------------------------------------------------------------------------

bool CTop2Delphi::PrintCharges(void)
{
    // prepare array of residue pointers
    CAmberResidue** p_residues = new CAmberResidue*[Topology.ResidueList.GetNumberOfResidues()];
    if( p_residues == NULL ) {
        fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Unable to create array for residues!\n");
        fprintf(stderr,"\n");
        return(false);
    }

    // init residue array
    CAmberResidue* p_res = NULL;

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        p_res = Topology.ResidueList.GetResidue(i);
        p_residues[i] = p_res;
    }

    // sort array
    qsort(p_residues,Topology.ResidueList.GetNumberOfResidues(),sizeof(CAmberResidue*),CompareResidueItemByCharge);

    // now we loop over the list
    CAmberResidue* p_nres = NULL;

    int number_of_residues = 0;

    CAmberResidue* p_group_res = NULL;
    int            group_count = 0;
    int            group_index = 0;

    CAmberResidue* p_largest_group_res = NULL;
    int            largest_group_count = 0;
    int            largest_group_index = 0;

    // first print nonspecific residues ------------------------------------------
    // fprintf(ChargesFile,"!\n");
    // fprintf(ChargesFile,"! non specific residues ------------------------------------------------------------------\n");
    // fprintf(ChargesFile,"! these respresent standard residues or residues that occur only once\n");
    // fprintf(ChargesFile,"!\n");

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {

        p_res = p_residues[i];

        number_of_residues = 0;

        p_group_res = p_res;
        group_count = 1;
        group_index = i;

        p_largest_group_res = p_res;
        largest_group_count = 1;
        largest_group_index = i;

        // find largest group of current residue
        for(int j=i+1; j < Topology.ResidueList.GetNumberOfResidues(); j++) {
            p_nres = p_residues[j];

            if( strncmp(p_res->GetName(),p_nres->GetName(),4) == 0 ) {
                number_of_residues++;

                // we are still process the same residue
                if( CompareResidueItemByCharge(&p_group_res,&p_nres) == 0 ) {
                    // if residues are totaly identical then group_count will be increased
                    group_count++;
                } else {
                    // if residues are not identical then new group starts
                    if( largest_group_count < group_count ) {
                        // update largest group if necessary
                        largest_group_count = group_count;
                        p_largest_group_res = p_group_res;
                        largest_group_index = group_index;
                    }
                    p_group_res = p_nres;
                    group_count = 1;
                    group_index = j;
                }
            } else {
                // this is the end of the same residue part
                break;
            }
        }

        // correct largest group if last residue is processed or if actual residue forms only one group

        if( largest_group_count < group_count ) {
            // update largest group if necessary
            largest_group_count = group_count;
            p_largest_group_res = p_group_res;
            largest_group_index = group_index;
        }

        // write data of largest group for current residue
        //  fprintf(ChargesFile,"! non specific residue: %4s - total occurency: %d\n",p_largest_group_res->GetName(),largest_group_count);
        for(int j=0; j < p_largest_group_res->GetNumberOfAtoms(); j++) {
            CAmberAtom* p_atom = Topology.AtomList.GetAtom(j+p_largest_group_res->GetFirstAtomIndex());
            fprintf(ChargesFile,"%-6s%4s   %9.4f\n",
                    (const char*)p_atom->GetPDBName(),p_largest_group_res->GetName(),p_atom->GetStandardCharge());
        }

        // set all residues from largest group as processed

        for(int j=0; j < largest_group_count; j++) {
            p_residues[j + largest_group_index] = NULL;
        }

        i += number_of_residues;

    }

    // then print specific residues ------------------------------------------

    // fprintf(ChargesFile,"!\n");
    // fprintf(ChargesFile,"! specific residues ----------------------------------------------------------------------\n");
    // fprintf(ChargesFile,"! these respresent terminal or modified residues\n");
    // fprintf(ChargesFile,"!\n");

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        p_res = p_residues[i];
        if( p_res == NULL ) continue;
        for(int j=0; j < p_res->GetNumberOfAtoms(); j++) {
            CAmberAtom* p_atom = Topology.AtomList.GetAtom(j+p_res->GetFirstAtomIndex());
            if( p_atom != NULL ) {
                fprintf(ChargesFile,"%-6s%4s%3d%9.4f\n",(const char*)p_atom->GetPDBName(),p_res->GetName(),p_res->GetIndex()+1,p_atom->GetStandardCharge());
            }
        }
    }

    // clean up

    delete[] p_residues;

    return(true);
}

//---------------------------------------------------------------------------

int CTop2Delphi::CompareResidueItemByCharge(const void *p_left, const void *p_right)
{
    CAmberResidue* p_l = *((CAmberResidue**)p_left);
    CAmberResidue* p_r = *((CAmberResidue**)p_right);

    if( (p_l == NULL)||(p_r == NULL) ) return(0);

    CAmberTopology* p_top = p_l->GetTopology();

    int result;

    // compare by name
    result = strncmp(p_l->GetName(),p_r->GetName(),4);

    if( result != 0 ) return(result);

    // compare by number of atoms

    if( p_l->GetNumberOfAtoms() != p_r->GetNumberOfAtoms() ) {
        return(p_l->GetNumberOfAtoms()-p_r->GetNumberOfAtoms());
    }

    // compare by atom charges
    double lcharge = 0.0;
    double rcharge = 0.0;

    for(int i=0; i < p_l->GetNumberOfAtoms(); i++) {
        CAmberAtom* p_lat = p_top->AtomList.GetAtom(p_l->GetFirstAtomIndex()+i);
        CAmberAtom* p_rat = p_top->AtomList.GetAtom(p_r->GetFirstAtomIndex()+i);
        if( p_lat != NULL ) lcharge = p_lat->GetCharge();
        if( p_rat != NULL ) rcharge = p_rat->GetCharge();
        if( fabs(lcharge-rcharge) > 0.000001 ) break;
    }

    if( fabs(lcharge-rcharge) < 0.000001 ) return(0);
    if( lcharge > rcharge ) return(1);
    return(-1);
}

//------------------------------------------------------------------------------

bool CTop2Delphi::PrintRadii(void)
{
    // prepare array of residue pointers
    CAmberResidue** p_residues = new CAmberResidue*[Topology.ResidueList.GetNumberOfResidues()];
    if( p_residues == NULL ) {
        fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Unable to create array for residues!\n");
        fprintf(stderr,"\n");
        return(false);
    }

    // init residue array
    CAmberResidue* p_res = NULL;

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        p_res = Topology.ResidueList.GetResidue(i);
        p_residues[i] = p_res;
    }

    // sort array
    qsort(p_residues,Topology.ResidueList.GetNumberOfResidues(),sizeof(CAmberResidue*),CompareResidueItemByRadii);

    // now we loop over the list
    CAmberResidue* p_nres = NULL;

    int number_of_residues = 0;

    CAmberResidue* p_group_res = NULL;
    int            group_count = 0;
    int            group_index = 0;

    CAmberResidue* p_largest_group_res = NULL;
    int            largest_group_count = 0;
    int            largest_group_index = 0;

    // first print non-specific residues ------------------------------------------

    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {

        p_res = p_residues[i];

        number_of_residues = 0;

        p_group_res = p_res;
        group_count = 1;
        group_index = i;

        p_largest_group_res = p_res;
        largest_group_count = 1;
        largest_group_index = i;

        // find largest group of current residue
        for(int j=i+1; j < Topology.ResidueList.GetNumberOfResidues(); j++) {
            p_nres = p_residues[j];

            if( strncmp(p_res->GetName(),p_nres->GetName(),4) == 0 ) {
                number_of_residues++;

                // we are still process the same residue
                if( CompareResidueItemByRadii(&p_group_res,&p_nres) == 0 ) {
                    // if residues are totaly identical then group_count will be increased
                    group_count++;
                } else {
                    // if residues are not identical then new group starts
                    if( largest_group_count < group_count ) {
                        // update largest group if necessary
                        largest_group_count = group_count;
                        p_largest_group_res = p_group_res;
                        largest_group_index = group_index;
                    }
                    p_group_res = p_nres;
                    group_count = 1;
                    group_index = j;
                }
            } else {
                // this is the end of the same residue part
                break;
            }
        }

        // correct largest group if last residue is processed or if actual residue forms only one group

        if( largest_group_count < group_count ) {
            // update largest group if necessary
            largest_group_count = group_count;
            p_largest_group_res = p_group_res;
            largest_group_index = group_index;
        }

        // non-specific residues
        for(int j=0; j < p_largest_group_res->GetNumberOfAtoms(); j++) {
            CAmberAtom* p_atom = Topology.AtomList.GetAtom(j+p_largest_group_res->GetFirstAtomIndex());
            fprintf(RadiiFile,"%-6s%-4s%6.2f\n",(const char*)p_atom->GetPDBName(),p_largest_group_res->GetName(),p_atom->GetRadius());
        }

        // set all residues from largest group as processed

        for(int j=0; j < largest_group_count; j++) {
            p_residues[j + largest_group_index] = NULL;
        }

        i += number_of_residues;

    }

    // then print specific residues ------------------------------------------
    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
        p_res = p_residues[i];
        if( p_res == NULL ) continue;
        for(int j=0; j < p_res->GetNumberOfAtoms(); j++) {
            CAmberAtom* p_atom = Topology.AtomList.GetAtom(j+p_res->GetFirstAtomIndex());
            if( p_atom != NULL ) {
                fprintf(RadiiFile,"%-6s%-4s%6.2f\n",(const char*)p_atom->GetPDBName(),p_res->GetName(),p_atom->GetRadius());
            }
        }
    }

    // clean up
    delete[] p_residues;

    return(true);
}

//---------------------------------------------------------------------------

int CTop2Delphi::CompareResidueItemByRadii(const void *p_left, const void *p_right)
{
    CAmberResidue* p_l = *((CAmberResidue**)p_left);
    CAmberResidue* p_r = *((CAmberResidue**)p_right);

    if( (p_l == NULL)||(p_r == NULL) ) return(0);

    CAmberTopology* p_top = p_l->GetTopology();

    int result;

    // compare by name
    result = strncmp(p_l->GetName(),p_r->GetName(),4);

    if( result != 0 ) return(result);

    // compare by number of atoms

    if( p_l->GetNumberOfAtoms() != p_r->GetNumberOfAtoms() ) {
        return(p_l->GetNumberOfAtoms()-p_r->GetNumberOfAtoms());
    }

    // compare by atom radii
    double lradius = 0.0;
    double rradius = 0.0;

    for(int i=0; i < p_l->GetNumberOfAtoms(); i++) {
        CAmberAtom* p_lat = p_top->AtomList.GetAtom(p_l->GetFirstAtomIndex()+i);
        CAmberAtom* p_rat = p_top->AtomList.GetAtom(p_r->GetFirstAtomIndex()+i);
        if( p_lat != NULL ) lradius = p_lat->GetRadius();
        if( p_rat != NULL ) rradius = p_rat->GetRadius();
        if( fabs(lradius-rradius) > 0.01 ) break;
    }

    if( fabs(lradius-rradius) < 0.01 ) return(0);
    if( lradius > rradius ) return(1);
    return(-1);
}

//------------------------------------------------------------------------------

void CTop2Delphi::CloseOutputFiles(void)
{
    if( RadiiFile != NULL ) fclose(RadiiFile);
    RadiiFile = NULL;
    if( ChargesFile != NULL ) fclose(ChargesFile);
    ChargesFile = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

