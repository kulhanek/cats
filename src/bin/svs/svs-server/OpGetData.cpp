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
#include <ServerCommand.hpp>
#include <XMLIterator.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSProcessor::GetData(void)
{
    int client_id = -1;
    CSmallString str_type;

    // get client ID --------------------------------
    if(CommandElement->GetAttribute("client_id",client_id) == false) {
        CMD_ERROR(Command,"unable to get client_id");
        return(false);
    }

    if(CommandElement->GetAttribute("type",str_type) == false) {
        CMD_ERROR(Command,"unable to get structure type");
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


    bool result = true;

    //*********** LOCKED DATABASE ACCESS ******************

    VSServer.DBMutex.Lock();

    // select new item
    sqlite3_reset(VSServer.SelectSTM);
    int rcode = sqlite3_step(VSServer.SelectSTM);
    if( rcode != SQLITE_ROW ){
        if( rcode != SQLITE_DONE ){
            ES_ERROR(sqlite3_errmsg(VSServer.SqlDB));
        }
        ResultElement->SetAttribute("molid","eof");     // no more structures
        VSServer.DBMutex.Unlock();
        return(true);
    }

    // get molid
    int molid = sqlite3_column_int(VSServer.SelectSTM,0);

    // update FLAG to 1
    sqlite3_reset(VSServer.P1STM);

    CSmallString smolid;
    smolid.IntToStr(molid,"%08d");

    rcode =  sqlite3_bind_text(VSServer.P1STM,1,smolid,-1,SQLITE_TRANSIENT);

    rcode = sqlite3_step(VSServer.P1STM);
    if( rcode != SQLITE_DONE ){
        CMD_ERROR(Command,"unable to update FLAG to 1");
        VSServer.DBMutex.Unlock();
        return(false);
    }

    VSServer.DBMutex.Unlock();

    //*********** END OF LOCKED DATABASE ACCESS ******************

    ResultElement->SetAttribute("molid",molid);

    bool send_str = false;
    CommandElement->GetAttribute("structure",send_str);
    if(send_str == false) {
        // register successfull operation
        p_client->RegisterOperation();
        return(true);    // do not send structure
    }

    // now we can download file into XML
    CXMLBinData* p_moldata = ResultElement->CreateChildBinData("STRUCTURE");
    if(p_moldata == NULL) {
        CMD_ERROR(Command,"unable to create STRUCTURE element");
        return(false);
    }

    CFileName molname(VSServer.StructureDir);

    if(VSServer.UseHiearchy) {
        molname = molname / CFileName(smolid.GetSubString(0,2)) / CFileName(smolid.GetSubString(2,2))
                  / CFileName(smolid.GetSubString(4,2));
    }

    molname = molname / VSServer.UnisID + smolid + "." + str_type;

    FILE* p_fin = fopen(molname,"rb");
    if(p_fin == NULL) {
        CSmallString error;
        error << "unable to open molecule file (" << molname << ")";
        CMD_ERROR(Command,error);
        return(false);
    }

    fseek(p_fin,0,SEEK_END);
    long molfilelen=ftell(p_fin);
    fseek(p_fin,0,SEEK_SET);

    p_moldata->SetLength(molfilelen,EXBDT_CHAR);

    fread(p_moldata->GetData(),molfilelen,sizeof(char),p_fin);
    fclose(p_fin);

    // register successfull operation
    p_client->RegisterOperation();

    return(result);
}

//------------------------------------------------------------------------------

bool CVSProcessor::LoadStructure(void)
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

    CSmallString str_type;
    if(CommandElement->GetAttribute("type",str_type) == false) {
        CMD_ERROR(Command,"unable to get structure type");
        return(false);
    }

    // now we can download file into XML
    CXMLBinData* p_moldata = ResultElement->CreateChildBinData("STRUCTURE");
    if(p_moldata == NULL) {
        CMD_ERROR(Command,"unable to create STRUCTURE element");
        return(false);
    }

    CFileName molname(VSServer.StructureDir);

    if(VSServer.UseHiearchy) {
        molname = molname / CFileName(molid.GetSubString(0,2)) / CFileName(molid.GetSubString(2,2))
                  / CFileName(molid.GetSubString(4,2));
    }

    molname = molname / VSServer.UnisID + molid + "." + str_type;

    FILE* p_fin = fopen(molname,"rb");
    if(p_fin == NULL) {
        CSmallString error;
        error << "unable to open molecule file (" << molname << ")";
        CMD_ERROR(Command,error);
        return(false);
    }

    fseek(p_fin,0,SEEK_END);
    long molfilelen=ftell(p_fin);
    fseek(p_fin,0,SEEK_SET);

    p_moldata->SetLength(molfilelen,EXBDT_CHAR);

    fread(p_moldata->GetData(),molfilelen,sizeof(char),p_fin);
    fclose(p_fin);

    // register successfull operation
    p_client->RegisterOperation();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
