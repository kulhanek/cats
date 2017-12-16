// =============================================================================
// VScreen - Virtual Screening Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "MolSplitOptions.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMolSplitOptions::CMolSplitOptions(void)
{
    SetShowMiniUsage(true);
}

//------------------------------------------------------------------------------

int CMolSplitOptions::CheckOptions(void)
{
    if(GetOptUnisID() != "title") {
        if( GetOptUnisID().GetLength() != 4 ){
            if(IsError == false) fprintf(stderr,"\n");
            fprintf(stderr,"%s: UnisID must be either 'title' or 4 character long identifier but '%s' is provided\n",(const char*)GetProgramName(),(const char*)GetOptUnisID());
            IsError = true;
        }
    }
    if(IsError == true) return(SO_OPTS_ERROR);
    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

int CMolSplitOptions::FinalizeOptions(void)
{
    bool ret_opt = false;

    if(GetOptHelp() == true) {
        PrintUsage();
        ret_opt = true;
    }

    if(GetOptVersion() == true) {
        PrintVersion();
        ret_opt = true;
    }

    if(ret_opt == true) {
        printf("\n");
        return(SO_EXIT);
    }

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
