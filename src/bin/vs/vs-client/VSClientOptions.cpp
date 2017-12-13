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

#include "VSClientOptions.hpp"
#include <ActionRequest.hpp>
#include <ErrorSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVSClientOptions::CVSClientOptions(void)
{
    SetShowMiniUsage(true);
}

//------------------------------------------------------------------------------

int CVSClientOptions::CheckOptions(void)
{
    if(IsOptPasswordSet() && IsOptServerKeySet()) {
        if(IsError == false) fprintf(stderr,"\n");
        fprintf(stderr,"%s: --password and --serverkey options are mutually exclusive\n", (char*)GetProgramName());
        IsError = true;
        return(SO_OPTS_ERROR);
    }

    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

int CVSClientOptions::FinalizeOptions(void)
{
    bool ret_opt = false;

    if(GetOptHelp() == true) {
        PrintUsage();
        ret_opt = true;
    }

    if(GetOptVersion() == true) {
        PrintVersion();
        ret_opt = true;
    }

    if(ret_opt == true) {
        printf("\n");
        return(SO_EXIT);
    }

    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

int CVSClientOptions::CheckArguments(void)
{
    CActionRequest server_desc;
    server_desc.SetProtocolName("cheminfo");

    try {
        server_desc.SetQualifiedName(GetArgCommand());
    } catch(...) {
        if(IsVerbose()) {
            if(IsError == false) fprintf(stderr,"\n");
            fprintf(stderr,"%s: %s\n", (const char*)GetProgramName(), (const char*)ErrorSystem.GetLastError());
            IsError = true;
        }
        return(SO_OPTS_ERROR);
    }

    if(server_desc.GetAction() == NULL) {
        if(IsVerbose()) {
            if(IsError == false) fprintf(stderr,"\n");
            fprintf(stderr,"%s: no action is specified\n", (const char*)GetProgramName());
            IsError = true;
        }
        return(SO_OPTS_ERROR);
    }

    if((server_desc.GetAction() != "register") &&
            (server_desc.GetAction() != "unregister") &&
            (server_desc.GetAction() != "info") &&
            (server_desc.GetAction() != "shutdown") &&
            (server_desc.GetAction() != "errors") &&
            (server_desc.GetAction() != "installpkg") &&
            (server_desc.GetAction() != "appname") &&
            (server_desc.GetAction() != "selreset") &&
            (server_desc.GetAction() != "get") &&
            (server_desc.GetAction() != "write") &&
            (server_desc.GetAction() != "load") &&
            (server_desc.GetAction() != "save")
            ) {
        if(IsVerbose()) {
            if(IsError == false) fprintf(stderr,"\n");
            fprintf(stderr,"%s: specified action '%s' is not supported\n", (const char*)GetProgramName(), (const char*)server_desc.GetAction());
            IsError = true;
        }
        return(SO_OPTS_ERROR);
    }

    if(server_desc.GetAction() == "unregister") {
        CSmallString value;
        if(server_desc.GetParameterKeyValue("id",value) == false) {
            if(IsVerbose()) {
                if(IsError == false) fprintf(stderr,"\n");
                fprintf(stderr,"%s: specification of client id is required by unregister command\n",
                        (const char*)GetProgramName());
                IsError = true;
            }
            return(SO_OPTS_ERROR);
        }
        return(SO_CONTINUE);
    }

    if(server_desc.GetAction() == "get") {
        CSmallString value;
        if(server_desc.GetParameterKeyValue("id",value) == false) {
            if(IsVerbose()) {
                if(IsError == false) fprintf(stderr,"\n");
                fprintf(stderr,"%s: specification of client id is required by get command\n",
                        (const char*)GetProgramName());
                IsError = true;
            }
            return(SO_OPTS_ERROR);
        }
        return(SO_CONTINUE);
    }

    if(server_desc.GetAction() == "write") {
        CSmallString value;
        if(server_desc.GetParameterKeyValue("id",value) == false) {
            if(IsVerbose()) {
                if(IsError == false) fprintf(stderr,"\n");
                fprintf(stderr,"%s: specification of client id is required by write command\n",
                        (const char*)GetProgramName());
                IsError = true;
            }
            return(SO_OPTS_ERROR);
        }
        if(server_desc.GetParameterKeyValue("molid",value) == false) {
            if(IsVerbose()) {
                if(IsError == false) fprintf(stderr,"\n");
                fprintf(stderr,"%s: specification of molid is required by write command\n",
                        (const char*)GetProgramName());
                IsError = true;
            }
            return(SO_OPTS_ERROR);
        }
        return(SO_CONTINUE);
    }

    if(server_desc.GetAction() == "load") {
        CSmallString value;
        if(server_desc.GetParameterKeyValue("id",value) == false) {
            if(IsVerbose()) {
                if(IsError == false) fprintf(stderr,"\n");
                fprintf(stderr,"%s: specification of client id is required by load command\n",
                        (const char*)GetProgramName());
                IsError = true;
            }
            return(SO_OPTS_ERROR);
        }
        if(server_desc.GetParameterKeyValue("molid",value) == false) {
            if(IsVerbose()) {
                if(IsError == false) fprintf(stderr,"\n");
                fprintf(stderr,"%s: specification of molid is required by load command\n",
                        (const char*)GetProgramName());
                IsError = true;
            }
            return(SO_OPTS_ERROR);
        }
        return(SO_CONTINUE);
    }

    if(server_desc.GetAction() == "save") {
        CSmallString value;
        if(server_desc.GetParameterKeyValue("id",value) == false) {
            if(IsVerbose()) {
                if(IsError == false) fprintf(stderr,"\n");
                fprintf(stderr,"%s: specification of client id is required by save command\n",
                        (const char*)GetProgramName());
                IsError = true;
            }
            return(SO_OPTS_ERROR);
        }
        if(server_desc.GetParameterKeyValue("molid",value) == false) {
            if(IsVerbose()) {
                if(IsError == false) fprintf(stderr,"\n");
                fprintf(stderr,"%s: specification of molid is required by save command\n",
                        (const char*)GetProgramName());
                IsError = true;
            }
            return(SO_OPTS_ERROR);
        }
        return(SO_CONTINUE);
    }

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
