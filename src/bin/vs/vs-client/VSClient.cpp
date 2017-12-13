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

#include <stdio.h>
#include <ErrorSystem.hpp>
#include <ClientCommand.hpp>
#include "VSClient.hpp"
#include <XMLElement.hpp>
#include <XMLBinData.hpp>
#include <iostream>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVSClient::CVSClient(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CVSClient::Init(int argc,char* argv[])
{
// encode program options, all check procedures are done inside of CABFIntOpts
    int result = Options.ParseCmdLine(argc,argv);

// should we exit or was it error?
    if(result != SO_CONTINUE) return(result);

    MsgOut.Attach(Terminal);
    if(Options.GetOptVerbose()) MsgOut.Verbosity(CVerboseStr::high);

// set server description - there should no be any problem
// syntax was already checked
    ActionRequest.SetProtocolName("cheminfo");
    ActionRequest.SetQualifiedName(Options.GetArgCommand());

    if(Options.IsOptServerKeySet()) {
        try {
            ActionRequest.ReadServerKey(Options.GetOptServerKey());
        } catch(...) {
            CSmallString error;
            error << "unable to read server key";
            ES_ERROR(error);
            return(SO_ERROR);
        }
    }

    if(Options.GetOptVerbose() == true) {
        // print header --------------------------------------------------------------
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        printf("\n");
        printf("# ==============================================================================\n");
        printf("# vs-client started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        printf("# Server request (by user)   : %s\n",(const char*)Options.GetArgCommand());
        printf("# Server request (processed) : %s\n",(const char*)ActionRequest.GetQualifiedName());
        printf("#\n");
        printf("# ------------------------------------------------------------------------------\n");
        printf("# Server name                : %s\n",(const char*)ActionRequest.GetName());
        printf("# Server IP                  : %s\n",(const char*)ActionRequest.GetIP());
        printf("# Server port                : %d\n",ActionRequest.GetPort());
        printf("# Command                    : %s\n",(const char*)ActionRequest.GetAction());
        printf("# ------------------------------------------------------------------------------\n");
        CSmallString passfile;
        bool         passfile_set;
        passfile = Options.GetOptPassword(passfile_set);
        if(passfile_set == true) {
            printf("# Password                   : from file '%s'\n",(const char*)passfile);
        } else {
            printf("# Password                   : from command line or server key file\n");
        }
        printf("# ------------------------------------------------------------------------------\n");
    }

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSClient::Run(void)
{
    if(Options.GetOptVerbose() == true) printf("\n");

    if(! Options.IsOptServerKeySet()) {
        if(ReadPassword(Options.IsOptPasswordSet(),Options.GetOptPassword(),
                        Options.GetOptVerbose()) == false) return(false);
    }

    if(Options.GetOptVerbose() == true) {
        printf("Sending request ... %s.\n",(const char*)ActionRequest.GetAction());
    }

    bool result;

    if(ActionRequest.GetAction() == "info") {
        result = GetServerInfo(MsgOut);
    } else if(ActionRequest.GetAction() == "shutdown") {
        result = ShutdownServer(MsgOut);
    } else if(ActionRequest.GetAction() == "errors") {
        result = GetServerErrors(MsgOut);
    } else if(ActionRequest.GetAction() == "register") {
        int client_id = -1;
        result = RegisterClient(client_id);
        if(Options.GetOptVerbose() == true) {
            printf("\n");
            printf("Client ID: ");
        }
        printf("%d\n",client_id);
    } else if(ActionRequest.GetAction() == "unregister") {
        CSmallString client_id("-1");
        ActionRequest.GetParameterKeyValue("id",client_id);
        result = UnregisterClient(client_id.ToInt());
    } else if(ActionRequest.GetAction() == "selreset") {
        result = SelReset();
    } else if(ActionRequest.GetAction() == "installpkg") {
        result = InstallClientPkg();
    } else if(ActionRequest.GetAction() == "appname") {
        bool silent_error = false;
        result = GetClientAppName(silent_error);
        if(silent_error == true) return(false);
    } else if(ActionRequest.GetAction() == "load") {
        result = LoadStructure();
    }  else if(ActionRequest.GetAction() == "save") {
        result = SaveStructure();
    }  else if(ActionRequest.GetAction() == "write") {
        result = WriteData();
    } else if(ActionRequest.GetAction() == "get") {
        CSmallString molid;
        result = GetData(molid);

        if(Options.GetOptVerbose() == true) {
            printf("Molecule ID: %s\n",(const char*)molid);
        } else {
            printf("%s",(const char*)molid);
        }
        if(molid == "eof") return(false);
    } else {
        CSmallString error;
        error << "action '" << ActionRequest.GetAction() << "' is not implemented";
        ES_ERROR(error);
        result = false;
    }

    if(Options.GetOptVerbose() == true) {
        if(result == false) {
            printf("Command execution failed.\n");
        }
    }

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSClient::Finalize(void)
{
    if(Options.GetOptVerbose()) {
        ErrorSystem.PrintErrors(stderr);

        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        fprintf(stdout,"\n");
        fprintf(stdout,"# ==============================================================================\n");
        fprintf(stdout,"# vs-client terminated at %s\n",(const char*)dt.GetSDateAndTime());
        fprintf(stdout,"# ==============================================================================\n");
        fprintf(stdout,"\n");
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

