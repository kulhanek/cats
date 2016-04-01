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

class CNABPPar {
public:
    // constructor
    CNABPPar(void);

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

class CNABPStepPar {
public:
    // constructor
    CNABPStepPar(void);

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

class CNABPHelPar {
public:
    // constructor
    CNABPHelPar(void);

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

    // make ID canonical (e.g. ResIDA < ResIDB), return true if helical axis is flipped
    bool MakeCanonical(void);   // e.g. ResIDA < ResIDB

public: 
    int         ID;                 // counted from 0
    std::string Name;               // (A-A) -> (ResIDA-ResIDB)
    int         ResIDA;             // local residue index - counted from 0
    int         ResIDB;             // local residue index - counted from 0

    bool operator < (const CNABPID& bp_id) const;
};

//------------------------------------------------------------------------------

class CNABPStepID {
public:
    // constructor
    CNABPStepID(void);

    // make ID canonical (e.g. ResIDA < ResIDD), return true if helical axis is flipped
    bool MakeCanonical(void);

public:
    int         ID;         // counted from 0
    int         ResIDA;     // local residue index - counted from 0
    int         ResIDB;     // local residue index - counted from 0
    int         ResIDC;     // local residue index - counted from 0
    int         ResIDD;     // local residue index - counted from 0
    std::string Step;       // (AT/AT) -> (ResIDA.ResIDB/ResIDC.ResIDD)

    bool operator < (const CNABPStepID& bp_id) const;
};

//------------------------------------------------------------------------------

#endif
