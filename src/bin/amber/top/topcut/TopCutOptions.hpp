#ifndef TopCutOptionsH
#define TopCutOptionsH
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

class CTopCutOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CTopCutOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "topcut"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Cut AMBER topology and saves it as new one. Artifical topologies with open valences could be save too but use them only for visualisation purposes not for energy evaluation."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,OldTopName)
    CSO_ARG(CSmallString,MaskSpec)
    CSO_ARG(CSmallString,NewTopName)
    // options ------------------------------
    CSO_OPT(CSmallString,CrdName)
    CSO_OPT(bool,MaskFile)
    CSO_OPT(bool,CopyBox)
    CSO_OPT(bool,IgnoreErrors)
    CSO_OPT(bool,Ver6)
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
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                MaskSpec,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "MASK",                           /* parametr name */
                "mask specification")   /* argument description */
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                NewTopName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "NEW",                           /* parametr name */
                "new topology name. If - is provided then topology is written to standard output stream.")   /* argument description */
// description of options -----------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                CrdName,                        /* option name */
                "",                          /* default value */
                false,                          /* is option mandatory */
                'c',                           /* short option name */
                "coords",                      /* long option name */
                "CRD",                           /* parametr name */
                "optional coordinates in AMBER coordinate format, coordinates are mandatory for masks containing distance operators (<: >: <@ >@)")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                MaskFile,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'f',                           /* short option name */
                "maskfile",                      /* long option name */
                NULL,                           /* parametr name */
                "read mask specification from the first line of the file of name MASK. This option is suitable for the specification of very long masks.")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                CopyBox,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "copybox",                      /* long option name */
                NULL,                           /* parametr name */
                "copy old box information to new topology")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                IgnoreErrors,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "ignoreerrors",                      /* long option name */
                NULL,                           /* parametr name */
                "ignore open valences in newly created topology")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Ver6,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "ver6",                      /* long option name */
                NULL,                           /* parametr name */
                "save new topology in version AMBER 6")   /* option description */
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
