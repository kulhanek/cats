#ifndef MolRmsdOptionsH
#define MolRmsdOptionsH
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

#include <SimpleOptions.hpp>
#include <CATsMainHeader.hpp>

//------------------------------------------------------------------------------

class CMolRmsdOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CMolRmsdOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "xyzfit"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Calculate RMSD between two molecules."
    CSO_PROG_DESC_END

    CSO_PROG_ARGS_SHORT_DESC_BEGIN
    "ref str [out]"
    CSO_PROG_ARGS_SHORT_DESC_END

    CSO_PROG_ARGS_LONG_DESC_BEGIN
    "ref - name of reference molecule file or - for input from standard input stream\n"
    "str - name of super-imposed molecule file or - for input from standard input stream\n"
    "out - super-imposed structure\n"
    CSO_PROG_ARGS_LONG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // options ------------------------------
    CSO_OPT(CSmallString,RefFormat)
    CSO_OPT(CSmallString,StrFormat)
    CSO_OPT(CSmallString,OutFormat)
    CSO_OPT(CSmallString,Pattern)
    CSO_OPT(bool,NoFit)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of options -----------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                RefFormat,                        /* option name */
                "auto",                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "refformat",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "format of reference molecule")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                StrFormat,                        /* option name */
                "auto",                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "strformat",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "format of superimposed molecule")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                Pattern,                        /* option name */
                "identity",                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "pattern",                      /* long option name */
                NULL,                           /* parametr name */
                "atom map between reference and superimposed molecule [num1:num2,..] or identity")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                NoFit,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "nofit",                      /* long option name */
                NULL,                           /* parametr name */
                "do not fit molecules")   /* option description */
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
