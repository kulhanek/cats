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

//------------------------------------------------------------------------------

using namespace std;

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
    MsgOut.Verbosity(CVerboseStr::low);
    if(Options.GetOptVerbose()) MsgOut.Verbosity(CVerboseStr::high);

// set server description - there should no be any problem
// syntax was already checked
    ActionRequest.SetProtocolName("svs");
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

    // print header --------------------------------------------------------------
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << high;
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# svs-client started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << endl;
    MsgOut << "# Server request (by user)   : " << Options.GetArgCommand() << endl;
    MsgOut << "# Server request (processed) : " << ActionRequest.GetQualifiedName() << endl;
    MsgOut << endl;
    MsgOut << "# ------------------------------------------------------------------------------" << endl;
    MsgOut << "# Server name                : " << ActionRequest.GetName() << endl;
    MsgOut << "# Server IP                  : " << ActionRequest.GetIP() << endl;
    MsgOut << "# Server port                : " << ActionRequest.GetPort() << endl;
    MsgOut << "# Command                    : " << ActionRequest.GetAction() << endl;
    MsgOut << "# ------------------------------------------------------------------------------" << endl;
    CSmallString passfile;
    bool         passfile_set;
    passfile = Options.GetOptPassword(passfile_set);
    if(passfile_set == true) {
        MsgOut << "# Password                   : from file '" << passfile << "'" << endl;
    } else {
        MsgOut << "# Password                   : from command line or server key file" << endl;
    }
    MsgOut << "# ------------------------------------------------------------------------------" << endl;

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSClient::Run(void)
{
    if(! Options.IsOptServerKeySet()) {
        if(ReadPassword(Options.IsOptPasswordSet(),Options.GetOptPassword(),
                        Options.GetOptVerbose()) == false) return(false);
    }

    MsgOut << high;
    MsgOut << endl;
    MsgOut << "Sending request ... " << ActionRequest.GetAction() << endl;

    bool result;

    if(ActionRequest.GetAction() == "info") {
        MsgOut << low;
        result = GetServerInfo(MsgOut);
    } else if(ActionRequest.GetAction() == "shutdown") {
        MsgOut << low;
        result = ShutdownServer(MsgOut);
    } else if(ActionRequest.GetAction() == "errors") {
        MsgOut << low;
        result = GetServerErrors(MsgOut);
    } else if(ActionRequest.GetAction() == "register") {
        int client_id = -1;
        result = RegisterClient(client_id);
        MsgOut << high;
        MsgOut << endl;
        MsgOut << "Client ID: ";
        MsgOut << low;
        MsgOut << client_id;
        MsgOut << high;
        MsgOut << endl;
    } else if(ActionRequest.GetAction() == "unregister") {
        CSmallString client_id("-1");
        ActionRequest.GetParameterKeyValue("id",client_id);
        result = UnregisterClient(client_id.ToInt());
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

        MsgOut << high;
        MsgOut << "Molecule ID: ";
        MsgOut << low;
        MsgOut << molid;
        MsgOut << high;
        MsgOut << endl;

        if(molid == "eof") return(false);
    } else {
        CSmallString error;
        error << "action '" << ActionRequest.GetAction() << "' is not implemented";
        ES_ERROR(error);
        result = false;
    }

    if(result == false) {
        MsgOut << high;
        MsgOut << "Command execution failed." << endl;
    }

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSClient::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << endl;
    MsgOut << high;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# svs-client terminated at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;

    if(ErrorSystem.IsError() || Options.GetOptVerbose()) {
        ErrorSystem.PrintErrors(stderr);
        fprintf(stderr,"\n");
    } else{
        MsgOut << endl;
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

