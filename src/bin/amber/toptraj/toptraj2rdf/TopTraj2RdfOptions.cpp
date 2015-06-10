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

#include "TopTraj2RdfOptions.hpp"
#include <ErrorSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopTraj2RdfOptions::CTopTraj2RdfOptions(void)
{
    SetShowMiniUsage(true);
    SetAllowProgArgs(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


int CTopTraj2RdfOptions::CheckOptions(void)
{
    if( GetOptStartValue() < 0 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: start value has to be greater than or equal to zero, but %f specified\n",
                (char*)GetProgramName(),GetOptStartValue());
        IsError = true;
    }

    if( GetOptStopValue() < GetOptStartValue() ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: stop value %f has to be greater than start value %f\n",
                (char*)GetProgramName(),GetOptStopValue(),GetOptStartValue());
        IsError = true;
    }

    if( GetOptNBins() <= 0 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: number of bins has to be greater than zero, but %d specified\n",
                (char*)GetProgramName(),GetOptNBins());
        IsError = true;
    }

    if( GetOptFrequency() <= 0 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: frequency has to be positive number, but %d specified\n",
                (char*)GetProgramName(),GetOptFrequency());
        IsError = true;
    }

    if( IsError == true ) return(SO_OPTS_ERROR);

    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

int CTopTraj2RdfOptions::FinalizeOptions(void)
{
    bool ret_opt = false;

    if( GetOptHelp() == true ) {
        PrintUsage();
        ret_opt = true;
    }

    if( GetOptVersion() == true ) {
        PrintVersion();
        ret_opt = true;
    }

    if( ret_opt == true ) {
        printf("\n");
        return(SO_EXIT);
    }

    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

int CTopTraj2RdfOptions::CheckArguments(void)
{
    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
