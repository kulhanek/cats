#ifndef TrajClientH
#define TrajClientH
// ===============================================================================
// PMFLib - Library Supporting Potential of Mean Force Calculations
// -------------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu
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
// ===============================================================================

#include "TrajClientOptions.hpp"
#include <TrajectoryClient.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

class CTrajClient : public CTrajectoryClient {
public:
    // constructor
    CTrajClient(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    void Finalize(void);

// section of private data ----------------------------------------------------
private:
    CTrajClientOptions  Options;            // program options
    CTerminalStr        Console;
    CVerboseStr         vout;

// supported operations -------------------------------------------------------
    /// register client to server with data template
    void RegisterClient(void);

    /// unregister client
    void UnregisterClient(void);

    /// write data to the server
    void GetSnapshot(bool read_vel);
};

//------------------------------------------------------------------------------

#endif
