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
#include "TSProcessor.hpp"
#include "TSServer.hpp"
#include <XMLElement.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTSProcessor::GetSnapshot(void)
{
    int client_id = -1;
    CSmallString snapshot_id;

    // get client ID --------------------------------
    if( CommandElement->GetAttribute("client_id",client_id) == false ) {
        ES_ERROR("unable to get client_id");
        return(false);
    }

    if( CommandElement->GetAttribute("snapshot_id",snapshot_id) == false ) {
        ES_ERROR("unable to get snapshot_id");
        return(false);
    }

    CRegClient* p_client = TSServer.RegClients.FindClient(client_id);

    if( p_client == NULL ) {
        CSmallString error;
        error << "unable to find client with id " << client_id;
        ES_ERROR(error);
        return(false);
    }

    if( p_client->GetClientStatus() != ERCS_REGISTERED ) {
        CSmallString error;
        error << "client " << client_id << " is not in active state";
        ES_ERROR(error);
        return(false);
    }

    if( snapshot_id != "next" ) {
        CSmallString error;
        error << "unsupported snapshot id '" << snapshot_id << "'";
        ES_ERROR(error);
        return(false);
    }

    bool result = true;

    // lock access to trajectory
    TSServer.TrajectoryMutex.Lock();

    if( TSServer.ReadSnapshot() ) {
        TSServer.SnapshotIndex++;

        // write data
        CXMLElement* p_sele = ResultElement->CreateChildElement("SNAPSHOT");
        if( p_sele == NULL ) {
            ES_ERROR("unable to create SNAPSHOT element");
            result = false;
        }

        if( result == true ) {
            // save data
            TSServer.Snapshot.SaveSnapshot(p_sele);
        }

        if( result == true ) {
            ResultElement->SetAttribute("status","ok");
        }

        if( result == true ) {
            ResultElement->SetAttribute("index",TSServer.SnapshotIndex);
        }

        // register operation
        if( result != false ) p_client->RegisterOperation();

    } else {
        ResultElement->SetAttribute("status","eof");
    }

    TSServer.TrajectoryMutex.Unlock();

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
