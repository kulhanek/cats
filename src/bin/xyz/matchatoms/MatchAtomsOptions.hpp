#ifndef MatchAtomsOptionsH
#define MatchAtomsOptionsH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2023 Petr Kulhanek, kulhanek@chemi.muni.cz
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

class CMatchAtomsOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CMatchAtomsOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "matchatoms"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Match one molecule to the other with atom and residues names."
    CSO_PROG_DESC_END

    CSO_PROG_ARGS_SHORT_DESC_BEGIN
    "ref in out"
    CSO_PROG_ARGS_SHORT_DESC_END

    CSO_PROG_ARGS_LONG_DESC_BEGIN
    "ref - name of reference molecule (template)\n"
    "in  - input structure\n"
    "out - output structure\n"
    CSO_PROG_ARGS_LONG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // options ------------------------------
    CSO_OPT(CSmallString,RefFormat)
    CSO_OPT(CSmallString,InFormat)
    CSO_OPT(CSmallString,OutFormat)
    CSO_OPT(CSmallString,RefH)
    CSO_OPT(CSmallString,InH)
    CSO_OPT(CSmallString,OutH)
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
                "reffmt",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "file format of reference molecule")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                InFormat,                        /* option name */
                "auto",                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "infmt",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "file format of input molecule")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                OutFormat,                        /* option name */
                "auto",                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "outfmt",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "file format of output molecule")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                RefH,                        /* option name */
                "keep",                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "refH",                      /* long option name */
                "MODE",                           /* parametr name */
                "modify hydrogen atoms in the reference structure: keep, add, addpolar, addnonpolar, remove, removepolar, removenonpolar")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                InH,                        /* option name */
                "keep",                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "inH",                      /* long option name */
                "MODE",                           /* parametr name */
                "modify hydrogen atoms in the input structure: keep, add, addpolar, addnonpolar, remove, removepolar, removenonpolar")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                OutH,                        /* option name */
                "keep",                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "outH",                      /* long option name */
                "MODE",                           /* parametr name */
                "modify hydrogen atoms in the output structure: keep, remove, removepolar, removenonpolar")   /* option description */
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
