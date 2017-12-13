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
#include <XMLIterator.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSClient::GetData(CSmallString& molid)
{
    molid = "";

    // create command
    CClientCommand* p_command = CreateCommand(Operation_GetVSData);
    if(p_command == NULL) return(false);

    // set client ID
    CXMLElement* p_ele = p_command->GetRootCommandElement();
    if(p_ele == NULL) {
        ES_ERROR("unable to get root command element");
        delete p_command;
        return(false);
    }

    CSmallString  client_id("-1");

    if(ActionRequest.GetParameterKeyValue("id",client_id) == false) {
        ES_ERROR("unable to get client id from command specification");
        delete p_command;
        return(false);
    }

    // is client_id integer?
    if(client_id.IsInt() == false) {
        CSmallString error;
        error << "specified client id '" << client_id << "' is not an integer number";
        ES_ERROR(error);
        delete p_command;
        return(false);
    }

    CSmallString str_type;
    if(ActionRequest.GetParameterKeyValue("type",str_type) == false) {
        str_type="pdbqt";
    }

    bool result = true;
    p_ele->SetAttribute("client_id",client_id);
    p_ele->SetAttribute("type",str_type);

    // optional
    CSmallString file_name;
    p_ele->SetAttribute("structure",ActionRequest.GetParameterKeyValue("structure",file_name));

    if(result == false) {
        ES_ERROR("unable to set client_id and/or type");
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

    // save structure ------------------------------------------------

    CXMLElement* p_rele = p_command->GetRootResultElement();
    if(p_rele == NULL) {
        ES_ERROR("unable to get root result element");
        delete p_command;
        return(false);
    }

    if(p_rele->GetAttribute("molid",molid) == false) {
        ES_ERROR("unable to get molid status");
        delete p_command;
        return(false);
    }

    if(molid == "eof") {
        delete p_command;
        return(true);
    }

    if(ReadStructure(p_rele) == false) {
        ES_ERROR("unable to read structure");
        delete p_command;
        return(false);
    }

    delete p_command;

    return(true);
}

//------------------------------------------------------------------------------

bool CVSClient::LoadStructure(void)
{
    // create command
    CClientCommand* p_command = CreateCommand(Operation_LoadStructure);
    if(p_command == NULL) return(false);

    // set client ID
    CXMLElement* p_ele = p_command->GetRootCommandElement();
    if(p_ele == NULL) {
        ES_ERROR("unable to get root command element");
        delete p_command;
        return(false);
    }

    CSmallString  client_id("-1");

    if(ActionRequest.GetParameterKeyValue("id",client_id) == false) {
        ES_ERROR("unable to get client id from command specification");
        delete p_command;
        return(false);
    }

    // is client_id integer?
    if(client_id.IsInt() == false) {
        CSmallString error;
        error << "specified client id '" << client_id << "' is not an integer number";
        ES_ERROR(error);
        delete p_command;
        return(false);
    }
    CSmallString    molid = "";
    if(ActionRequest.GetParameterKeyValue("molid",molid) == false) {
        ES_ERROR("unable to get molid from command specification");
        delete p_command;
        return(false);
    }

    CSmallString str_type;
    if(ActionRequest.GetParameterKeyValue("type",str_type) == false) {
        str_type="pdbqt";
    }

    bool result = true;
    p_ele->SetAttribute("client_id",client_id);
    p_ele->SetAttribute("molid",molid);
    p_ele->SetAttribute("type",str_type);

    // optional
    CSmallString file_name;
    p_ele->SetAttribute("structure",ActionRequest.GetParameterKeyValue("structure",file_name));

    // FIXME - try/catch
    ExecuteCommand(p_command);

    if(result == false) {
        ES_ERROR("unable to execute command");
        delete p_command;
        return(false);
    }

    // load structure ------------------------------------------------

    CXMLElement* p_rele = p_command->GetRootResultElement();
    if(p_rele == NULL) {
        ES_ERROR("unable to get root result element");
        delete p_command;
        return(false);
    }

    if(ReadStructure(p_rele) == false) {
        ES_ERROR("unable to read structure");
        delete p_command;
        return(false);
    }

    delete p_command;

    return(true);
}

//------------------------------------------------------------------------------

bool CVSClient::ReadStructure(CXMLElement* p_root)
{
    CSmallString file_name;

    // optional
    if(ActionRequest.GetParameterKeyValue("structure",file_name) == false) {
        return(true);
    }

    CXMLBinData* p_sele = p_root->GetFirstChildBinData("STRUCTURE");
    if(p_sele == NULL) {
        ES_ERROR("unable to get final STRUCTURE bin element");
        return(false);
    }

    FILE* p_fout = fopen(file_name,"wb");
    if(p_fout == NULL) {
        CSmallString error;
        error << "unable to open molecule file (" << file_name << ")";
        ES_ERROR(error);
        return(false);
    }

    fwrite(p_sele->GetData(),p_sele->GetLength(),sizeof(char),p_fout);
    fclose(p_fout);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
