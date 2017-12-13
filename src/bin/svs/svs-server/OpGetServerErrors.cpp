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
#include "VSProcessor.h"
#include "VSServer.h"
#include <XMLElement.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSProcessor::GetServerErrors(void)
{
    CXMLElement* p_ele = ResultElement->CreateChildElement("ERRORS");
    if(p_ele == NULL) {
        ErrorSystem.AddError("unable to create ERRORS element","CVSProcessor::GetServerErrors");
        return(false);
    }

// make copy of local error stack
    if(ErrorSystem.CopyErrors(p_ele) == false) {
        ErrorSystem.AddError("unable to copy errors","CVSProcessor::GetServerErrors");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

