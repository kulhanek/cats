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

#include "blur-opts.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBlurOpts::CBlurOpts(void)
{
    SetShowUsage(false);
}

//------------------------------------------------------------------------------

int CBlurOpts::CheckOptions(void)
{
    if( (GetOptFilterType() != "low-pass") && (GetOptFilterType() != "gauss") ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: unsupported filter type %s\n",
                (char*)GetProgramName(),(const char*)GetOptFilterType());
        IsError = true;
    }

    if( IsError == true ) return(SO_OPTS_ERROR);

    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

int CBlurOpts::FinalizeOptions(void)
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

int CBlurOpts::CheckArguments(void)
{
    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
