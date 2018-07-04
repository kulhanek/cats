// =============================================================================
// PMFLib - Library Supporting Potential of Mean Force Calculations
// -----------------------------------------------------------------------------
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
// =============================================================================

#include <stdio.h>
#include <ErrorSystem.hpp>
#include <RegClient.hpp>
#include "RSProcessor.hpp"
#include "RSServer.hpp"
#include <XMLElement.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CRSProcessor::WriteData(void)
{
    int client_id = -1;

    // get client ID --------------------------------
    if( CommandElement->GetAttribute("client_id",client_id) == false ) {
        ES_ERROR("unable to get client_id");
        return(false);
    }

    CRegClient* p_client = RSServer.RegClients.FindClient(client_id);

    if( p_client == NULL ) {
        CSmallString error;
        error << "unable to find client with id " << client_id;
        ES_ERROR(error);
        return(false);
    }

    if( p_client->GetClientStatus() != ERCS_REGISTERED ) {
        CSmallString error;
        error << "client " << client_id << " is not active state";
        ES_ERROR(error);
        return(false);
    }

    // write data -----------------------------------
    CXMLElement* p_cele = CommandElement->GetFirstChildElement("DATA");
    if( p_cele == NULL ) {
        ES_ERROR("unable to open DATA element");
        return(false);
    }

    // write results
    if( RSServer.ResultFile.WriteData(p_cele,client_id) == false ) {
        ES_ERROR("unable to write results");
        return(false);
    }

    // register operation
    p_client->RegisterOperation();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
