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
#include <ErrorSystem.hpp>
#include <RegClient.hpp>
#include "VSProcessor.hpp"
#include "VSServer.hpp"
#include <XMLElement.hpp>
#include <FirebirdQuerySQL.hpp>
#include <FirebirdExecuteSQL.hpp>
#include <FirebirdItem.hpp>
#include <ServerCommand.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSProcessor::SelReset(void)
{
    //*********** LOCKED DATABASE ACCESS ******************

    VSServer.SelMutex.Lock();

    if(VSServer.CommitSelTrans(true) == false) {
        VSServer.SelMutex.Unlock();
        return(false);
    }

    VSServer.SelMutex.Unlock();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

