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
#include <DirectoryEnum.hpp>
#include <FileSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSProcessor::InstallPkg(void)
{
    CFileName app_dir = VSServer.ClientPackageDir;
    CFileName root_dir = app_dir.GetFileDirectory();
    CFileName dir_name = app_dir.GetFileName();

    if(root_dir == NULL) {
        if(CFileSystem::GetCurrentDir(root_dir) == false) {
            CMD_ERROR(Command,"unable to get current directory");
            return(false);
        }
    }

    // create archive
    bool result = CreateArchive(ResultElement,root_dir,dir_name);

    return(result);
}

//------------------------------------------------------------------------------

bool CVSProcessor::CreateArchive(CXMLElement* p_root,const CFileName& root_path,const CFileName& dir_name)
{
    CXMLElement* p_direle = p_root->CreateChildElement("DIRECTORY");
    if(p_direle == NULL) {
        CMD_ERROR(Command,"unable to create directory element");
        return(false);
    }

    p_direle->SetAttribute("name",dir_name);

    p_direle->SetAttribute("mode",CFileSystem::GetPosixMode(root_path/dir_name));

    // enumerate given directory
    CDirectoryEnum direnum(root_path/dir_name);

    direnum.StartFindFile("*");

    CFileName file;
    while(direnum.FindFile(file)) {
        if(file == "..") continue;
        if(file == ".") continue;
        if(CFileSystem::IsDirectory(root_path/dir_name/file)) {
            if(CreateArchive(p_direle,root_path/dir_name,file) == false) return(false);
        } else {
            if(CreateFile(p_direle,root_path/dir_name,file) == false) return(false);
        }
    }

    //! close file search
    direnum.EndFindFile();

    return(true);
}

//------------------------------------------------------------------------------

bool CVSProcessor::CreateFile(CXMLElement* p_root,const CFileName& root_path,const CFileName& file_name)
{
    CXMLBinData* p_fileele = p_root->CreateChildBinData("FILE");
    if(p_fileele == NULL) {
        CMD_ERROR(Command,"unable to create FILE element");
        return(false);
    }

    p_fileele->SetAttribute("name",file_name);

    p_fileele->SetAttribute("mode",CFileSystem::GetPosixMode(root_path/file_name)) ;

    FILE* p_fin = fopen(root_path/file_name,"rb");
    if(p_fin == NULL) {
        CSmallString error;
        error << "unable to open file (" << file_name << ")";
        CMD_ERROR(Command,error);
        return(false);
    }

    fseek(p_fin,0,SEEK_END);
    long molfilelen=ftell(p_fin);
    fseek(p_fin,0,SEEK_SET);

    p_fileele->SetLength(molfilelen,EXBDT_CHAR);

    fread(p_fileele->GetData(),molfilelen,sizeof(char),p_fin);
    fclose(p_fin);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVSProcessor::GetClientAppName(void)
{
    CFileName appname = VSServer.ClientPackageDir;
    appname = appname.GetFileName() / VSServer.ClientAppName;

    bool result = true;
    if(VSServer.ClientPackageAvailable) {
        ResultElement->SetAttribute("appname",appname);
    } else {
        ResultElement->SetAttribute("noappname",true);
    }
    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


