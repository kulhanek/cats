#ifndef TopCrd2VMDBoxOptionsH
#define TopCrd2VMDBoxOptionsH
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

class CTopCrd2VMDBoxOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CTopCrd2VMDBoxOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "topcrd2vmdbox"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Print the box information from AMBER topology that can be visualized by vmd."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,TopologyName)
    CSO_ARG(CSmallString,CrdName)
    CSO_ARG(CSmallString,OutName)
    // options ------------------------------
    CSO_OPT(int,MolID)
    CSO_OPT(int,KX)
    CSO_OPT(int,KY)
    CSO_OPT(int,KZ)
    CSO_OPT(bool,Familiar)
    CSO_OPT(bool,Origin)
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
                "PARM",                           /* parametr name */
                "topology file name")   /* argument description */
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                CrdName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "CRD",                           /* parametr name */
                "input coordinates in AMBER coordinate format")   /* argument description */
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                OutName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "OUT",                           /* parametr name */
                "output file with box for VMD")   /* argument description */

// description of options -----------------------------------------------------
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                MolID,                        /* option name */
                0,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "molid",                      /* long option name */
                "INT",                           /* parametr name */
                "molecule id for VMD")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                KX,                        /* option name */
                0,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "kx",                      /* long option name */
                "INT",                           /* parametr name */
                "box position in x direction")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                KY,                        /* option name */
                0,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "ky",                      /* long option name */
                "INT",                           /* parametr name */
                "box position in y direction")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                KZ,                        /* option name */
                0,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "kz",                      /* long option name */
                "INT",                           /* parametr name */
                "box position in z direction")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Origin,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "origin",                      /* long option name */
                NULL,                           /* parametr name */
                "the center of box will be moved to the origin")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Familiar,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "familiar",                      /* long option name */
                NULL,                           /* parametr name */
                "print familiar shape of box")   /* option description */
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
