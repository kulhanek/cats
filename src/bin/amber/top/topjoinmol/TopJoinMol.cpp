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
#include "TopJoinMol.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopJoinMol::CTopJoinMol(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopJoinMol::Init(int argc,char* argv[])
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
        printf("# topjoinmol (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        if( Options.GetArgOldTopologyName() == "-" ) {
            printf("# Old topology name          : -stdin- (standard input)\n");
        } else {
            printf("# Old topology name          : %s\n",(const char*)Options.GetArgOldTopologyName());
        }
        if( Options.GetArgNewTopologyName() == "-" ) {
            printf("# New topology name          : -stdout- (standard output)\n");
        } else {
            printf("# New topology name          : %s\n",(const char*)Options.GetArgNewTopologyName());
        }
        printf("# Number of merged molecules : %d\n",Options.GetOptNumOfMolecules());
        printf("# ------------------------------------------------------------------------------\n");
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopJoinMol::Run(void)
{
    CAmberTopology topology;

    if( Options.GetOptVerbose() ) printf("Loading old topology ...\n");

    if( topology.Load(Options.GetArgOldTopologyName(),true) == false ) {
        fprintf(stderr,"\n");
        fprintf(stderr,">>> ERROR: Unable to load old topology: %s\n",
                (const char*)Options.GetArgOldTopologyName());
        return(false);
    }

    if( topology.BoxInfo.GetType() == AMBER_BOX_NONE ) {
        fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Only system with box can be processed!\n");
        fprintf(stderr,"\n");
        return(false);
    }

    if( Options.GetOptVerbose() ) topology.PrintInfo();

    if( Options.GetOptNumOfMolecules() > 1 ) {
        if( Options.GetOptVerbose() ) printf("Merging molecules ...\n");
        int nsolmol = topology.BoxInfo.GetNumberOfMolecules() - topology.BoxInfo.GetNumberOfSolventMolecules();
        if( Options.GetOptNumOfMolecules() > nsolmol ) {
            fprintf(stderr,"\n");
            fprintf(stderr," ERROR: Number of specified molecules '%d' is grater than the number of solute molecules '%d' !\n",Options.GetOptNumOfMolecules(),nsolmol);
            fprintf(stderr,"\n");
            return(false);
        }

        topology.BoxInfo.JoinSoluteMolecules(Options.GetOptNumOfMolecules());

        if( Options.GetOptVerbose() ) topology.PrintInfo();
    } else {
        if( Options.GetOptVerbose() ) {
            printf("The copy of old topology will be saved as new one (no merging will be performed) ...\n");
        }
    }

    if( Options.GetOptVerbose() ) printf("Saving new topology ...\n");

    if( topology.Save(Options.GetArgNewTopologyName(),AMBER_VERSION_NONE,true) == false ) {
        fprintf(stderr,"\n");
        fprintf(stderr,">>> ERROR: Unable to save new topology: %s\n",
                (const char*)Options.GetArgNewTopologyName());
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopJoinMol::Finalize(void)
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

