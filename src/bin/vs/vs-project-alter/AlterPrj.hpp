#ifndef AlterPrjH
#define AlterPrjH
// =============================================================================
// VScreen - Virtual Screening Tools
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

#include "AlterPrjOptions.hpp"
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

class CFirebirdTransaction;

//------------------------------------------------------------------------------

class CAlterPrj {
public:
    // constructor
    CAlterPrj(void);

// main methods ----------------------------------------------------------------
    //! init options
    int Init(int argc,char* argv[]);

    //! main part of program
    bool Run(void);

    //! finalize program
    bool Finalize(void);

// section of public data ------------------------------------------------------
public:
    CAlterPrjOptions         Options;            // program options

// section of private data ----------------------------------------------------
private:
    CVerboseStr             MsgOut;             // output messages

    bool SoftReset(CFirebirdTransaction* p_trans);
    bool HardReset(CFirebirdTransaction* p_trans);
    bool FinalizeAll(CFirebirdTransaction* p_trans);
    bool AddIndex(CFirebirdTransaction* p_trans);
    bool RemoveIndex(CFirebirdTransaction* p_trans);
    bool DeleteAll(CFirebirdTransaction* p_trans);
    bool AddResultColumn(CFirebirdTransaction* p_trans,int old_num_of_results);
    bool RemoveResultColumn(CFirebirdTransaction* p_trans,int old_num_of_results);
    bool AlterOperator(CFirebirdTransaction* p_trans);
    bool AlterDescription(CFirebirdTransaction* p_trans);

};

//------------------------------------------------------------------------------

extern CAlterPrj AlterPrj;

//------------------------------------------------------------------------------

#endif
