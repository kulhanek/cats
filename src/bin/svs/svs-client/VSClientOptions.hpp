#ifndef VSClientOptionsH
#define VSClientOptionsH
// =============================================================================
// ChemInfo - Chemoinformatics Tools
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

class CVSClientOptions : public CSimpleOptions {
public:
    // constructor - tune option setup
    CVSClientOptions(void);

// program name and description -----------------------------------------------
    CSO_PROG_NAME_BEGIN
    "vs-client"
    CSO_PROG_NAME_END

    CSO_PROG_DESC_BEGIN
    "Client of the server/client virtual screening infrastructure."
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
                "svs://server[:port]/command",                        /* parametr name */
                "Specifies fully qualified administration request. Trasport protocol svs:// has to be always specified, "
                "which is followed by the server specification (DNS name or IP address of the server and optionally "
                "by the server port). This is followed by the command, which can be one of the following:\n"
                "   <green>register</green>   = register client on server side\n"
                "   <green>unregister</green> = unregister client on server side (unregister?id=client_id)\n"
                "   <green>get</green>        = get unprocessed structure from database (get?id=client_id,structure=file.ext[,type=xyz])\n"
                "   <green>write</green>      = write data to database and set structure as processed (write?id=client_id,molid=molecule_id[,structure=file.ext,type=log])\n"
                "   <green>load</green>       = load structure from database (load?id=client_id,molid=molecule_id[,structure=file.ext,type=xyz])\n"
                "   <green>save</green>       = save structure to database (save?id=client_id,molid=molecule_id[,structure=file.ext,type=log])\n"
                "   <green>installpkg</green> = install client package to current directory\n"
                "   <green>appname</green>    = get client application name\n"
                "   <green>info</green>       = prints information about registered clients\n"
                "   <green>shutdown</green>   = stops server execution\n"
                "   <green>errors</green>     = prints errors from the server stack\n"
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
