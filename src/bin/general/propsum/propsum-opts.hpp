#ifndef BM_PROPSUM_OPTS_H
#define BM_PROPSUM_OPTS_H
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

#include <SimpleOptions.hpp>
#include <CATsMainHeader.hpp>

//------------------------------------------------------------------------------

class CPropSumOpts : public CSimpleOptions {
public:
    // constructor - tune option setup
    CPropSumOpts(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "propsum"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Calculate total sum of value X, its average <X>, root-mean-square deviation of X, and standard deviation of <X>."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,Input)
    // options ------------------------------
    CSO_OPT(int,SkipLines)
    CSO_OPT(int,AnalLines)
    CSO_OPT(int,PadLines)
    CSO_OPT(int,ColumnIndex)
    CSO_OPT(bool,Intermediates)
    CSO_OPT(bool,NoHeader)
    CSO_OPT(CSmallString,INFormat)
    CSO_OPT(CSmallString,IXFormat)
    CSO_OPT(CSmallString,OSumFormat)
    CSO_OPT(CSmallString,OAveFormat)
    CSO_OPT(CSmallString,ORMSDFormat)
    CSO_OPT(CSmallString,OSigmaFormat)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of arguments ---------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                Input,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "input",                        /* parametr name */
                "input data file or - for data taken from standard input")   /* argument description */
// description of options ---------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                SkipLines,                        /* option name */
                0,                          /* default value */
                false,                          /* is option mandatory */
                's',                           /* short option name */
                "skip",                      /* long option name */
                "LINES",                           /* parametr name */
                "number of lines skipped from the beginning of input file")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                AnalLines,                        /* option name */
                -1,                          /* default value */
                false,                          /* is option mandatory */
                'a',                           /* short option name */
                "anal",                      /* long option name */
                "LINES",                           /* parametr name */
                "number of LINES from input file to be analyzed")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                PadLines,                        /* option name */
                0,                          /* default value */
                false,                          /* is option mandatory */
                'p',                           /* short option name */
                "pad",                      /* long option name */
                "LINES",                           /* parametr name */
                "number of padding LINES between used records from input file")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                ColumnIndex,                        /* option name */
                1,                          /* default value */
                false,                          /* is option mandatory */
                'c',                           /* short option name */
                "column",                      /* long option name */
                "NUM",                           /* parametr name */
                "NUM is the index of column to be analyzed")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Intermediates,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "intermediates",                      /* long option name */
                NULL,                           /* parametr name */
                "print running intermediates")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                NoHeader,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "noheader",                      /* long option name */
                NULL,                           /* parametr name */
                "do not print header for running intermediates")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                INFormat,                        /* option name */
                "%15d",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "fn",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of the number of X values")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                IXFormat,                        /* option name */
                "%15.7e",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "fx",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of echoed X values")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                OSumFormat,                        /* option name */
                "%14.6e",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "fsum",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of sum(X) values")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                OAveFormat,                        /* option name */
                "%15.7e",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "fave",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of ave(X) values")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                ORMSDFormat,                        /* option name */
                "%14.6e",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "frmsd",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of RMSD(X) values")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                OSigmaFormat,                        /* option name */
                "%14.6e",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "fsigma",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of sigma(<X>) values")   /* option description */

    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Verbose,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'v',                           /* short option name */
                "verbose",                      /* long option name */
                NULL,                           /* parametr name */
                "increase output verbosity")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Version,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "version",                      /* long option name */
                NULL,                           /* parametr name */
                "output version information and exit")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Help,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'h',                           /* short option name */
                "help",                      /* long option name */
                NULL,                           /* parametr name */
                "display this help and exit")   /* option description */
    CSO_MAP_END

// final operation with options ------------------------------------------------
private:
    virtual int FinalizeOptions(void);
    virtual int CheckOptions(void);
};

//------------------------------------------------------------------------------

#endif
