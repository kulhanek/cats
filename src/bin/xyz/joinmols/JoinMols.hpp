#ifndef JoinMolsH
#define JoinMolsH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2025 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "JoinMolsOptions.hpp"
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>
#include <InfMol.hpp>

//------------------------------------------------------------------------------

class CJoinMols {
public:
    // constructor
    CJoinMols(void);
    ~CJoinMols(void);

// main methods ----------------------------------------------------------------
    //! init options
    int Init(int argc,char* argv[]);

    //! main part of program
    bool Run(void);

    //! finalize program
    bool Finalize(void);

// section of public data -----------------------------------------------------
public:
    CJoinMolsOptions    Options;            // program options

// section of private data ----------------------------------------------------
private:
    CVerboseStr         MsgOut;             // output messages
    CInfMol             Str;
};

//------------------------------------------------------------------------------

extern CJoinMols JoinMols;

//------------------------------------------------------------------------------

#endif
