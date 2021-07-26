#ifndef TopCrdManipH
#define TopCrdManipH
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

#include "TopCrdManipOptions.hpp"
#include <AmberTopology.hpp>
#include <AmberRestart.hpp>
#include <AmberMaskAtoms.hpp>
#include <Point.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

class CTopCrdManip {
public:
    // constructor
    CTopCrdManip(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    bool Finalize(void);

// executive methods ----------------------------------------------------------
    /// image coordinates
    bool ImageCoordinates(void);
    /// decode image options
    bool DecodeImageOptions(void);

    /// center slected region
    bool CenterCoordinates(void);
    /// decode center options
    bool DecodeCenterOptions(void);

    /// swap selected regions
    bool SwapCoordinates(void);
    /// decode swap options
    bool DecodeSwapOptions(void);

    /// move selected regions
    bool MoveCoordinates(void);
    /// decode move options
    bool DecodeMoveOptions(void);

    /// move selected regions to given position
    bool MoveToCoordinates(void);
    /// decode moveto options
    bool DecodeMoveToOptions(void);

    /// align principal axes
    bool Principal(void);
    /// decode principal options
    bool DecodePrincipal(void);

    /// rotate around bond
    bool BondRot(void);
    /// rotate around bond options
    bool DecodeBondRot(void);

// section of private data ----------------------------------------------------
private:
    CTopCrdManipOptions     Options;            // program options
    CAmberTopology          Topology;
    CAmberRestart           Coordinates;

    // image options
    bool                    ImageToOrigin;      // cell is centered around origin
    bool                    AtomBasedImaging;
    bool                    FamiliarImaging;    // familiar imaging

    // center options
    CSmallString            CenterMask;
    bool                    CenterToOrigin;

    // swap options
    CSmallString            SwapMask1;
    CSmallString            SwapMask2;

    // move and moveto options
    CSmallString            MoveMask;
    CPoint                  MoveData;

    // principal
    CSmallString            PrincipalMask;

    // bondrot
    CSmallString            AtmMask1;
    CSmallString            AtmMask2;
    double                  Angle;
};

//------------------------------------------------------------------------------

#endif
