#ifndef AddStrPrjOptionsH
#define AddStrPrjOptionsH
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
#include <CATsMainHeader.hpp>

//------------------------------------------------------------------------------

class CAddStrPrjOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CAddStrPrjOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "svs-project-add-structures"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Initialize the project database by records from the structure database."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,StructurePath)
    CSO_ARG(CSmallString,ProjectName)
    // options ------------------------------
    CSO_OPT(CSmallString,InputFormat)
    CSO_OPT(bool,Progress)
    CSO_OPT(bool,UseHiearchy)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of arguments ---------------------------------------------------

    CSO_MAP_ARG(CSmallString,                   /* argument type */
                StructurePath,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "structuredb",                        /* parametr name */
                "pathname to the structure database")   /* argument description */
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                ProjectName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "projectdb",                        /* parametr name */
                "filename with the project database")   /* argument description */
// description of options -----------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                InputFormat,                        /* option name */
                "xyz",                          /* default value */
                false,                          /* is option mandatory */
                'i',                           /* short option name */
                "input",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "input molecule format")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                Progress,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'p',                           /* short option name */
                "progress",                      /* long option name */
                NULL,                           /* parametr name */
                "print progress")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                UseHiearchy,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'u',                           /* short option name */
                "usehiearchy",                      /* long option name */
                NULL,                           /* parametr name */
                "use hiearchy")   /* option description */
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
};

//------------------------------------------------------------------------------

#endif
