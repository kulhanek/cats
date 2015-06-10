#ifndef ResultClientH
#define ResultClientH
// =============================================================================
// CATs - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2011 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu
//    Copyright (C) 2005 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <ExtraClient.hpp>

//------------------------------------------------------------------------------

class CAmberRestart;

//------------------------------------------------------------------------------

class CResultClient : public CExtraClient {
public:
    // constructor
    CResultClient(void);

// supported operations -------------------------------------------------------
    /// register client with template
    bool RegisterClient(const CSmallString& template_name, int& client_id);

    /// get statistics
    bool GetStatistics(const CSmallString& stat_name);

    /// write data to server
    bool WriteData(int client_id,const CSmallString& data_name);

    /// flush server data
    bool FlushServerData(void);
};

//------------------------------------------------------------------------------

#endif
