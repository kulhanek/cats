#ifndef CubeGridGenH
#define CubeGridGenH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2018 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "CubeGridGenOptions.hpp"
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>
#include <vector>
#include <Point.hpp>
#include <XYZStructure.hpp>

//------------------------------------------------------------------------------

class CCubeGridGen {
public:
    // constructor
    CCubeGridGen(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    void Finalize(void);

// section of private data ----------------------------------------------------
private:
    CCubeGridGenOptions Options;            // program options
    CTerminalStr        Console;
    CVerboseStr         vout;
    std::vector<CPoint> GridPoints;
    CXYZStructure       Structure;

    // statistics
    int                 TotalNumber;
    int                 ExcludedNumber;
    int                 NumberOfBatches;

    // test grid point against the structure
    bool IsPointValid(const CPoint& pos);

    // save final data
    bool SaveGridPoints(void);
};

//------------------------------------------------------------------------------

#endif
