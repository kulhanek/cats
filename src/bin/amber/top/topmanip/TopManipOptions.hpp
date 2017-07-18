#ifndef TopManipOptionsH
#define TopManipOptionsH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2015 Petr Kulhanek, kulhanek@chemi.muni.cz
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

class CTopManipOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CTopManipOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "topmanip"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Manipulate with topology information."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,TopologyName)
    CSO_ARG(CSmallString,OutputName)
    // options ------------------------------
    CSO_OPT(CSmallString,MaskSpec)
    CSO_OPT(CSmallString,MaskFile)
    CSO_OPT(bool,GetCharges)
    CSO_OPT(CSmallString,SetCharges)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of arguments ---------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                TopologyName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "TOP",                           /* parametr name */
                "topology name. If - is provided then topology is read from standard input stream.")   /* argument description */
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                OutputName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "OUT",                           /* parametr name */
                "output file name. If - is provided then output is written to standard output stream.")   /* argument description */

// description of options -----------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                MaskSpec,                        /* option name */
                "",                          /* default value */
                false,                          /* is option mandatory */
                'm',                           /* short option name */
                "mask",                      /* long option name */
                "MASK",                           /* parametr name */
                "only atoms selected according to MASK will be converted. Mutually exclusive with 'maskfile' option.")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                MaskFile,                        /* option name */
                NULL,                          /* default value */
                false,                          /* is option mandatory */
                'f',                           /* short option name */
                "maskfile",                      /* long option name */
                "MASKFILE",                           /* parametr name */
                "only atoms selected according to the mask will be converted. The mask specification is read from the first line of the file of name MASKFILE. This option is suitable for the specification of very long masks. Mutually exclusive with 'mask' option.")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                GetCharges,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "getcharges",                      /* long option name */
                NULL,                           /* parametr name */
                "get topology charges")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                SetCharges,                        /* option name */
                NULL,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "setcharges",                      /* long option name */
                "FILE",                           /* parametr name */
                "set charges from FILE")   /* option description */
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
