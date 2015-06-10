// =============================================================================
// CATs - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu
//    Copyright (C) 2005 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include "ResultClient.hpp"
#include <CATsOperation.hpp>
#include <ErrorSystem.hpp>
#include <ClientCommand.hpp>
#include <ResultFile.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CResultClient::WriteData(int client_id,const CSmallString& data_name)
{
    CClientCommand* p_command = CreateCommand(Operation_WriteData);
    if( p_command == NULL ) return(false);

    // set client ID
    CXMLElement* p_ele = p_command->GetRootCommandElement();
    if( p_ele == NULL ) {
        ES_ERROR("unable to get root command element");
        delete p_command;
        return(false);
    }

    bool result = true;
    p_ele->SetAttribute("client_id",client_id);

    if( p_ele == NULL ) {
        ES_ERROR("unable to set client_id");
        delete p_command;
        return(false);
    }

    CResultFile result_file;

    if( result_file.ReadData(data_name,false) == false ) {
        ES_ERROR("unable to read data");
        return(false);
    }

    CXMLElement* p_cele = p_command->GetCommandElementByPath("DATA",true);
    if( p_cele == NULL ) {
        ES_ERROR("unable to create DATA element");
        delete p_command;
        return(false);
    }

    if( result_file.SaveData(p_cele) == false ) {
        ES_ERROR("unable to save data");
        delete p_command;
        return(false);
    }

    // send data and execute command
    //FIXME
    //result = ExecuteCommand(p_command);

    if( result == false ) {
        ES_ERROR("unable to execute command");
        delete p_command;
        return(false);
    }

    // release command
    delete p_command;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
