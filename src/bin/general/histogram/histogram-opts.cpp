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

#include "histogram-opts.hpp"
#include <FormatSpec.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CHistogramOpts::CHistogramOpts(void)
{
    SetShowUsage(false);
}

//------------------------------------------------------------------------------

int CHistogramOpts::CheckOptions(void)
{
    // check if options are in the proper ranges
    // CSO_OPT(int,SkipLines)
    // CSO_OPT(int,AnalLines)
    // CSO_OPT(int,PadLines)
    // CSO_OPT(int,ColumnIndex)
    // CSO_OPT(int,NBins)

    if( GetOptSkipLines() < 0 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: number of skipped lines has to be grater or equal to zero, but %d specified\n",
                (char*)GetProgramName(),GetOptSkipLines());
        IsError = true;
    }
    if( GetOptAnalLines() < -1 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: number of analysed lines has to be grater or equal to minus one, but %d specified\n",
                (char*)GetProgramName(),GetOptAnalLines());
        IsError = true;
    }
    if( GetOptPadLines() < 0 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: number of padding lines has to be grater or equal to zero, but %d specified\n",
                (char*)GetProgramName(),GetOptPadLines());
        IsError = true;
    }
    if( GetOptColumnIndex() <= 0 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: column index has to be grater than zero, but %d specified\n",
                (char*)GetProgramName(),GetOptColumnIndex());
        IsError = true;
    }

    if( GetOptNBins() <= 0 ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: number of bins has to grater than zero, but %d specified\n",
                (char*)GetProgramName(),GetOptNBins());
        IsError = true;
    }

    // CSO_OPT(double,LeftValue)
    // CSO_OPT(double,RightValue)
    // run-time check

    CFormatSpec fspec;

    // CSO_OPT(CSmallString,IXFormat)
    // CSO_OPT(CSmallString,ONFormat)
    // CSO_OPT(CSmallString,ORFormat)
    fspec.SetFormat(GetOptIXFormat());
    if( fspec.IsFormatValid(EFT_DOUBLE) == false ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: illegal format specifier '%s' for X values\n",
                (const char*)GetProgramName(),(const char*)GetOptIXFormat());
        IsError = true;
    }
    fspec.SetFormat(GetOptONFormat());
    if( fspec.IsFormatValid(EFT_INTEGER) == false ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: illegal format specifier '%s' for N(X) values\n",
                (const char*)GetProgramName(),(const char*)GetOptONFormat());
        IsError = true;
    }
    fspec.SetFormat(GetOptORFormat());
    if( fspec.IsFormatValid(EFT_DOUBLE) == false ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: illegal format specifier '%s' for r(X) values\n",
                (const char*)GetProgramName(),(const char*)GetOptIXFormat());
        IsError = true;
    }

    if( IsError == true ) return(SO_OPTS_ERROR);

    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

int CHistogramOpts::FinalizeOptions(void)
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

int CHistogramOpts::CheckArguments(void)
{
    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
