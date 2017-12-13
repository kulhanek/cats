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
#include <XMLBinData.hpp>
#include <XMLIterator.hpp>
#include <ServerCommand.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSProcessor::WriteData(void)
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

    //*********** LOCKED DATABASE ACCESS ******************

    VSServer.ManipMutex.Lock();

    if(VSServer.ManipTransaction.StartTransaction() == false) {
        CMD_ERROR(Command,"unable to initiate transaction");
        VSServer.ManipMutex.Unlock();
        return(false);
    }

    CFirebirdExecuteSQL exsql;

    if(exsql.AssignToTransaction(&VSServer.ManipTransaction) == false) {
        CMD_ERROR(Command,"unable to initiate execsql statement");
        VSServer.ManipTransaction.RollbackTransaction();
        VSServer.ManipMutex.Unlock();
        return(false);
    }

    CXMLElement* p_results = CommandElement->GetFirstChildElement("RESULTS");
    if(p_results != NULL) {
        if(exsql.AllocateInputItems(1+p_results->NumOfAttributes()) == false) {
            CMD_ERROR(Command,"unable to allocate items in exsql");
            VSServer.ManipTransaction.RollbackTransaction();
            VSServer.ManipMutex.Unlock();
            return(false);
        }
    } else {
        if(exsql.AllocateInputItems(1) == false) {
            CMD_ERROR(Command,"unable to allocate items in exsql");
            VSServer.ManipTransaction.RollbackTransaction();
            VSServer.ManipMutex.Unlock();
            return(false);
        }
    }

    // write results
    CSmallString sql;
    sql << "UPDATE \"" <<  VSServer.ProjectName << "\" SET \"Flag\" = 2";

    CXMLIterator    I(p_results);
    CSmallString    key,value;
    int             i = 0;

    while(I.GetNextAttributeName(key) != false) {

        bool valid = true;
        // check column name
        if(key == "ID")           valid = false;
        if(key == "Flag")         valid = false;
        if(key == "InChiKey")     valid = false;
        if(key == "TC")           valid = false;
        if(key == "MW")           valid = false;
        if(key == "NA")           valid = false;

        if(valid == false) {
            CSmallString error;
            error << "attempt to change static column (" << key << ")";
            CMD_ERROR(Command,error);
            VSServer.ManipTransaction.RollbackTransaction();
            VSServer.ManipMutex.Unlock();
            return(false);
        }

        p_results->GetAttribute(key,value);
        sql << ",\"" << key << "\" = ?";

        exsql.GetInputItem(i)->SetString(value);
        i++;
    }

    sql << " WHERE \"ID\" = ?";
    exsql.GetInputItem(i)->SetString(molid);

    if(exsql.ExecuteSQL(sql) == false) {
        CMD_ERROR(Command,"unable to execute sql statement");
        VSServer.ManipTransaction.RollbackTransaction();
        VSServer.ManipMutex.Unlock();
        return(false);
    }

    if(VSServer.ManipTransaction.CommitTransaction() == false) {
        CMD_ERROR(Command,"unable to commit transaction");
        VSServer.ManipTransaction.RollbackTransaction();
        VSServer.ManipMutex.Unlock();
        return(false);
    }

    VSServer.ManipMutex.Unlock();

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
