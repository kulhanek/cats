#ifndef MolSplitOptionsH
#define MolSplitOptionsH
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

class CMolSplitOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CMolSplitOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "mol-split"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Split a multiple structure file into individual structures. "
    "The input file can be processed either by openbalel (--format <i>auto</i> or user specified input format) or "
    "by a special parser (--lineentries) in the case of <i>ssmi</i> (stream SMILE file). The format <i>ssmi</i> contains two entries on every line. "
    "The first entry is an ID of molecule and the second entry is SMILE representation of a molecule."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,InputFile)
    CSO_ARG(CSmallString,StructurePath)
    // options ------------------------------
    CSO_OPT(CSmallString,InFormat)
    CSO_OPT(CSmallString,OutFormat)
    CSO_OPT(CSmallString,UnisID)
    CSO_OPT(bool,LineEntries)
    CSO_OPT(bool,CreateHiearchy)
    CSO_OPT(bool,PrintDuplicit)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of arguments ---------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                InputFile,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "input",                        /* parametr name */
                "name of input file or - for input from the standard input stream")   /* argument description */
    //----------------------------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                StructurePath,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "structuredb",                        /* parametr name */
                "pathname to the structure database")   /* argument description */
// description of options -----------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                InFormat,                        /* option name */
                "auto",                          /* default value */
                false,                          /* is option mandatory */
                'f',                           /* short option name */
                "informat",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "input molecule format")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                OutFormat,                        /* option name */
                "xyz",                          /* default value */
                false,                          /* is option mandatory */
                'o',                           /* short option name */
                "outformat",                      /* long option name */
                "FORMAT",                           /* parametr name */
                "output molecule format")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                UnisID,                        /* option name */
                "UNIS",                          /* default value */
                false,                          /* is option mandatory */
                'a',                           /* short option name */
                "unisID",                      /* long option name */
                "NAME",                           /* parametr name */
                "UNIS type (four characters) with autoindexing or 'title' for UNIS ID extracted from molecule title")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                LineEntries,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'l',                           /* short option name */
                "lineentries",                      /* long option name */
                NULL,                           /* parametr name */
                "read as line entries")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                CreateHiearchy,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'c',                           /* short option name */
                "createhiearchy",                      /* long option name */
                NULL,                           /* parametr name */
                "create directory hiearchy")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(bool,                           /* option type */
                PrintDuplicit,                        /* option name */
                false,                          /* default value */
                false,                          /* is option mandatory */
                'd',                           /* short option name */
                "printduplicit",                      /* long option name */
                NULL,                           /* parametr name */
                "print IDs of duplicit molecules")   /* option description */
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
