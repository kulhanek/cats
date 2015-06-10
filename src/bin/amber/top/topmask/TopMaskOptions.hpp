#ifndef TopMaskOptionsH
#define TopMaskOptionsH
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

class CTopMaskOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CTopMaskOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "topmask"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Print the names of atoms or residues selected by the application of MASK. Current implementation fully supports ambmask syntax (see AMBER manual)."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    CATS_VERSION
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,TopologyName)
    CSO_ARG(CSmallString,MaskSpec)
    // options ------------------------------
    CSO_OPT(CSmallString,CrdName)
    CSO_OPT(bool,MaskFile)
    CSO_OPT(bool,ResidueMask)
    CSO_OPT(CSmallString,OutputFormat)
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
                "topology file name. If - is provided then topology is read from standard input stream.")   /* argument description */
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                MaskSpec,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "MASK",                           /* parametr name */
                "mask specification")   /* argument description */
// description of options -----------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                CrdName,                        /* option name */
                "",                          /* default value */
                false,                          /* is option mandatory */
                'c',                           /* short option name */
                "coords",                      /* long option name */
                "CRD",                           /* parametr name */
                "optional coordinates in AMBER coordinate format, coordinates are mandatory for masks containing distance operators (<: >: <@ >@)")   /* option description */
    CSO_MAP_OPT(bool,                           /* option type */
                MaskFile,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'f',                           /* short option name */
                "maskfile",                      /* long option name */
                NULL,                           /* parametr name */
                "read mask specification from the first line of the file of name MASK. This option is suitable for the specification of very long masks.")   /* option description */
    CSO_MAP_OPT(bool,                           /* option type */
                ResidueMask,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'r',                           /* short option name */
                "residues",                      /* long option name */
                NULL,                           /* parametr name */
                "perform selection on the level of residues. The mask specification cannot contains atom selectors (@ <@ >@ @%).")   /* option description */
    CSO_MAP_OPT(CSmallString,                           /* option type */
                OutputFormat,                        /* option name */
                "cats",                          /* default value */
                false,                          /* is option mandatory */
                'o',                           /* short option name */
                "output",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "specify output format. Default format 'cats' prints atoms or residues as the list sorted into three colums. Format 'list' prints the same information as 'cats' but only to a single colum without any header. Format 'number' prints only the number of selected atoms or residues. Format 'amber' provides the same output as ambmask. Format 'bmask' write bitmap mask.")   /* option description */
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
