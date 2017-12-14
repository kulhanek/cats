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
#include "VSClient.hpp"
#include <VSOperation.hpp>
#include <ErrorSystem.hpp>
#include <ClientCommand.hpp>
#include <XMLBinData.hpp>
#include <PrmFile.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSClient::WriteData(void)
{
    CSmallString  client_id("-1");

    if(ActionRequest.GetParameterKeyValue("id",client_id) == false) {
        ES_ERROR("unable to get client id from command specification");
        return(false);
    }

    // is client_id integer?
    if(client_id.IsInt() == false) {
        CSmallString error;
        error << "specified client id '" << client_id << "' is not an integer number";
        ES_ERROR(error);
        return(false);
    }

    CSmallString molid;
    if(ActionRequest.GetParameterKeyValue("molid",molid) == false) {
        ES_ERROR("unable to get molid from command specification");
        return(false);
    }

    // -------------------------------------------

    // create command
    CClientCommand* p_command = CreateCommand(Operation_WriteVSData);
    if(p_command == NULL) return(false);

    // set client ID
    CXMLElement* p_ele = p_command->GetRootCommandElement();
    if(p_ele == NULL) {
        ES_ERROR("unable to get root command element");
        delete p_command;
        return(false);
    }

    bool result = true;
    p_ele->SetAttribute("client_id",client_id);
    p_ele->SetAttribute("molid",molid);

    if(result == false) {
        ES_ERROR("unable to set client_id and/or molid");
        delete p_command;
        return(false);
    }

    if(WriteStructure(p_ele) == false) {
        ES_ERROR("unable to write structure");
        delete p_command;
        return(false);
    }

    // FIXME - try/catch
    ExecuteCommand(p_command);

    if(result == false) {
        ES_ERROR("unable to execute command");
        delete p_command;
        return(false);
    }

    delete p_command;

    return(true);
}

//------------------------------------------------------------------------------

bool CVSClient::SaveStructure(void)
{
    CSmallString  client_id("-1");

    if(ActionRequest.GetParameterKeyValue("id",client_id) == false) {
        ES_ERROR("unable to get client id from command specification");
        return(false);
    }

    // is client_id integer?
    if(client_id.IsInt() == false) {
        CSmallString error;
        error << "specified client id '" << client_id << "' is not an integer number";
        ES_ERROR(error);
        return(false);
    }

    CSmallString molid;
    if(ActionRequest.GetParameterKeyValue("molid",molid) == false) {
        ES_ERROR("unable to get molid from command specification");
        return(false);
    }

    // -------------------------------------------

    // create command
    CClientCommand* p_command = CreateCommand(Operation_SaveStructure);
    if(p_command == NULL) return(false);

    // set client ID
    CXMLElement* p_ele = p_command->GetRootCommandElement();
    if(p_ele == NULL) {
        ES_ERROR("unable to get root command element");
        delete p_command;
        return(false);
    }

    bool result = true;
    p_ele->SetAttribute("client_id",client_id);
    p_ele->SetAttribute("molid",molid);

    if(result == false) {
        ES_ERROR("unable to set client_id and/or molid");
        delete p_command;
        return(false);
    }

    if(WriteStructure(p_ele) == false) {
        ES_ERROR("unable to write structure");
        delete p_command;
        return(false);
    }

    // FIXME - try/catch
    ExecuteCommand(p_command);

    if(result == false) {
        ES_ERROR("unable to execute command");
        delete p_command;
        return(false);
    }

    delete p_command;

    return(true);
}

//------------------------------------------------------------------------------

bool CVSClient::WriteStructure(CXMLElement* p_root)
{
    CSmallString str_file_name,str_type;

    // this is optional operation
    if(ActionRequest.GetParameterKeyValue("structure",str_file_name) == false) return(true);

    if(ActionRequest.GetParameterKeyValue("type",str_type) == false) {
        str_type="log";
    }

    // append structure if it is provided ----------------------------
    CXMLBinData* p_mol = p_root->CreateChildBinData("STRUCTURE");
    if(p_mol == NULL) {
        ES_ERROR("unable to create STRUCTURE element");
        return(false);
    }

    FILE* p_fin = fopen(str_file_name,"rb");
    if(p_fin == NULL) {
        CSmallString error;
        error << "unable to open molecule file (" << str_file_name << ")";
        ES_ERROR(error);
        return(false);
    }

    fseek(p_fin,0,SEEK_END);
    long molfilelen=ftell(p_fin);
    fseek(p_fin,0,SEEK_SET);

    p_mol->SetLength(molfilelen,EXBDT_CHAR);

    fread(p_mol->GetData(),molfilelen,sizeof(char),p_fin);
    fclose(p_fin);

    p_root->SetAttribute("type",str_type);

    return(true);
}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
