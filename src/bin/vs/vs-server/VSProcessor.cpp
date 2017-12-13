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
#include <VSOperation.hpp>
#include "VSProcessor.hpp"
#include "VSServer.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVSProcessor::CVSProcessor(CServerCommand* p_cmd)
    : CCmdProcessor(p_cmd)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSProcessor::ProcessCommand(void)
{
    if(Operation == Operation_GetData) {
        return(GetData());
    }
    if(Operation == Operation_WriteData) {
        return(WriteData());
    }
    if(Operation == Operation_LoadStructure) {
        return(LoadStructure());
    }
    if(Operation == Operation_SaveStructure) {
        return(SaveStructure());
    }
    if(Operation == Operation_SelReset) {
        return(SelReset());
    }
    if(Operation == Operation_InstallPkg) {
        return(InstallPkg());
    }
    if(Operation == Operation_GetAppName) {
        return(GetClientAppName());
    }

    CSmallString error;
    error << "operation " << Operation.GetStringForm() << " is not implemented";
    ES_ERROR(error);

    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
