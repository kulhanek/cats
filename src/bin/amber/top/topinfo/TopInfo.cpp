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
#include "TopInfo.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopInfo::CTopInfo(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopInfo::Init(int argc,char* argv[])
{
    // encode program options
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    // print header --------------------------------------------------------------
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    printf("\n");
    printf("# ==============================================================================\n");
    printf("# topinfo (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
    printf("# ==============================================================================\n");
    printf("#\n");
    if( Options.GetArgTopologyName() == "-" ) {
        printf("# Topology name: -stdin- (standard input)\n");
    } else {
        printf("# Topology name: %s\n",(const char*)Options.GetArgTopologyName());
    }
    printf("# ------------------------------------------------------------------------------\n");

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopInfo::Run(void)
{
    CAmberTopology topology;

    if( topology.Load(Options.GetArgTopologyName(),true) == false ) {
        fprintf(stderr,"\n");
        fprintf(stderr,">>> ERROR: Unable to load specified topology: %s\n",
                (const char*)Options.GetArgTopologyName());
        return(false);
    }

    topology.PrintInfo();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopInfo::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    fprintf(stdout,"\n");
    fprintf(stdout,"# ==============================================================================\n");
    fprintf(stdout,"# %s terminated at %s\n",(const char*)Options.GetProgramName(),(const char*)dt.GetSDateAndTime());
    fprintf(stdout,"# ==============================================================================\n");

    if( Options.GetOptVerbose() || ErrorSystem.IsError() ){
        ErrorSystem.PrintErrors(stderr);
    }

    fprintf(stdout,"\n");

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

