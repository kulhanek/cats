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
#include "RSProcessor.h"
#include "RSServer.h"
#include <XMLElement.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CRSProcessor::RegisterClient(void)
{
    CXMLElement* p_cele = CommandElement->GetFirstChildElement("TEMPLATE");
    if( p_cele == NULL ){
        ES_ERROR("unable to open TEMPLATE element");
        return(false);
    }

    // register result items -----------------------
    if( RSServer.ResultFile.LoadOrCheckTemplate(p_cele) == false ){
        ES_ERROR("unable to register or check data template");
        return(false);
    }

    // register client -----------------------------
    CRegClient* p_rc;

    // create CRSRegClient
    try{
        p_rc = new CRegClient;
    }
    catch(...){
        ES_ERROR("unable to create CRSRegClient");
        return(false);
    }

    //  and register it to the list
    if( RSServer.RegClients.RegisterClient(p_rc) == false ){
        ES_ERROR("unable to register client");
        return(false);
    }

    // get client ID
    int client_id = p_rc->GetClientID();

    // write response
    ResultElement->SetAttribute("client_id",client_id);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
