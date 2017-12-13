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

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSClient::SelReset(void)
{
    // create command
    CClientCommand* p_command = CreateCommand(Operation_SelReset);
    if(p_command == NULL) return(false);

    // execute
    // FIXME - try/catch
    ExecuteCommand(p_command);

    bool result = true;
    if(result == false) {
        ES_ERROR("unable to execute command");
        delete p_command;
        return(false);
    }

    delete p_command;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

