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
#include <XMLBinData.hpp>
#include <XMLIterator.hpp>
#include <ServerCommand.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSProcessor::WriteData(void)
{
    int client_id = -1;
    int molid;

    // get client ID --------------------------------
    if(CommandElement->GetAttribute("client_id",client_id) == false) {
        CMD_ERROR(Command,"unable to get client_id");
        return(false);
    }

    if(CommandElement->GetAttribute("molid",molid) == false) {
        CMD_ERROR(Command,"unable to get molid");
        return(false);
    }

    CRegClient* p_client = VSServer.RegClients.FindClient(client_id);

    if(p_client == NULL) {
        CSmallString error;
        error << "unable to find client with id " << client_id;
        CMD_ERROR(Command,error);
        return(false);
    }

    if(p_client->GetClientStatus() != ERCS_REGISTERED) {
        CSmallString error;
        error << "client " << client_id << " is not in active state";
        CMD_ERROR(Command,error);
        return(false);
    }

    //*********** LOCKED DATABASE ACCESS ******************

    VSServer.DBMutex.Lock();

    // update FLAG to 2
    sqlite3_reset(VSServer.P2STM);
    sqlite3_bind_int(VSServer.P2STM,1,molid);

    if( sqlite3_step(VSServer.P2STM) != SQLITE_DONE ){
        CMD_ERROR(Command,"unable to update FLAG to 2");
        VSServer.DBMutex.Unlock();
        return(false);
    }

    VSServer.DBMutex.Unlock();

    //*********** END OF LOCKED DATABASE ACCESS ******************

    // register successfull operation
    p_client->RegisterOperation();

    // write structure if it is present
    CXMLBinData* p_sele = CommandElement->GetFirstChildBinData("STRUCTURE");
    if(p_sele != NULL) {
        CSmallString str_type;
        str_type = "rpdbqt";
        CommandElement->GetAttribute("type",str_type);

        CFileName molname(VSServer.StructureDir);

        CSmallString smolid;
        smolid.IntToStr(molid,"%08d");

        if(VSServer.UseHiearchy) {
            molname = molname / CFileName(smolid.GetSubString(0,2)) / CFileName(smolid.GetSubString(2,2))
                      / CFileName(smolid.GetSubString(4,2));
        }

        molname = molname / VSServer.UnisID + smolid + "." + str_type;

        FILE* p_fout = fopen(molname,"wb");
        if(p_fout == NULL) {
            CSmallString error;
            error << "unable to open molecule file (" << molname << ")";
            CMD_ERROR(Command,error);
            return(false);
        }

        fwrite(p_sele->GetData(),p_sele->GetLength(),sizeof(char),p_fout);
        fclose(p_fout);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CVSProcessor::SaveStructure(void)
{
    int client_id = -1;
    CSmallString molid;

    // get client ID --------------------------------
    if(CommandElement->GetAttribute("client_id",client_id) == false) {
        CMD_ERROR(Command,"unable to get client_id");
        return(false);
    }

    if(CommandElement->GetAttribute("molid",molid) == false) {
        CMD_ERROR(Command,"unable to get molid");
        return(false);
    }

    CRegClient* p_client = VSServer.RegClients.FindClient(client_id);

    if(p_client == NULL) {
        CSmallString error;
        error << "unable to find client with id " << client_id;
        CMD_ERROR(Command,error);
        return(false);
    }

    if(p_client->GetClientStatus() != ERCS_REGISTERED) {
        CSmallString error;
        error << "client " << client_id << " is not in active state";
        CMD_ERROR(Command,error);
        return(false);
    }

    // register successfull operation
    p_client->RegisterOperation();

    // write structure if it is present
    CXMLBinData* p_sele = CommandElement->GetFirstChildBinData("STRUCTURE");
    if(p_sele != NULL) {
        CSmallString str_type;
        str_type = "rpdbqt";
        CommandElement->GetAttribute("type",str_type);

        CFileName molname(VSServer.StructureDir);

        if(VSServer.UseHiearchy) {
            molname = molname / CFileName(molid.GetSubString(0,2)) / CFileName(molid.GetSubString(2,2))
                      / CFileName(molid.GetSubString(4,2));
        }

        molname = molname / VSServer.UnisID + molid + "." + str_type;

        FILE* p_fout = fopen(molname,"wb");
        if(p_fout == NULL) {
            CSmallString error;
            error << "unable to open molecule file (" << molname << ")";
            CMD_ERROR(Command,error);
            return(false);
        }

        fwrite(p_sele->GetData(),p_sele->GetLength(),sizeof(char),p_fout);
        fclose(p_fout);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
