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

#include "gauss-integrate-opts.hpp"
#include <FormatSpec.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGaussIntegrateOpts::CGaussIntegrateOpts(void)
{
    SetShowUsage(false);
}

//------------------------------------------------------------------------------

int CGaussIntegrateOpts::CheckOptions(void)
{
    // check if options are in the proper ranges
    if( GetOptStartValue() >= GetOptStopValue() ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: left boundary %f has to be smaller than right boundary %f of integration interval\n",
                (const char*)GetProgramName(),GetOptStartValue(),GetOptStopValue());
        IsError = true;
    }
    if( (GetOptPolynom() != "legendre") ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: illegal polynom kind '%s' (Supported polynoms are: legendre.)\n",
                (const char*)GetProgramName(),(const char*)GetOptPolynom());
        IsError = true;
    }

    CFormatSpec fspec;

    // CSO_OPT(CSmallString,IXFormat)
    // CSO_OPT(CSmallString,IYFormat)
    // CSO_OPT(CSmallString,ISFormat)
    // CSO_OPT(CSmallString,OIFormat)
    // CSO_OPT(CSmallString,OEFormat)
    fspec.SetFormat(GetOptIXFormat());
    if( fspec.IsFormatValid(EFT_DOUBLE) == false ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: illegal format specifier '%s' for X values\n",
                (const char*)GetProgramName(),(const char*)GetOptIXFormat());
        IsError = true;
    }
    fspec.SetFormat(GetOptIYFormat());
    if( fspec.IsFormatValid(EFT_DOUBLE) == false ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: illegal format specifier '%s' for Y values\n",
                (const char*)GetProgramName(),(const char*)GetOptIYFormat());
        IsError = true;
    }
    fspec.SetFormat(GetOptISFormat());
    if( fspec.IsFormatValid(EFT_DOUBLE) == false ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: illegal format specifier '%s' for sigma(y) values\n",
                (const char*)GetProgramName(),(const char*)GetOptISFormat());
        IsError = true;
    }
    fspec.SetFormat(GetOptOIFormat());
    if( fspec.IsFormatValid(EFT_DOUBLE) == false ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: illegal format specifier '%s' for integrated values\n",
                (const char*)GetProgramName(),(const char*)GetOptOIFormat());
        IsError = true;
    }
    fspec.SetFormat(GetOptOEFormat());
    if( fspec.IsFormatValid(EFT_DOUBLE) == false ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: illegal format specifier '%s' for integrated error values\n",
                (const char*)GetProgramName(),(const char*)GetOptOEFormat());
        IsError = true;
    }

    if( IsError == true ) return(SO_OPTS_ERROR);

    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

int CGaussIntegrateOpts::FinalizeOptions(void)
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

int CGaussIntegrateOpts::CheckArguments(void)
{
    if( GetArgNPoints() <= 1 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: number of integration points '%d' has to be grater than 2\n",
                (const char*)GetProgramName(),GetArgNPoints());
        IsError = true;
    }

    if( IsError == true ) return(SO_OPTS_ERROR);

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
