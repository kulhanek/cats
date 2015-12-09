#ifndef Qx3DNADatabaseHelperH
#define Qx3DNADatabaseHelperH
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
#include <map>
#include <vector>
#include <set>

//------------------------------------------------------------------------------

// parameters - only data now
class CLocalBPSnapshot {
public:
    // constructor
    CLocalBPSnapshot(void);         // basic data initialization

public:
    int     SnapshotID;             // snapshot ID
    double  Shear;
    double  Stretch;
    double  Stagger;
    double  Buckle;
    double  Propeller;
    double  Opening;
};

typedef std::vector<CLocalBPSnapshot>   CLocalBPSnapshots;

//------------------------------------------------------------------------------

class CLocalBPStepSnasphot {
public:
    // constructor
    CLocalBPStepSnasphot(void);     // basic data initialization

public:
    int     SnapshotID;             // snapshot ID
    double  Shift;
    double  Slide;
    double  Rise;
    double  Tilt;
    double  Roll;
    double  Twist;
};

typedef std::vector<CLocalBPStepSnasphot>   CLocalBPStepSnapshots;

//------------------------------------------------------------------------------

class CLocalBPHelSnapshot {
public:
    // constructor
    CLocalBPHelSnapshot(void);      // basic data initialization

public:
    int     SnapshotID;             // snapshot ID
    double  Xdisp;
    double  Ydisp;
    double  Hrise;
    double  Incl;
    double  Tip;
    double  Htwist;
};

typedef std::vector<CLocalBPHelSnapshot>   CLocalBPHelSnapshots;

//------------------------------------------------------------------------------

// parameters - only ID of pair or step

class CDNABasePairID {
public:
    // constructor
    CDNABasePairID(void);       // basic data initialization

public:
    int         ResIDA;
    int         ResIDB;
    std::string Name;           // (ResA-ResB)
};

//------------------------------------------------------------------------------

class CDNABasePairStepID {
public:
    // constructor
    CDNABasePairStepID(void);   // basic data initialization

public:
    int         ResIDA;
    int         ResIDB;
    int         ResIDC;
    int         ResIDD;
    std::string Step;           // ((ResA-ResB)/(ResC-ResD)) ???
};

//------------------------------------------------------------------------------

#endif
