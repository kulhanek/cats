#ifndef TopCrdManipOptionsH
#define TopCrdManipOptionsH
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

class CTopCrdManipOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CTopCrdManipOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "topcrdmanip"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Manipulates with coordinates."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    CATS_VERSION
    CSO_PROG_VERS_END

    CSO_PROG_ARGS_SHORT_DESC_BEGIN
    "TOP CRD OUTCRD COMMAND [command options]"
    CSO_PROG_ARGS_SHORT_DESC_END

    CSO_PROG_ARGS_LONG_DESC_BEGIN
    "Arguments:\n"
    "* PARM         topology file name\n"
    "* INCRD        input coordinates in AMBER coordinate format\n"
    "* OUTCRD       output coordinates in AMBER coordinate format\n"
    "* COMMAND      manipulation command\n"
    "\n"
    "Supported commands:\n"
    "  identity     \n"
    "  center       mask [origin]\n"
    "  image        [origin] [familiar] [byatom]\n"
    "  move         mask dx dy dz\n"
    "  moveto       mask x   y  z\n"
    "  swap         mask1 maks2\n"
    "  principal    mask\n"
    CSO_PROG_ARGS_LONG_DESC_END


// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // options ------------------------------
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
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


// suplementary methods -------------------------------------------------------
    const CSmallString& GetArgTopologyName(void);
    const CSmallString& GetArgInCrdName(void);
    const CSmallString& GetArgOutCrdName(void);
    const CSmallString& GetArgCommand(void);
    const CSmallString& GetCommandArg(int pos);
    int                 GetNumOfCommandArgs(void);

// final operation with options -----------------------------------------------
private:
    virtual int CheckOptions(void);
    virtual int FinalizeOptions(void);
    virtual int CheckArguments(void);
};

//------------------------------------------------------------------------------

#endif
