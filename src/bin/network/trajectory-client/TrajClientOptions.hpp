#ifndef TrajClientOptionsH
#define TrajClientOptionsH
// ===============================================================================
// PMFLib - Library Supporting Potential of Mean Force Calculations
// -------------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu
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
// ===============================================================================

#include <SimpleOptions.hpp>
#include <CATsMainHeader.hpp>

//------------------------------------------------------------------------------

class CTrajClientOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CTrajClientOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "trajectory-client"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "This program interacts with a trajectory server."
    CSO_PROG_DESC_END

    CSO_PROG_VERS_BEGIN
    LibBuildVersion_CATs
    CSO_PROG_VERS_END

// list of all options and arguments ------------------------------------------
    CSO_LIST_BEGIN
    // arguments ----------------------------
    CSO_ARG(CSmallString,Command)
    // options ------------------------------
    CSO_OPT(CSmallString,ServerKey)
    CSO_OPT(CSmallString,Password)
    CSO_OPT(bool,Help)
    CSO_OPT(bool,Version)
    CSO_OPT(bool,Verbose)
    CSO_LIST_END

    CSO_MAP_BEGIN
// description of arguments ---------------------------------------------------
    CSO_MAP_ARG(CSmallString,                   /* argument type */
                Command,                          /* argument name */
                NULL,                           /* default value */
                true,                           /* is argument mandatory */
                "trj://server[:port]/command",                        /* parametr name */
                "specifies fully qualified administration request. The trasport protocol trj:// has to be always specified. "
                "It is followed by a server parameter, which could be either DNS name or IP address of server. "
                "Optional port parameter is port number used in a communication. Finally, the command is a task, "
                "which can be one of the following:\n"
                "   <green>register</green>   = register client on server side\n"
                "   <green>unregister</green> = unregister client on server side (unregister?id=client_id)\n"
                "   <green>getcrd</green>     = get coordinate snapshot (getcrd?id=client_id,topology=file1.top,coords=file2.crd)\n"
                "   <green>getvel</green>     = get velocities (getvel?id=client_id,topology=file1.top,coords=file2.crd)\n"
                "   <green>info</green>       = prints information about registered clients\n"
                "   <green>shutdown</green>   = stops server execution\n"
                "   <green>errors</green>     = prints errors from server stack\n"
               )   /* argument description */
// description of options -----------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                ServerKey,                        /* option name */
                NULL,                          /* default value */
                false,                          /* is option mandatory */
                's',                           /* short option name */
                "serverkey",                      /* long option name */
                "FILE",                           /* parametr name */
                "name of file containing server description (name, port, and password) (mutually exclusive with 'password' option)")   /* option description */
    //----------------------------------------------------------------------
    CSO_MAP_OPT(CSmallString,                           /* option type */
                Password,                        /* option name */
                NULL,                          /* default value */
                false,                          /* is option mandatory */
                'p',                           /* short option name */
                "password",                      /* long option name */
                "FILE",                           /* parametr name */
                "name of file containing server magic word, if not specified then password is read interactively")   /* option description */
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
