#ifndef VSClientH
#define VSClientH
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

#include "VSClientOptions.hpp"
#include <ExtraClient.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

class CVSClient : private CExtraClient {
public:
    // constructor
    CVSClient(void);

// main methods ---------------------------------------------------------------
    //! init options
    int Init(int argc,char* argv[]);

    //! main part of program
    bool Run(void);

    //! finalize program
    bool Finalize(void);

// section of public data ----------------------------------------------------
public:
    CVSClientOptions    Options;              // program options
    CVerboseStr         MsgOut;
    CTerminalStr        Terminal;

// supported operations -------------------------------------------------------
    //! get data from the server
    bool GetData(CSmallString& molid);

    //! read structure
    bool ReadStructure(CXMLElement* p_root);

    //! write data to the server
    bool WriteData(void);

    //! write structure
    bool WriteStructure(CXMLElement* p_root);

    //! get data from the server - structure only
    bool LoadStructure(void);

    //! write data to the server - structure only
    bool SaveStructure(void);

    //! install client package
    bool InstallClientPkg(void);

    //! get client app name
    bool GetClientAppName(bool& silent_error);

    bool ExtractArchive(CXMLElement* p_direle,const CFileName& root_path);
    bool ExtractFile(CXMLBinData* p_fileele,const CFileName& root_path);
};

//------------------------------------------------------------------------------

#endif
