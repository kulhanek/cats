#ifndef CreatePrjOptionsH
#define CreatePrjOptionsH
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
#include <ChemInfoMainHeader.hpp>

//------------------------------------------------------------------------------

class CCreatePrjOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CCreatePrjOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "vs-project-create"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Create new virtual screening project."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    CHEMINFO_VERSION
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,DatabaseKey)
    CSO_ARG(CSmallString,ProjectName)
    // options ------------------------------
    CSO_OPT(CSmallString,Description)
    CSO_OPT(CSmallString,Operator)
    CSO_OPT(CSmallString,DynamicProperties)
    CSO_OPT(int,NumOfResults)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of arguments ---------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                DatabaseKey,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "dbkey",                        /* parametr name */
                "database key file")   /* argument description */
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                ProjectName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "project",                        /* parametr name */
                "project name")   /* argument description */
// description of options -----------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                Description,                        /* option name */
                NULL,                          /* default value */
                false,                          /* is option mandatory */
                'd',                           /* short option name */
                "description",                      /* long option name */
                "TEXT",                           /* parametr name */
                "project description")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                Operator,                        /* option name */
                NULL,                          /* default value */
                false,                          /* is option mandatory */
                'o',                           /* short option name */
                "operator",                      /* long option name */
                "NAME",                           /* parametr name */
                "project operator")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                DynamicProperties,                        /* option name */
                NULL,                          /* default value */
                false,                          /* is option mandatory */
                'p',                           /* short option name */
                "properties",                      /* long option name */
                "LIST",                           /* parametr name */
                "comma separated list of dynamic properties")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(int,                           /* option type */
                NumOfResults,                        /* option name */
                5,                          /* default value */
                false,                          /* is option mandatory */
                'r',                           /* short option name */
                "results",                      /* long option name */
                "NUMBER",                           /* parametr name */
                "number of results")   /* option description */
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
