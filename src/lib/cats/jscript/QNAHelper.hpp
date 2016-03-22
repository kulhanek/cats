#ifndef QNAHelperH
#define QNAHelperH
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
// nucleic acid parameters
//------------------------------------------------------------------------------

class CNALocalBPPar {
public:
    // constructor
    CNALocalBPPar(void);

public:
    bool        Valid;      // determine if data are valid - if reference snapshot is activated
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

class CNALocalBPStepPar {
public:
    // constructor
    CNALocalBPStepPar(void);

public:
    bool        Valid;      // determine if data are valid - if reference snapshot is activated
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

class CNALocalBPHelPar {
public:
    // constructor
    CNALocalBPHelPar(void);

public:
    bool        Valid;      // determine if data are valid - if reference snapshot is activated
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

class CNABPID {
public:
    // constructor
    CNABPID(void);

public:
    int         ID;         // counted from 0
    std::string Name;       // (A-A)
    int         ResIDA;     // local residue index - counted from 0
    int         ResIDB;     // local residue index - counted from 0

    bool operator < (const CNABPID& bp_id) const;
};

//------------------------------------------------------------------------------

class CNABPStepID {
public:
    // constructor
    CNABPStepID(void);

public:
    int         ID;         // counted from 0
    int         ResIDA;     // local residue index - counted from 0
    int         ResIDB;     // local residue index - counted from 0
    int         ResIDC;     // local residue index - counted from 0
    int         ResIDD;     // local residue index - counted from 0
    std::string Step;       // (AT/AT)

    bool operator < (const CNABPStepID& bp_id) const;
};

//------------------------------------------------------------------------------

#endif
