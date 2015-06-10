#ifndef TopCrd2CpmdOptionsH
#define TopCrd2CpmdOptionsH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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

class CTopCrd2CpmdOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CTopCrd2CpmdOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "topcrd2cpmd"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "<red>topcrd2cpmd</red> converts AMBER topology and coordinates to coordinates in other format."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    CATS_VERSION
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,TopologyName)
    CSO_ARG(CSmallString,CrdName)
    CSO_ARG(CSmallString,MaskSpec)
    CSO_ARG(CSmallString,PrefixName)
    // options ------------------------------
    CSO_OPT(int,QMCharge)
    CSO_OPT(CSmallString,Functional)
    CSO_OPT(CSmallString,PSPFormat)
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
                "input topology")   /* argument description */
    // -------------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                CrdName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "CRD",                           /* parametr name */
                "input coordinates")   /* argument description */
    // -------------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                MaskSpec,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "MASK",                           /* parametr name */
                "QM zone specification")   /* argument description */
    // -------------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                PrefixName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "PREFIX",                           /* parametr name */
                "prefix name")   /* argument description */
// description of options -----------------------------------------------------
    CSO_MAP_OPT(int,              /* option type */
                QMCharge,                        /* option name */
                0,                          /* default value */
                true,                          /* is option mandatory */
                'c',                           /* short option name */
                "qmcharge",                      /* long option name */
                "NUMBER",                           /* parametr name */
                "QM zone total charge")   /* option description */
    // -------------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                   /* option type */
                Functional,                     /* option name */
                "BLYP",                         /* default value */
                false,                           /* is option mandatory */
                'f',                            /* short option name */
                "functional",                     /* long option name */
                "FUNCTIONAL",                       /* parametr name */
                "DFT functional for QM zone")         /* option description */
    // -------------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                   /* option type */
                PSPFormat,                     /* option name */
                "%1%_MT_%2%.psp KLEINMAN-BYLANDER",                         /* default value */
                false,                           /* is option mandatory */
                'p',                            /* short option name */
                "pspformat",                     /* long option name */
                "FORMAT",                       /* parametr name */
                "pseudopotential specification: %1 - symbol, %2 - functional")         /* option description */
    // -------------------------------------------------------------------------
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
