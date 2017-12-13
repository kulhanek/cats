#ifndef VSProcessorH
#define VSProcessorH
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

#include <CmdProcessor.hpp>
#include <FileName.hpp>

//------------------------------------------------------------------------------

class CVSProcessor : public CCmdProcessor {
public:
    // constructor
    CVSProcessor(CServerCommand* p_cmd);

// section of private data ----------------------------------------------------
private:
    virtual bool ProcessCommand(void);

// implemented operations -----------------------------------------------------
    //! get structure or data
    bool GetData(void);

    //! write structure or results
    bool WriteData(void);

    //! load structure
    bool LoadStructure(void);

    //! save structure
    bool SaveStructure(void);

    //! forced selection transaction reset
    bool SelReset(void);

    //! sent client package
    bool InstallPkg(void);

    //! get client application name
    bool GetClientAppName(void);

    bool CreateArchive(CXMLElement* p_root,const CFileName& root_path,const CFileName& dir_name);
    bool CreateFile(CXMLElement* p_root,const CFileName& root_path,const CFileName& file_name);
};

//------------------------------------------------------------------------------

#endif
