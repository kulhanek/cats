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
#include <ResultClient.hpp>
#include <CATsOperation.hpp>
#include <ErrorSystem.hpp>
#include <ClientCommand.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CResultClient::FlushServerData(void)
{
    CClientCommand* p_command = CreateCommand(Operation_FlushServerData);
    if( p_command == NULL ) return(false);

    // we do not need to send any data so directly execute command
    bool result = false;
    //FIXME
    //= ExecuteCommand(p_command);

    if( result == false ){
        delete p_command;
        ES_ERROR("unable to execute command");
        return(false);
    }

    // release command
    delete p_command;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

