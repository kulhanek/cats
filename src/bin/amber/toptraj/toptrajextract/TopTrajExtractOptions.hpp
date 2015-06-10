#ifndef TopTrajExtractOptionsH
#define TopTrajExtractOptionsH
// =============================================================================
// CATS - Conversion and Analysis Tools
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

class CTopTrajExtractOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CTopTrajExtractOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "toptrajextract"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "<red>toptrajextract</red> extracts AMBER coordinates from AMBER trajectory file."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    CATS_VERSION
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,TopName)
    CSO_ARG(CSmallString,TrajName)
    CSO_ARG(CSmallString,CrdName)
    // options ------------------------------
    CSO_OPT(CSmallString,SnapshotNumber)
    CSO_OPT(bool,CopyBox)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of arguments ---------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                TopName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "PARM",                           /* parametr name */
                "topology file name")   /* argument description */
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                TrajName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "CRD",                           /* parametr name */
                "trajectory file name")   /* argument description */
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                CrdName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "OUT",                           /* parametr name */
                "output AMBER coordinates file name")   /* argument description */

// description of options -----------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                SnapshotNumber,                        /* option name */
                "first",                          /* default value */
                false,                          /* is option mandatory */
                's',                           /* short option name */
                "snapshot",                      /* long option name */
                "NUMBER",                           /* parametr name */
                "number of extracted snapshot or first/last keys for first/last snapshot")   /* option description */
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
