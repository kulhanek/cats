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
#include <AmberMaskAtoms.hpp>
#include <AmberMaskResidues.hpp>

#include "TopMask.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopMask::CTopMask(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopMask::Init(int argc,char* argv[])
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
        printf("# topmask (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        if( Options.GetArgTopologyName() == "-" ) {
            printf("# Topology name      : -stdin- (standard input)\n");
        } else {
            printf("# Topology name      : %s\n",(const char*)Options.GetArgTopologyName());
        }
        if( Options.GetOptMaskFile() == false ) {
            printf("# Mask specification : %s\n",(const char*)Options.GetArgMaskSpec());
        } else {
            printf("# Mask file name     : %s\n",(const char*)Options.GetArgMaskSpec());
        }
        printf("# ------------------------------------------------------------------------------\n");
        if( Options.IsOptCrdNameSet() == true ) {
            printf("# Coordinate name    : %s\n",(const char*)Options.GetOptCrdName());
        } else {
            printf("# Coordinate name    : %s\n","-none-");
        }
        if( Options.GetOptResidueMask() == true ) {
            printf("# Residue mask       : yes\n");
        } else {
            printf("# Residue mask       : no\n");
        }
        printf("# Output format      : %s\n",(const char*)Options.GetOptOutputFormat());
        printf("# ------------------------------------------------------------------------------\n");
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopMask::Run(void)
{
    // load topology
    if( Topology.Load(Options.GetArgTopologyName(),true) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified topology: %s\n",
                (const char*)Options.GetArgTopologyName());
        return(false);
    }

    // load optional coordinates
    if( Options.IsOptCrdNameSet() == true ) {
        Coordinates.AssignTopology(&Topology);
        if( Coordinates.Load(Options.GetOptCrdName(),false,AMBER_RST_UNKNOWN) == false ) {
            fprintf(stderr,">>> ERROR: Unable to load specified coordinates: %s\n",
                    (const char*)Options.GetOptCrdName());
            return(false);
        }
    }

    // init and print mask
    if( Options.GetOptResidueMask() ) {
        return( ProcessResidueBasedMask() );
    } else {
        return( ProcessAtomBasedMask() );
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopMask::ProcessAtomBasedMask(void)
{
    CAmberMaskAtoms    mask;

    // assign topology and optionally coordinates -------------
    mask.AssignTopology(&Topology);
    if( Options.IsOptCrdNameSet() == true ) mask.AssignCoordinates(&Coordinates);

    // init mask ----------------------------------------------
    bool result;
    if( Options.GetOptMaskFile() == true ) {
        result = mask.SetMaskFromFile(Options.GetArgMaskSpec());
    } else {
        result = mask.SetMask(Options.GetArgMaskSpec());
    }

    if( result == false ) {
        fprintf(stderr,">>> ERROR: Unable to set specified mask!\n");
        return(false);
    }

    // print selected atoms -----------------------------------
    if( Options.GetOptOutputFormat() == "cats" ) {
        if( Options.GetOptVerbose() == false ) printf("\n");
        mask.PrintInfo();
        return(true);
    }

    if( Options.GetOptOutputFormat() == "list" ) {
        for(int i=0 ; i < mask.GetNumberOfTopologyAtoms(); i++) {
            CAmberAtom* p_atm = mask.GetSelectedAtom(i);
            if( p_atm == NULL ) continue;
            fprintf(stdout,"%6d %4s %5d %4s\n",
                    i+1,
                    p_atm->GetName(),
                    p_atm->GetResidue()->GetIndex()+1,
                    p_atm->GetResidue()->GetName());
        }
        return(true);
    }

    if( Options.GetOptOutputFormat() == "number" ) {
        fprintf(stdout,"%d",mask.GetNumberOfSelectedAtoms());
        fflush(stdout);
        return(true);
    }

    if( Options.GetOptOutputFormat() == "amber" ) {
        for(int i=0 ; i < mask.GetNumberOfTopologyAtoms(); i++) {
            CAmberAtom* p_atm = mask.GetSelectedAtom(i);
            if( p_atm == NULL ) continue;
            int beginning = i+1;
            // find the end of continuous array
            int end = beginning;
            for(int j=i+1 ; j < mask.GetNumberOfTopologyAtoms(); j++) {
                CAmberAtom* p_atm = mask.GetSelectedAtom(j);
                if( p_atm == NULL ) break;
                end = j+1;
            }
            fprintf(stdout," ATOM %6d %6d\n",beginning,end);
        }
        fflush(stdout);
        return(true);
    }

    if( Options.GetOptOutputFormat() == "bmask" ) {
        for(int i=0 ; i < mask.GetNumberOfTopologyAtoms(); i++) {
            CAmberAtom* p_atm = mask.GetSelectedAtom(i);
            if( p_atm == NULL ){
                fprintf(stdout,"0\n");
            } else {
                fprintf(stdout,"1\n");
            }
        }
        fflush(stdout);
        return(true);
    }

    ES_ERROR("not implemented");

    return(false);
}

//------------------------------------------------------------------------------

bool CTopMask::ProcessResidueBasedMask(void)
{
    CAmberMaskResidues    mask;

    // assign topology and optionally coordinates
    mask.AssignTopology(&Topology);
    if( Options.IsOptCrdNameSet() == true ) mask.AssignCoordinates(&Coordinates);

    // init mask ----------------------------------------------
    bool result;
    if( Options.GetOptMaskFile() == true ) {
        result = mask.SetMaskFromFile(Options.GetArgMaskSpec());
    } else {
        result = mask.SetMask(Options.GetArgMaskSpec());
    }

    if( result == false ) {
        fprintf(stderr,">>> ERROR: Unable to set specified mask!\n");
        return(false);
    }

    // print selected atoms -----------------------------------
    if( Options.GetOptOutputFormat() == "cats" ) {
        if( Options.GetOptVerbose() == false ) printf("\n");
        mask.PrintInfo();
        return(true);
    }

    if( Options.GetOptOutputFormat() == "list" ) {
        for(int i=0 ; i < mask.GetNumberOfTopologyResidues(); i++) {
            CAmberResidue* p_res = mask.GetSelectedResidue(i);
            if( p_res == NULL ) continue;
            fprintf(stdout,"%6d %4s\n",i+1,p_res->GetName());
        }
        return(true);
    }

    if( Options.GetOptOutputFormat() == "number" ) {
        fprintf(stdout,"%d",mask.GetNumberOfSelectedResidues());
        fflush(stdout);
        return(true);
    }

    if( Options.GetOptOutputFormat() == "amber" ) {
        for(int i=0 ; i < mask.GetNumberOfTopologyResidues(); i++) {
            CAmberResidue* p_res = mask.GetSelectedResidue(i);
            if( p_res == NULL ) continue;
            int beginning = i+1;
            // find the end of continuous array
            int end = beginning;
            for(int j=i+1 ; j < mask.GetNumberOfTopologyResidues(); j++) {
                CAmberResidue* p_res = mask.GetSelectedResidue(j);
                if( p_res == NULL ) break;
                end = j+1;
            }
            fprintf(stdout," RES %6d %6d\n",beginning,end);
        }
        fflush(stdout);
        return(true);
    }

    ES_ERROR("not implemented");

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopMask::Finalize(void)
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

