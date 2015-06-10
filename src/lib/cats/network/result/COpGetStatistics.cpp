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

bool CResultClient::GetStatistics(const CSmallString& stat_name)
{
    CClientCommand* p_command = CreateCommand(Operation_GetStatistics);
    if( p_command == NULL ) return(false);

    // we do not need to send any data so directly execute command
    bool result = false;
    //FIXME
    //= ExecuteCommand(p_command);

    if( result == false ) {
        ES_ERROR("unable to execute command");
        delete p_command;
        return(false);
    }

    // get STAT element
    CXMLElement* p_rele = p_command->GetResultElementByPath("STAT",true);
    if( p_rele == NULL ) {
        ES_ERROR("unable to get STAT result element");
        delete p_command;
        return(false);
    }

    CResultFile result_file;

    if( result_file.LoadStatistics(p_rele) == false ) {
        ES_ERROR("unable to load statistics");
        delete p_command;
        return(false);
    }

    result_file.SetStatisticsFileName(stat_name);

    if( result_file.WriteStatistics() == false ) {
        ES_ERROR("unable to write statistics to a file");
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
