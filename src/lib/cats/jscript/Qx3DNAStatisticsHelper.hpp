#ifndef Qx3DNAStatisticsHelperH
#define Qx3DNAStatisticsHelperH
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
#include <boost/shared_ptr.hpp>
#include <string>

//------------------------------------------------------------------------------

class CLocalBPStat {
public:
    // constructor
    CLocalBPStat(void);

public:
    int         NumOfSamples;
    CLocalBP    Sum;
    CLocalBP    Sum2;

    void RegisterData(const CLocalBP& data);
};

typedef boost::shared_ptr<CLocalBPStat>   CLocalBPStatPtr;

//------------------------------------------------------------------------------

class CLocalBPStepStat {
public:
    // constructor
    CLocalBPStepStat(void);

public:
    int             NumOfSamples;
    CLocalBPStep    Sum;
    CLocalBPStep    Sum2;
};

typedef boost::shared_ptr<CLocalBPStepStat>   CLocalBPStepStatPtr;

//------------------------------------------------------------------------------

class CLocalBPHelStat {
public:
    // constructor
    CLocalBPHelStat(void);

public:
    int         NumOfSamples;
    CLocalBPHel Sum;
    CLocalBPHel Sum2;
};

typedef boost::shared_ptr<CLocalBPHelStat>   CLocalBPHelStatPtr;

//------------------------------------------------------------------------------

// parameters - only ID of pair or step

class CDNABasePairID {
public:
    // constructor
    CDNABasePairID(const CDNABasePair& bp);

public:
    int         ResIDA;
    int         ResIDB;
    std::string Name;           // (ResA-ResB)

    bool operator < (const CDNABasePairID& bp_id) const;
};

//------------------------------------------------------------------------------

class CDNABasePairStepID {
public:
    // constructor
    CDNABasePairStepID(void);

public:
    int         ResIDA;
    int         ResIDB;
    int         ResIDC;
    int         ResIDD;
    std::string Step;           // ((ResA-ResB)/(ResC-ResD)) ???
};

//------------------------------------------------------------------------------

#endif
