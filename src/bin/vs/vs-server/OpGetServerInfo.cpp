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
#include <ErrorSystem.h>
#include <XMLElement.h>
#include "VSProcessor.h"
#include "VSServer.h"
#include <XMLElement.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSProcessor::GetServerInfo(void)
{
// basic info is about number of transactions
    bool result = true;

    result &= ResultElement->SetAttribute("pt",VSServer.GetNumberOfTransactions());
    result &= ResultElement->SetAttribute("it",VSServer.GetNumberOfIllegalTransactions());

    if(result == false) {
        ErrorSystem.AddError("unable to set server basic info","CVSProcessor::GetServerInfo");
        return(false);
    }

// write info about clients
    if(VSServer.RegClients.SaveInfo(ResultElement) == false) {
        ErrorSystem.AddError("unable to save info about clients","CVSProcessor::GetServerInfo");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

