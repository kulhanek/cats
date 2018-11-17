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
#include "ResClient.hpp"
#include <XMLElement.hpp>
#include <XMLBinData.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CResClient::CResClient(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CResClient::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CABFIntOpts
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

// attach verbose stream to terminal stream and set desired verbosity level
    vout.Attach(Console);
    if( Options.GetOptVerbose() ) {
        vout.Verbosity(CVerboseStr::debug);
    } else {
        vout.Verbosity(CVerboseStr::high);
    }

    // set server description - there should no be any problem
    // syntax was already checked
    ActionRequest.SetProtocolName("res");
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

    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# result-client started at " << dt.GetSDateAndTime() << endl;
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
        vout << "# Password                   : from file '" << passfile << "'" << endl;
    } else {
        vout << "# Password                   : from command line or server key file" << endl;
    }
    vout << "# ------------------------------------------------------------------------------" << endl;

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CResClient::Run(void)
{
    if( Options.GetOptVerbose() == true ) vout << endl;

    if( ! Options.IsOptServerKeySet() ){
       if( ReadPassword(Options.IsOptPasswordSet(),Options.GetOptPassword(),
                        Options.GetOptVerbose()) == false ) return(false);
       }

    if( Options.GetOptVerbose() == true ){
       vout << "Sending request ... " << ActionRequest.GetAction() << endl;
       }

    bool result;

    if( ActionRequest.GetAction() == "info" ) {
        result = GetServerInfo(vout);
    } else if( ActionRequest.GetAction() == "shutdown" ) {
        result = ShutdownServer(vout);
    } else if( ActionRequest.GetAction() == "errors" ) {
        result = GetServerErrors(vout);
    } else if( ActionRequest.GetAction() == "register" ) {
        result = RegisterClient();
    } else if( ActionRequest.GetAction() == "unregister" ) {
        result = UnregisterClient();
    } else if( ActionRequest.GetAction() == "write" ) {
        result = WriteData();
//    } else if( ActionRequest.GetAction() == "flush" ) {
//        result = FlushServerData();
    } else if( ActionRequest.GetAction() == "getstat" ) {
        result = GetStatistics();
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

void CResClient::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << debug;
    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# result-client terminated at " << dt.GetSDateAndTime() << endl;
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

bool CResClient::RegisterClient(void)
{
    CSmallString template_name;
    if( ActionRequest.GetParameterKeyValue("template",template_name) == false ){
        ES_TRACE_ERROR("unable to get template key");
        return(false);
    }

    int id = 0;
    if( CResultClient::RegisterClient(template_name,id) == false ) {
        ES_TRACE_ERROR("unable to register client");
        return(false);
    }
    vout << id;
    return(true);
}

//------------------------------------------------------------------------------

bool CResClient::UnregisterClient(void)
{
    int id = 0;
    if( ActionRequest.GetParameterKeyValue("id",id) == false ){
        ES_TRACE_ERROR("unable to get client id");
        return(false);
    }
    if( CExtraClient::UnregisterClient(id) == false ){
        CSmallString error;
        error << "unable to unregister client " << id;
        ES_TRACE_ERROR(error);
        return(false);
    }
    return(true);
}

//------------------------------------------------------------------------------

bool CResClient::WriteData(void)
{
    int id = 0;
    if( ActionRequest.GetParameterKeyValue("id",id) == false ){
        ES_TRACE_ERROR("unable to get client id");
        return(false);
    }
    CSmallString data_name;
    if( ActionRequest.GetParameterKeyValue("data",data_name) == false ){
        ES_TRACE_ERROR("unable to get data key");
        return(false);
    }
    if( CResultClient::WriteData(id,data_name) == false ){
        CSmallString error;
        error << "unable to write data to client " << id;
        ES_TRACE_ERROR(error);
        return(false);
    }
    return(true);
}

//------------------------------------------------------------------------------

bool CResClient::GetStatistics(void)
{
    CSmallString stat_name;
    if( ActionRequest.GetParameterKeyValue("file",stat_name) == false ){
        ES_TRACE_ERROR("unable to get file key");
        return(false);
    }
    if( CResultClient::GetStatistics(stat_name) == false ){
        CSmallString error;
        error << "unable to get statistics data";
        ES_TRACE_ERROR(error);
        return(false);
    }
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

