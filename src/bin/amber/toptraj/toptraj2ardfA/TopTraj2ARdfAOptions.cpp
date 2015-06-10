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

#include "TopTraj2ARdfAOptions.hpp"
#include <ErrorSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopTraj2ARdfAOptions::CTopTraj2ARdfAOptions(void)
{
    SetShowMiniUsage(true);
    SetAllowProgArgs(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


int CTopTraj2ARdfAOptions::CheckOptions(void)
{
    if( GetOptStartValue1() < 0 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: distance start value has to be greater than or equal to zero, but %f specified\n",
                (char*)GetProgramName(),GetOptStartValue1());
        IsError = true;
    }

    if( GetOptStopValue1() < GetOptStartValue1() ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: distance stop value %f has to be greater than start value %f\n",
                (char*)GetProgramName(),GetOptStopValue1(),GetOptStartValue1());
        IsError = true;
    }

    if( GetOptNBins1() <= 0 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: number of bins for distance has to be greater than zero, but %d specified\n",
                (char*)GetProgramName(),GetOptNBins1());
        IsError = true;
    }

    if( GetOptStartValue2() < 0 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: angle start value has to be greater than or equal to zero, but %f specified\n",
                (char*)GetProgramName(),GetOptStartValue1());
        IsError = true;
    }

    if( GetOptStopValue2() <= GetOptStartValue2() ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: angle stop value %f has to be grater than start value %f\n",
                (char*)GetProgramName(),GetOptStopValue2(),GetOptStartValue2());
        IsError = true;
    }

    if( GetOptNBins2() <= 0 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: number of bins for angle has to grater than zero, but %d specified\n",
                (char*)GetProgramName(),GetOptNBins2());
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

int CTopTraj2ARdfAOptions::FinalizeOptions(void)
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

int CTopTraj2ARdfAOptions::CheckArguments(void)
{
    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
