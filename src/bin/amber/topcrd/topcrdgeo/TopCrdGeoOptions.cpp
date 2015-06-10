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

#include "TopCrdGeoOptions.hpp"
#include <ErrorSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopCrdGeoOptions::CTopCrdGeoOptions(void)
{
    SetShowMiniUsage(true);
    SetAllowProgArgs(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const CSmallString& CTopCrdGeoOptions::GetArgTopologyName(void)
{
    return(GetProgArg(0));
}

//------------------------------------------------------------------------------

const CSmallString& CTopCrdGeoOptions::GetArgCrdName(void)
{
    return(GetProgArg(1));
}

//------------------------------------------------------------------------------

const CSmallString& CTopCrdGeoOptions::GetArgCommand(void)
{
    return(GetProgArg(2));
}

//------------------------------------------------------------------------------

const CSmallString& CTopCrdGeoOptions::GetCommandArg(int pos)
{
    static CSmallString zero;
    if( pos+3 < 3 ) return(zero);
    return(GetProgArg(pos+3));
}

//------------------------------------------------------------------------------

int CTopCrdGeoOptions::GetNumOfCommandArgs(void)
{
    if( GetNumberOfProgArgs() - 3 <= 0 ) return(0);
    return(GetNumberOfProgArgs() - 3);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


int CTopCrdGeoOptions::CheckOptions(void)
{
    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

int CTopCrdGeoOptions::FinalizeOptions(void)
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

int CTopCrdGeoOptions::CheckArguments(void)
{
    if( GetNumberOfProgArgs() < 3 ) {
        if( IsVerbose() ) {
            if( IsError == false ) fprintf(stderr,"\n");
            fprintf(stderr,"%s: incorrect number of parameters\n", (const char*)GetProgramName());
            IsError = true;
        }
        return(SO_OPTS_ERROR);
    }

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
