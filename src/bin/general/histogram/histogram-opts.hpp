#ifndef HistogramOPTSH
#define HistogramOPTSH
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

class CHistogramOpts : public CSimpleOptions {
public:
    // constructor - tune option setup
    CHistogramOpts(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "histogram"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Perform histogram analysis."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    CATS_VERSION
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,Input)
    CSO_ARG(CSmallString,Output)
    // options ------------------------------
    CSO_OPT(int,SkipLines)
    CSO_OPT(int,AnalLines)
    CSO_OPT(int,PadLines)
    CSO_OPT(int,ColumnIndex)
    CSO_OPT(double,StartValue)
    CSO_OPT(double,StopValue)
    CSO_OPT(int,NBins)
    CSO_OPT(bool,Normalize)
    CSO_OPT(bool,NoHeader)
    CSO_OPT(CSmallString,IXFormat)
    CSO_OPT(CSmallString,ONFormat)
    CSO_OPT(CSmallString,ORFormat)
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
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                Output,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "output",                        /* parametr name */
                "output result file or - for result printed to standard output")   /* argument description */
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
    CSO_MAP_OPT(double,                           /* option type */
                StartValue,                        /* option name */
                0,                          /* default value */
                false,                          /* is option mandatory */
                'l',                           /* short option name */
                "start",                      /* long option name */
                "REAL",                           /* parametr name */
                "REAL is the left value of the histogram interval")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                StopValue,                        /* option name */
                0,                          /* default value */
                false,                          /* is option mandatory */
                'r',                           /* short option name */
                "stop",                      /* long option name */
                "REAL",                           /* parametr name */
                "REAL is the right value of the histogram interval")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                NBins,                        /* option name */
                40,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "nbins",                      /* long option name */
                "NUM",                           /* parametr name */
                "NUM is the number of histogram bins")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Normalize,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'n',                           /* short option name */
                "normalize",                      /* long option name */
                NULL,                           /* parametr name */
                "normalize histogram")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                IXFormat,                        /* option name */
                "%15.7e",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "fx",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of X values")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                ONFormat,                        /* option name */
                "%15d",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "fn",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of N(X) values")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                ORFormat,                        /* option name */
                "%14.6e",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "fr",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of r(X) values")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                NoHeader,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "noheader",                      /* long option name */
                NULL,                           /* parametr name */
                "do not print header to output")   /* option description */
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
    virtual int CheckOptions(void);
    virtual int FinalizeOptions(void);
    virtual int CheckArguments(void);
};

//------------------------------------------------------------------------------

#endif
