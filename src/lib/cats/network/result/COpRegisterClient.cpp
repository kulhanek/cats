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
#include <ExtraOperation.hpp>
#include <ErrorSystem.hpp>
#include <ClientCommand.hpp>
#include <ResultFile.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CResultClient::RegisterClient(const CSmallString& template_name, int& client_id)
{
    CResultFile result_file;

    if( result_file.LoadTemplate(template_name,false) == false ){
        ES_ERROR("unable to load template");
        return(false);
    }

    CClientCommand* p_command = CreateCommand(Operation_RegisterClient);
    if( p_command == NULL ) return(false);

    CXMLElement* p_cele = p_command->GetCommandElementByPath("TEMPLATE",true);
    if( p_cele == NULL ){
        ES_ERROR("unable to create TEMPLATE element");
        delete p_command;
        return(false);
    }

    if( result_file.SaveTemplate(p_cele) == false ){
        ES_ERROR("unable to save template");
        delete p_command;
        return(false);
    }

    //FIXME
    //bool result = ExecuteCommand(p_command);

    bool result = false;
    if( result == false ){
        ES_ERROR("unable to execute command");
        delete p_command;
        return(false);
    }

    // get client ID and return
    CXMLElement* p_rele = p_command->GetRootResultElement();
    if( p_rele == NULL ){
        ES_ERROR("unable to get root result element");
        delete p_command;
        return(false);
    }

    client_id = -1;

    result = true;
    result &= p_rele->GetAttribute("client_id",client_id);

    if( result == false ){
        ES_ERROR("unable to get client_id");
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
