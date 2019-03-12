#ifndef TopRMLAOptionsH
#define TopRMLAOptionsH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2018 Petr Kulhanek, kulhanek@chemi.muni.cz
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

class CTopRMLAOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CTopRMLAOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "toprmla"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Remove incorrect angles from the topology. The incorrect angle has deviation between equilibrium "
    "and real angle value larger than specified treshold. Typicaly these are linear angles around the AB6 centers. "
    " In addition, the program removes also corresponding dihedral angles."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,OldTopName)
    CSO_ARG(CSmallString,CrdName)
    CSO_ARG(CSmallString,NewTopName)
    // options ------------------------------
    CSO_OPT(CSmallString,MaskSpec)
    CSO_OPT(double,MaxDeviation)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of arguments ---------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                OldTopName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "OLD",                           /* parametr name */
                "old topology name. If - is provided then topology is read from standard input stream.")   /* argument description */
    CSO_MAP_ARG(CSmallString,                           /* option type */
                CrdName,                        /* option name */
                NULL,                          /* default value */
                true,                          /* is option mandatory */
                "CRD",                           /* parametr name */
                "coordinates (in AMBER restart format) defining real angle values")   /* option description */
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                NewTopName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "NEW",                           /* parametr name */
                "new topology name. If - is provided then topology is written to standard output stream.")   /* argument description */
// description of options -----------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                MaxDeviation,                        /* option name */
                20.0,                          /* default value */
                false,                          /* is option mandatory */
                't',                           /* short option name */
                "treshold",                      /* long option name */
                "REAL",                           /* parametr name */
                "if deviation between topology and real value of the angle is higher than this treshold, the angle is removed from the topology. The value is in degrees.")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                MaskSpec,                        /* option name */
                "",                          /* default value */
                false,                          /* is option mandatory */
                'm',                           /* short option name */
                "mask",                      /* long option name */
                "MASK",                           /* parametr name */
                "only angles with central atoms selected according to MASK will be filtered otherwise all angles are evaluated.")   /* option description */
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
