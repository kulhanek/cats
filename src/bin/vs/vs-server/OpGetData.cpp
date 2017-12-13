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

    VSServer.SelMutex.Lock();

    if(VSServer.CommitSelTrans() == false) {
        VSServer.SelMutex.Unlock();
        return(false);
    }

    if(VSServer.SelQuery.QueryRecord() == false) {
        ResultElement->SetAttribute("molid","eof");     // no more structures
        VSServer.SelMutex.Unlock();
        return(true);
    }

    if(VSServer.SelQuery.GetOutputItem(0) == NULL) {
        CMD_ERROR(Command,"unable to get output item of query");
        VSServer.SelMutex.Unlock();
        return(false);
    }

    CSmallString molid;

    molid = VSServer.SelQuery.GetOutputItem(0)->GetString();

    // get template element (optional)
    CXMLElement* p_template = CommandElement->GetFirstChildElement("TEMPLATE");
    CXMLElement* p_data = NULL;

    if(p_template != NULL) {
        p_data = ResultElement->CreateChildElement("DATA");
        if(p_data == NULL) {
            CMD_ERROR(Command,"unable to create DATA element");
            VSServer.SelMutex.Unlock();
            return(false);
        }
    }

    CXMLIterator    I(p_template);
    CSmallString    key;
    int             colid;

    while(I.GetNextAttributeName(key) != false) {
        colid = -1;

        // check column name
        if(key == "ID") {
            CSmallString value = VSServer.SelQuery.GetOutputItem(0)->GetString();
            p_data->SetAttribute(key,value);
            continue;
        }
        if(key == "InChiKey") {
            CSmallString value = VSServer.SelQuery.GetOutputItem(1)->GetString();
            p_data->SetAttribute(key,value);
            continue;
        }
        if(key == "TC") {
            int value = VSServer.SelQuery.GetOutputItem(2)->GetInt();
            p_data->SetAttribute(key,value);
            continue;
        }
        if(key == "MW") {
            double value = VSServer.SelQuery.GetOutputItem(3)->GetDouble();
            p_data->SetAttribute(key,value);
            continue;
        }
        if(key == "NA") {
            int value = VSServer.SelQuery.GetOutputItem(4)->GetInt();
            p_data->SetAttribute(key,value);
            continue;
        }

        // dynamic properties
        CSmallString props2(VSServer.DynamicProperties);
        char*   p_temp = NULL;
        int     n=0;
        if(props2 != NULL) {
            char* p_str = strtok_r(props2.GetBuffer(),",",&p_temp);
            while(p_str != NULL) {
                if(p_str == key) {
                    colid = n + 5;
                }
                n++;
                p_str = strtok_r(NULL,",",&p_temp);
            }
        }

        // results
        for(int i=0; i < VSServer.NumOfResults; i++) {
            CSmallString rkey;
            rkey << "R" << i+1;

            if(rkey == key) {
                colid = i + 5 + n;
            }
        }

        if(colid == -1) {
            CSmallString error;
            error << "unknown static/dynamic property or result item (" << key << ")";
            CMD_ERROR(Command,error);
            VSServer.SelMutex.Unlock();
            return(false);
        }
        double value = VSServer.SelQuery.GetOutputItem(colid)->GetDouble();
        p_data->SetAttribute(key,value);

    }

    VSServer.SelMutex.Unlock();


    VSServer.ManipMutex.Lock();

    if(VSServer.ManipTransaction.StartTransaction() == false) {
        CMD_ERROR(Command,"unable to start transaction");
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

    // change item flag
    CSmallString sql;
    sql << "UPDATE \"" <<  VSServer.ProjectName << "\" SET \"Flag\" = 1 WHERE \"ID\" = ?";

    if(exsql.AllocateInputItems(1) == false) {
        CMD_ERROR(Command,"unable to allocate items in exsql");
        VSServer.ManipTransaction.RollbackTransaction();
        VSServer.ManipMutex.Unlock();
        return(false);
    }

    exsql.GetInputItem(0)->SetString(molid);

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
