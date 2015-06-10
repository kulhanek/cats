#ifndef TopCrdGeoH
#define TopCrdGeoH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
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

#include "TopCrdGeoOptions.hpp"
#include <AmberTopology.hpp>
#include <AmberRestart.hpp>
#include <AmberMaskAtoms.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

class CTopCrdGeo {
public:
    // constructor
    CTopCrdGeo(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    bool Finalize(void);

// executive methods ----------------------------------------------------------
    /// COM position
    bool GetCOM(void);
    /// decode COM position options
    bool DecodeGetCOM(void);

    /// distance
    bool GetDistance(void);
    /// decode distance options
    bool DecodeGetDistance(void);

    /// principal vector of tensor of inertia
    bool GetTOIVector(void);
    /// decode principal vector of tensor of inertias
    bool DecodeGetTOIVector(void);

    /// eigenvalues of tensor of inertia
    bool GetTOIMagnitudes(void);
    /// decode eigenvalues of tensor of inertia
    bool DecodeTOIMagnitudes(void);

// section of private data ----------------------------------------------------
private:
    CTopCrdGeoOptions       Options;            // program options
    CAmberTopology          Topology;
    CAmberRestart           Coordinates;

    CSmallString            Mask1;
    CSmallString            Mask2;
    CSmallString            Mask3;
    CSmallString            Mask4;
    int                     VectorNumber;
};

//------------------------------------------------------------------------------

#endif
