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

#include "ResClientOptions.h"
#include <ActionRequest.hpp>
#include <ErrorSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResClientOptions::CResClientOptions(void)
{
    SetShowMiniUsage(true);
}

//------------------------------------------------------------------------------

int CResClientOptions::CheckOptions(void)
{
    if( IsOptPasswordSet() && IsOptServerKeySet() ) {
        if( IsError == false ) fprintf(stderr,"\n");
        fprintf(stderr,"%s: --password and --serverkey options are mutually exclusive\n", (char*)GetProgramName());
        IsError = true;
        return(SO_OPTS_ERROR);
    }

    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

int CResClientOptions::FinalizeOptions(void)
{
    bool ret_opt = false;

    if( GetOptHelp() == true ) {
        PrintUsage();
        ret_opt = true;
    }

    if( GetOptVersion() == true ) {
        PrintVersion();
        ret_opt = true;
    }

    if( ret_opt == true ) {
        printf("\n");
        return(SO_EXIT);
    }

    return(SO_CONTINUE);
}

//------------------------------------------------------------------------------

int CResClientOptions::CheckArguments(void)
{
    CActionRequest server_desc;
    server_desc.SetProtocolName("res");

    if( server_desc.SetQualifiedName(GetArgCommand()) == false ) {
        if( IsVerbose() ) {
            if( IsError == false ) fprintf(stderr,"\n");
            fprintf(stderr,"%s: %s\n", (const char*)GetProgramName(), (const char*)ErrorSystem.GetLastError());
            IsError = true;
        }
        return(SO_OPTS_ERROR);
    }

    if( server_desc.GetAction() == NULL ) {
        if( IsVerbose() ) {
            if( IsError == false ) fprintf(stderr,"\n");
            fprintf(stderr,"%s: no action is specified\n", (const char*)GetProgramName());
            IsError = true;
        }
        return(SO_OPTS_ERROR);
    }

    if( (server_desc.GetAction() != "register") &&
            (server_desc.GetAction() != "unregister") &&
            (server_desc.GetAction() != "write") &&
            (server_desc.GetAction() != "getstat") &&
            (server_desc.GetAction() != "info") &&
            (server_desc.GetAction() != "shutdown") &&
            (server_desc.GetAction() != "flush") &&
            (server_desc.GetAction() != "errors") ) {
        if( IsVerbose() ) {
            if( IsError == false ) fprintf(stderr,"\n");
            fprintf(stderr,"%s: specified action '%s' is not supported\n", (const char*)GetProgramName(), (const char*)server_desc.GetAction());
            IsError = true;
        }
        return(SO_OPTS_ERROR);
    }

    // check particular commands
    if( server_desc.GetAction() == "register" ) {
        CSmallString value;
        if( server_desc.GetParameterKeyValue("template",value) == false ) {
            if( IsVerbose() ) {
                if( IsError == false ) fprintf(stderr,"\n");
                fprintf(stderr,"%s: specification of template name is required by register command\n",
                        (const char*)GetProgramName());
                IsError = true;
            }
            return(SO_OPTS_ERROR);
        }
        return(SO_CONTINUE);
    }

    if( server_desc.GetAction() == "unregister" ) {
        CSmallString value;
        if( server_desc.GetParameterKeyValue("id",value) == false ) {
            if( IsVerbose() ) {
                if( IsError == false ) fprintf(stderr,"\n");
                fprintf(stderr,"%s: specification of client id is required by unregister command\n",
                        (const char*)GetProgramName());
                IsError = true;
            }
            return(SO_OPTS_ERROR);
        }
        return(SO_CONTINUE);
    }

    if( server_desc.GetAction() == "write" ) {
        CSmallString value;
        if( server_desc.GetParameterKeyValue("id",value) == false ) {
            if( IsVerbose() ) {
                if( IsError == false ) fprintf(stderr,"\n");
                fprintf(stderr,"%s: specification of client id is required by write command\n",
                        (const char*)GetProgramName());
                IsError = true;
            }
            return(SO_OPTS_ERROR);
        }
        if( server_desc.GetParameterKeyValue("data",value) == false ) {
            if( IsVerbose() ) {
                if( IsError == false ) fprintf(stderr,"\n");
                fprintf(stderr,"%s: specification of data file is required by write command\n",
                        (const char*)GetProgramName());
                IsError = true;
            }
            return(SO_OPTS_ERROR);
        }
        return(SO_CONTINUE);
    }

    if( server_desc.GetAction() == "getstat" ) {
        CSmallString value;
        if( server_desc.GetParameterKeyValue("file",value) == false ) {
            if( IsVerbose() ) {
                if( IsError == false ) fprintf(stderr,"\n");
                fprintf(stderr,"%s: specification of output file is required by getstat command\n",
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
