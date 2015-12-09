#ifndef Qx3DNAHelperH
#define Qx3DNAHelperH
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
#include <string>

//------------------------------------------------------------------------------

// parameters
class CLocalBP {
public:
    // constructor
    CLocalBP(void);         // basic data initialization

public:
    bool        Valid;      // determine if data are valid
    int         ID;         // counted from 0
    std::string Name;       // one base pair (A-A)
    double      Shear;
    double      Stretch;
    double      Stagger;
    double      Buckle;
    double      Propeller;
    double      Opening;
};

//------------------------------------------------------------------------------

class CLocalBPStep {
public:
    // constructor
    CLocalBPStep(void);     // basic data initialization

public:
    bool        Valid;      // determine if data are valid
    int         ID;         // counted from 0
    std::string Step;       // two base pairs (AT/AT)
    double      Shift;
    double      Slide;
    double      Rise;
    double      Tilt;
    double      Roll;
    double      Twist;
};

//------------------------------------------------------------------------------

class CLocalBPHel {
public:
    // constructor
    CLocalBPHel(void);      // basic data initialization

public:
    bool        Valid;      // determine if data are valid
    int         ID;         // counted from 0
    std::string Step;       // two base pairs (AT/AT)
    double      Xdisp;
    double      Ydisp;
    double      Hrise;
    double      Incl;
    double      Tip;
    double      Htwist;
};

//------------------------------------------------------------------------------

class CDNABasePair {
public:
    // constructor
    CDNABasePair(void);      // basic data initialization

public:
    int         ID;         // counted from 0
    std::string Name;       // (A-A)
    int         ResIDA;     // local residue index - counted from 0
    int         ResIDB;     // local residue index - counted from 0
};

//------------------------------------------------------------------------------

class CDNABasePairStep {
public:
    // constructor
    CDNABasePairStep(void);      // basic data initialization

public:
    int         ID;     // counted from 0
    std::string Step;   // (AT/AT)
};

//------------------------------------------------------------------------------

#endif
