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
#include <TrajectoryClient.hpp>
#include <CATsOperation.hpp>
#include <ErrorSystem.hpp>
#include <ClientCommand.hpp>
#include <ResultFile.hpp>
#include <AmberTopology.hpp>
#include <AmberRestart.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTrajectoryClient::GetSnapshot(int client_id,CAmberRestart* p_rst,
        const CSmallString& snapop,bool read_vel)
{
    // create command
    CClientCommand* p_command = CreateCommand(Operation_GetSnapshot);
    if( p_command == NULL ) return(false);

    // set client ID
    CXMLElement* p_ele = p_command->GetRootCommandElement();
    if( p_ele == NULL ) {
        ES_ERROR("unable to get root command element");
        delete p_command;
        return(-1);
    }

    p_ele->SetAttribute("client_id",client_id);
    p_ele->SetAttribute("snapshot_id",snapop);

    if( p_ele == NULL ) {
        ES_ERROR("unable to set client_id and/or snapshot_id");
        delete p_command;
        return(-1);
    }

    // we do not need to send any data so directly execute command
    //FIXME
    try {
        ExecuteCommand(p_command);

    } catch(...) {
        ES_ERROR("unable to execute command");
        delete p_command;
        return(-1);
    }


    // get total status
    CXMLElement* p_rele = p_command->GetRootResultElement();
    if( p_rele == NULL ) {
        ES_ERROR("unable to get root result element");
        delete p_command;
        return(-1);
    }

    CSmallString status;
    int          snapshot_index = 0;

    if( p_rele->GetAttribute("status",status) == false ) {
        ES_ERROR("unable to get final status");
        delete p_command;
        return(-1);
    }

    if( status == "eof" ) {
        return(snapshot_index);
    }

    CXMLElement* p_sele = p_command->GetResultElementByPath("SNAPSHOT",false);
    if( p_sele == NULL ) {
        ES_ERROR("unable to get final SNAPSHOT");
        delete p_command;
        return(-1);
    }

    // load data
    if( read_vel ) {
        // load velocities
        if( p_rst->LoadVelocities(p_sele) == false ) {
            ES_ERROR("unable to load snapshot velocities");
            delete p_command;
            return(-1);
        }
    } else {
        // load data
        if( p_rst->LoadSnapshot(p_sele) == false ) {
            ES_ERROR("unable to load snapshot coordinates");
            delete p_command;
            return(-1);
        }
    }

    if( p_rele->GetAttribute("index",snapshot_index) == false ) {
        ES_ERROR("unable to get snapshot index");
        delete p_command;
        return(-1);
    }

    delete p_command;

    return(snapshot_index);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
