#ifndef AlterPrjOptionsH
#define AlterPrjOptionsH
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

class CAlterPrjOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CAlterPrjOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "svs-project-alter"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Alter a project setup."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,ProjectName)
    CSO_ARG(CSmallString,Command)
    // options ------------------------------
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of arguments ---------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                ProjectName,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "projectdb",                        /* parametr name */
                "filename with the project database")   /* argument description */
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                Command,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "command",                        /* parametr name */
                "Supported commands:\n"
                "   softreset   = set processing items into unprocessed state\n"
                "   hardreset   = set all items into unprocessed state\n"
                "   finalizeall = set all items into processed state\n"
                "   addindx     = add primary index to the ID column\n"
                "   removeindx  = remove primary index from the ID solumn\n"
                "   rebuildindx = removeindx + addindx\n"
                "   deleteall   = delete all records from project database\n"
               )   /* argument description */
// description of options -----------------------------------------------------
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
