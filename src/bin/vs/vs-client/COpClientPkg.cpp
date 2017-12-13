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
#include <FileSystem.hpp>
#include <XMLBinData.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSClient::InstallClientPkg(void)
{
    // create command
    CClientCommand* p_command = CreateCommand(Operation_InstallPkg);
    if(p_command == NULL) return(false);

    // execute
    bool result = true;

    // FIXME - try/catch
    ExecuteCommand(p_command);

    if(result == false) {
        ES_ERROR("unable to execute command");
        delete p_command;
        return(false);
    }

    // create archive
    CXMLElement* p_res = p_command->GetRootResultElement();
    if(p_res == NULL) return(false);

    CXMLElement* p_archive = p_res->GetFirstChildElement("DIRECTORY");
    if(p_archive == NULL) {
        ES_ERROR("unable to find archive");
        delete p_command;
        return(false);
    }

    CSmallString current_dir;
    if(CFileSystem::GetCurrentDir(current_dir) == false) {
        ES_ERROR("unable to get current directory");
        delete p_command;
        return(false);
    }

    if(ExtractArchive(p_archive,current_dir) == false) {
        ES_ERROR("unable to extract archive");
        delete p_command;
        return(false);
    }

    delete p_command;

    return(true);
}

//------------------------------------------------------------------------------

bool CVSClient::ExtractArchive(CXMLElement* p_direle,const CFileName& root_path)
{
    if(p_direle->GetName() != "DIRECTORY") {
        ES_ERROR("archive node mismatch");
        return(false);
    }

    CSmallString dir_name;

    if(p_direle->GetAttribute("name",dir_name) == false) {
        ES_ERROR("unable to get directory name");
        return(false);
    }

    // create directory
    if(CFileSystem::CreateDir(root_path / dir_name) == false) {
        ES_ERROR("unable to create directory");
        return(false);
    }

    // set mode
    int mode = 0;
    if(p_direle->GetAttribute("mode",mode) == false) {
        ES_ERROR("unable to get dir mode");
        return(false);
    }
    if(CFileSystem::SetPosixMode(root_path / dir_name,mode) == false) {
        ES_ERROR("unable to set dir mode");
        return(false);
    }

    // enum archive
    CXMLNode* p_node = p_direle->GetFirstChildNode();
    while(p_node != NULL) {
        if(p_node->GetNodeType() == EXNT_BIN_DATA) {
            if(ExtractFile(static_cast<CXMLBinData*>(p_node),root_path / dir_name) == false) return(false);
        } else {
            if(p_node->GetNodeType() == EXNT_ELEMENT) {
                if(ExtractArchive(static_cast<CXMLElement*>(p_node),root_path / dir_name) == false) return(false);
            }
        }
        p_node = p_node->GetNextSiblingNode();
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CVSClient::ExtractFile(CXMLBinData* p_fileele,const CFileName& root_path)
{
    if(p_fileele->GetName() != "FILE") {
        ES_ERROR("archive node mismatch");
        return(false);
    }

    CSmallString file_name;

    if(p_fileele->GetAttribute("name",file_name) == false) {
        ES_ERROR("unable to get file name");
        return(false);
    }

    FILE* p_fout = fopen(root_path/file_name,"wb");
    if(p_fout == NULL) {
        CSmallString error;
        error << "unable to open file (" << file_name << ")";
        ES_ERROR(error);
        return(false);
    }

    fwrite(p_fileele->GetData(),p_fileele->GetLength(),sizeof(char),p_fout);
    fclose(p_fout);

    // set mode
    int mode = 0;
    if(p_fileele->GetAttribute("mode",mode) == false) {
        ES_ERROR("unable to get file mode");
        return(false);
    }
    if(CFileSystem::SetPosixMode(root_path/file_name,mode) == false) {
        ES_ERROR("unable to set file mode");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CVSClient::GetClientAppName(bool& silent_error)
{
    silent_error = false;

    // create command
    CClientCommand* p_command = CreateCommand(Operation_GetAppName);
    if(p_command == NULL) return(false);

    // execute
    bool result = true;

    // FIXME - try/catch
    ExecuteCommand(p_command);

    if(result == false) {
        ES_ERROR("unable to execute command");
        delete p_command;
        return(false);
    }

    bool noappname = false;
    CXMLElement* p_res = p_command->GetRootResultElement();
    if(p_res == NULL) return(false);

    p_res->GetAttribute("noappname",noappname);
    if(noappname == true) {
        silent_error = true;
    } else {
        CSmallString appname;
        p_res->GetAttribute("appname",appname);

        if(Options.GetOptVerbose() == true) {
            printf("Application name: %s\n",(const char*)appname);
        } else {
            printf("%s",(const char*)appname);
        }
    }

    delete p_command;

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


