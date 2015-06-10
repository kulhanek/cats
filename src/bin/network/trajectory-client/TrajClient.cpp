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

#include <stdio.h>
#include <ErrorSystem.hpp>
#include <ClientCommand.hpp>
#include "TrajClient.hpp"
#include <XMLElement.hpp>
#include <XMLBinData.hpp>
#include <AmberTopology.hpp>
#include <AmberRestart.hpp>
#include <iostream>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTrajClient::CTrajClient(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTrajClient::Init(int argc,char* argv[])
{
// encode program options, all check procedures are done inside of CABFIntOpts
    int result = Options.ParseCmdLine(argc,argv);

// should we exit or was it error?
    if(result != SO_CONTINUE) return(result);

// attach verbose stream to terminal stream and set desired verbosity level
    vout.Attach(Console);
    if( Options.GetOptVerbose() ) {
        vout.Verbosity(CVerboseStr::debug);
    } else {
        vout.Verbosity(CVerboseStr::high);
    }

// print header --------------------------------------------------------------
    // set server description - there should no be any problem
    // syntax was already checked
    ActionRequest.SetProtocolName("trj");
    ActionRequest.SetQualifiedName(Options.GetArgCommand());

    if( Options.IsOptServerKeySet() ) {
        ActionRequest.ReadServerKey(Options.GetOptServerKey());
    }

    // is port valid?
    if( ActionRequest.GetPort() == INVALID_PORT ) {
        CSmallString error;
        error << "invalid port is specified";
        ES_ERROR(error);
        return(SO_ERROR);
    }

    // print header --------------------------------------------------------------
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << debug;
    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# trajectory-client started at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;
    vout << "#" << endl;
    vout << "# Server request (by user)   : " << Options.GetArgCommand() << endl;
    vout << "# Server request (processed) : " << ActionRequest.GetQualifiedName() << endl;
    vout << "#" << endl;
    vout << "# ------------------------------------------------------------------------------" << endl;
    vout << "# Server name                : " << ActionRequest.GetName() << endl;
    vout << "# Server IP                  : " << ActionRequest.GetIP() << endl;
    vout << "# Server port                : " << ActionRequest.GetPort() << endl;
    vout << "# Command                    : " << ActionRequest.GetAction() << endl;
    vout << "# ------------------------------------------------------------------------------" << endl;
    CSmallString passfile;
    bool         passfile_set;
    passfile = Options.GetOptPassword(passfile_set);
    if( passfile_set == true ) {
        vout << "# Password                   : from file " << passfile << endl;
    } else {
        vout << "# Password                   : from command line or server key file" << endl;
    }
    vout << "# ------------------------------------------------------------------------------" << endl;

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTrajClient::Run(void)
{
    vout << endl;

    if( ! Options.IsOptServerKeySet() ){
       if( ReadPassword(Options.IsOptPasswordSet(),Options.GetOptPassword(),
                        Options.GetOptVerbose()) == false ) return(false);
    }

    if( Options.GetOptVerbose() == true ){
       vout << "Sending request ... " << ActionRequest.GetAction() << endl;
    }

    bool result = true;

    if( ActionRequest.GetAction() == "info" ) {
        result = GetServerInfo(vout);
    } else if( ActionRequest.GetAction() == "shutdown" ) {
        result = ShutdownServer(vout);
    } else if( ActionRequest.GetAction() == "errors" ) {
        result = GetServerErrors(vout);
    } else if( ActionRequest.GetAction() == "register" ) {
        RegisterClient();
    } else if( ActionRequest.GetAction() == "unregister" ) {
        UnregisterClient();
    } else if( ActionRequest.GetAction() == "getcrd" ) {
        GetSnapshot(false);
    } else if( ActionRequest.GetAction() == "getvel" ) {
        GetSnapshot(true);
    } else {
        CSmallString error;
        error << "action '" << ActionRequest.GetAction() << "' is not implemented";
        ES_ERROR(error);
        result = false;
    }

    if( Options.GetOptVerbose() == true ) {
        if( result == false ) {
            vout << "Command execution failed." << endl;
        }
    }

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajClient::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << debug;
    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# trajectory-client terminated at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;

    if( ErrorSystem.IsError() ){
        vout << high;
        ErrorSystem.PrintErrors(vout);
    }
    vout << endl;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTrajClient::RegisterClient(void)
{
    int id = 0;
    if( CExtraClient::RegisterClient(id) == false ) {
        RUNTIME_ERROR("unable to register client");
    }
    vout << low;
    vout << id << endl;
    vout << high;
}

//------------------------------------------------------------------------------

void CTrajClient::UnregisterClient(void)
{
    int id = 0;
    if( ActionRequest.GetParameterKeyValue("id",id) == false ){
        RUNTIME_ERROR("unable to get client id");
    }
    if( CExtraClient::UnregisterClient(id) == false ){
        CSmallString error;
        error << "unable to unregister client " << id;
        RUNTIME_ERROR(error);
    }
}

//------------------------------------------------------------------------------

void CTrajClient::GetSnapshot(bool read_vel)
{
    int id = 0;
    if( ActionRequest.GetParameterKeyValue("id",id) == false ){
        RUNTIME_ERROR("unable to get client id");
    }
    CSmallString topname;
    if( ActionRequest.GetParameterKeyValue("topology",topname) == false ){
        RUNTIME_ERROR("unable to get system topology name");
    }
    CSmallString crdname;
    if( ActionRequest.GetParameterKeyValue("coords",crdname) == false ){
        RUNTIME_ERROR("unable to get coordinate file name");
    }

    CAmberTopology top;
    CAmberRestart  crd;

    if( top.Load(topname) == false ){
        CSmallString error;
        error << "unable to load topology '" << topname << "'";
        RUNTIME_ERROR(error);
    }

    crd.AssignTopology(&top);

    if( read_vel ){
        if( crd.Load(crdname) == false ){
            CSmallString error;
            error << "unable to load coordinate file '" << crdname << "'";
            RUNTIME_ERROR(error);
        }
    }

    if( CTrajectoryClient::GetSnapshot(id,&crd,"next",read_vel) == false ){
        RUNTIME_ERROR("unable to get next snapshot");
    }

    if( crd.Save(crdname) == false ){
        CSmallString error;
        error << "unable to save coordinate file '" << crdname << "'";
        RUNTIME_ERROR(error);
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

