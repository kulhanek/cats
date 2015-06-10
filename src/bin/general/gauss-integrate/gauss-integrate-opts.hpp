#ifndef GAUSS_INTEGRATE_OPTS_H
#define GAUSS_INTEGRATE_OPTS_H
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

class CGaussIntegrateOpts : public CSimpleOptions {
public:
    // constructor - tune option setup
    CGaussIntegrateOpts(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "gauss-integrate"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Numerically integrate data using Gaussian quadrature."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    CATS_VERSION
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(int,NPoints)
    CSO_ARG(CSmallString,Input)
    CSO_ARG(CSmallString,Output)
    // options ------------------------------
    CSO_OPT(double,StartValue)
    CSO_OPT(double,StopValue)
    CSO_OPT(double,IntOffset)
    CSO_OPT(CSmallString,Polynom)
    CSO_OPT(bool,ShowProgress)
    CSO_OPT(bool,NoSigma)
    CSO_OPT(bool,NoHeader)
    CSO_OPT(CSmallString,IXFormat)
    CSO_OPT(CSmallString,IYFormat)
    CSO_OPT(CSmallString,ISFormat)
    CSO_OPT(CSmallString,OIFormat)
    CSO_OPT(CSmallString,OEFormat)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of arguments ---------------------------------------------------
    CSO_MAP_ARG(int,                   /* argument type */
                NPoints,                          /* argument name */
                0,                           /* default value */
                true,                           /* is argument mandatory */
                "N",                        /* parametr name */
                "number of points used in Gaussian quadrature")   /* argument description */
    //----------------------------------------------------------------------
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
                "output result file or - for results printed to standard output")   /* argument description */
// description of options ---------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                StartValue,                        /* option name */
                0.0,                          /* default value */
                false,                          /* is option mandatory */
                'l',                           /* short option name */
                "start",                      /* long option name */
                "REAL",                           /* parametr name */
                "left boundary of integration interval")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(double,                           /* option type */
                StopValue,                        /* option name */
                1.0,                          /* default value */
                false,                          /* is option mandatory */
                'r',                           /* short option name */
                "stop",                      /* long option name */
                "REAL",                           /* parametr name */
                "right boundary of integration interval")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                IntOffset,                        /* option name */
                0.0,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "offset",                      /* long option name */
                "REAL",                           /* parametr name */
                "specify integration constant")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                Polynom,                        /* option name */
                "legendre",                          /* default value */
                false,                          /* is option mandatory */
                'p',                           /* short option name */
                "polynom",                      /* long option name */
                "NAME",                           /* parametr name */
                "name of orthogonal polynom: legendre")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                ShowProgress,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "progress",                      /* long option name */
                NULL,                           /* parametr name */
                "show integration progress")   /* option description */
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
                NoSigma,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "nosigma",                      /* long option name */
                NULL,                           /* parametr name */
                "standard deviation of derivative is not present in input data")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                IXFormat,                        /* option name */
                "%15.7e",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "fx",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of echoed x values")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                IYFormat,                        /* option name */
                "%15.7e",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "fy",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of echoed derivative values")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                ISFormat,                        /* option name */
                "%14.6e",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "fs",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of echoed sigma values")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                OIFormat,                        /* option name */
                "%15.7e",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "fi",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of integrated values")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                OEFormat,                        /* option name */
                "%14.6e",                          /* default value */
                false,                          /* is option mandatory */
                '\0',                           /* short option name */
                "fe",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output FORMAT of integrated error values")   /* option description */
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
    virtual int CheckArguments(void);
};

//------------------------------------------------------------------------------

#endif
