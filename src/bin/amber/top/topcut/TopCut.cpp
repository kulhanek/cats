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
#include <AmberSubTopology.hpp>

#include "TopCut.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopCut::CTopCut(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopCut::Init(int argc,char* argv[])
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
        printf("# topcut (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        if( Options.GetArgOldTopName() == "-" ) {
            printf("# Old topology name  : -stdin- (standard input)\n");
        } else {
            printf("# Old topology name  : %s\n",(const char*)Options.GetArgOldTopName());
        }
        if( Options.GetOptMaskFile() == false ) {
            printf("# Mask specification : %s\n",(const char*)Options.GetArgMaskSpec());
        } else {
            printf("# Mask file name     : %s\n",(const char*)Options.GetArgMaskSpec());
        }
        if( Options.GetArgNewTopName() == "-" ) {
            printf("# New topology name  : -stdout- (standard output)\n");
        } else {
            printf("# New topology name  : %s\n",(const char*)Options.GetArgNewTopName());
        }
        printf("# ------------------------------------------------------------------------------\n");
        if( Options.IsOptCrdNameSet() == true ) {
            printf("# Coordinate name    : %s\n",(const char*)Options.GetOptCrdName());
        } else {
            printf("# Coordinate name    : %s\n","-none-");
        }
        if( Options.GetOptCopyBox() == true ) {
            printf("# Copy box info      : yes\n");
        } else {
            printf("# Copy box info      : no\n");
        }
        if( Options.GetOptIgnoreErrors() == true ) {
            printf("# Ignore errors      : yes\n");
        } else {
            printf("# Ignore errors      : no\n");
        }
        if( Options.GetOptVer6() == true ) {
            printf("# Output format      : AMBER 6\n");
        } else {
            printf("# Output format      : AMBER 7\n");
        }
        printf("# ------------------------------------------------------------------------------\n");
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCut::Run(void)
{
    // load topology
    if( Topology.Load(Options.GetArgOldTopName(),true) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified topology: %s\n",
                (const char*)Options.GetArgOldTopName());
        return(false);
    }

    Mask.AssignTopology(&Topology);

    // load optional coordinates
    if( Options.IsOptCrdNameSet() == true ) {
        Coordinates.AssignTopology(&Topology);
        if( Coordinates.Load(Options.GetOptCrdName()) == false ) {
            fprintf(stderr,">>> ERROR: Unable to load specified coordinates: %s\n",
                    (const char*)Options.GetOptCrdName());
            return(false);
        }
        Mask.AssignCoordinates(&Coordinates);
    }

    // set mask
    bool result;
    if( Options.GetOptMaskFile() == true ) {
        result = Mask.SetMaskFromFile(Options.GetArgMaskSpec());
    } else {
        result = Mask.SetMask(Options.GetArgMaskSpec());
    }

    if( result == false ) {
        fprintf(stderr,">>> ERROR: Unable to set specified mask!\n");
        return(false);
    }

    // cut topology
    CAmberSubTopology sub_topology;

    if( sub_topology.InitSubTopology(&Mask,
                                     Options.GetOptCopyBox(),
                                     Options.GetOptIgnoreErrors(),
                                     Options.GetOptVerbose()) == false ) {
        fprintf(stderr,">>> ERROR: Unable to cut topology!\n");
        return(false);
    }

    if( Options.GetOptVer6() == true ) {
        // save topology
        if( sub_topology.Save(Options.GetArgNewTopName(),AMBER_VERSION_6,true) == false ) {
            fprintf(stderr,">>> ERROR: Unable to save specified topology: %s\n",
                    (const char*)Options.GetArgNewTopName());
            return(false);
        }
    } else {
        // save topology
        if( sub_topology.Save(Options.GetArgNewTopName(),AMBER_VERSION_7,true) == false ) {
            fprintf(stderr,">>> ERROR: Unable to save specified topology: %s\n",
                    (const char*)Options.GetArgNewTopName());
            return(false);
        }
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCut::Finalize(void)
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

