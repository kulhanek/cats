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

#include "d2u-opts.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

Cd2uOpts::Cd2uOpts(void)
{
    SetShowUsage(false);
    SetAllowProgArgs(true);
}

//------------------------------------------------------------------------------

int Cd2uOpts::FinalizeOptions(void)
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

int Cd2uOpts::CheckArguments(void)
{
    if( GetNumberOfProgArgs() <= 0 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: at least one argument has to be specified\n", (char*)GetProgramName());
        IsError = true;
        return(SO_OPTS_ERROR);
    }

    bool std_stream = false;
    for(int i=0; i < GetNumberOfProgArgs(); i++) {
        std_stream |= GetProgArg(i) == "-";
    }

    if( std_stream && ( GetNumberOfProgArgs() > 1 ) ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: only one argument can be provided for conversion from input to output standard stream\n", (char*)GetProgramName());
        IsError = true;
        return(SO_OPTS_ERROR);
    }

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
