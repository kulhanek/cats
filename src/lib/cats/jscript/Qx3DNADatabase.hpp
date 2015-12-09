#ifndef Qx3DNADatabaseH
#define Qx3DNADatabaseH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2015 Michal Ruzicka, michalruz@mail.muni.cz
//    Copyright (C) 2015 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <CATsMainHeader.hpp>
#include <Qx3DNAHelper.hpp>
#include <Qx3DNADatabaseHelper.hpp>
#include <map>

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------

class Qx3DNADatabase {
public:
    /// clear all previous data
    void Clear(void);



private:
    /// register data into the database
    void RegisterData(const CLocalBP& data);
    void RegisterData(const CLocalBPStep& data);
    void RegisterData(const CLocalBPHel& data);

    std::set<CDNABasePairID>                            BasePairIDs;
    std::set<CDNABasePairStepID>                        BasePairStepIDs;

    std::map<CDNABasePairID,CLocalBPSnapshots>          LocalBPSnapshots;
    std::map<CDNABasePairStepID,CLocalBPStepSnapshots>  LocalBPStepSnapshots;
    std::map<CDNABasePairStepID,CLocalBPHelSnapshots>   LocalBPHelSnapshots;
};


//------------------------------------------------------------------------------

#endif
