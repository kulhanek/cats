#ifndef OptionsH
#define OptionsH
// =============================================================================
// This file is part of OBSymChg.
//    Copyright (C) 2021 Petr Kulhanek, kulhanek@chemi.muni.cz
//
// OBSymChg is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// OBSymChg is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OBSymChg. If not, see <http://www.gnu.org/licenses/>.
// =============================================================================

#include <SimpleOptions.hpp>
#include <CATsMainHeader.hpp>

//------------------------------------------------------------------------------

class COptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    COptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "obsymchg"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Symmetrize charges accroding to OpenBabel symmetry classes."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,MolName)
    CSO_ARG(CSmallString,QIn)
    CSO_ARG(CSmallString,QOut)    
    // options ------------------------------
    CSO_OPT(CSmallString,MolFormat)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of arguments ---------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                MolName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "MOL",                           /* parametr name */
                "input molecule")   /* argument description */
    //----------------------------------------------------------------------    
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                QIn,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "QIN",                           /* parametr name */
                "input charges")   /* argument description */
    //----------------------------------------------------------------------    
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                QOut,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "QOUT",                           /* parametr name */
                "output charges")   /* argument description */    
// description of options -----------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                MolFormat,                        /* option name */
                "auto",                          /* default value */
                false,                          /* is option mandatory */
                0,                           /* short option name */
                "molformat",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "openbabel format of molecule")   /* option description */
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
