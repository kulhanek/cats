#ifndef TopCrd2VMDBoxH
#define TopCrd2VMDBoxH
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

#include "TopCrd2VMDBoxOptions.hpp"
#include <AmberTopology.hpp>
#include <AmberRestart.hpp>
#include <AmberMaskAtoms.hpp>
#include <Point.hpp>
#include <Transformation.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

class CTopCrd2VMDBox {
public:
    // constructor
    CTopCrd2VMDBox(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    bool Finalize(void);

// section of private data ----------------------------------------------------
private:
    CTopCrd2VMDBoxOptions   Options;            // program options
    CAmberTopology          Topology;
    CAmberRestart           Coordinates;
    CPoint                  AVector;
    CPoint                  BVector;
    CPoint                  CVector;
    CTransformation         BoxTrans;

    bool WriteBox(FILE* p_vmdbox);
    bool WriteFamiliarTruncatedOctahedralBox(FILE* p_vmdbox);
    void GetBoxVectors(const CPoint& sizes,const CPoint& angles);
};

//------------------------------------------------------------------------------

#endif
