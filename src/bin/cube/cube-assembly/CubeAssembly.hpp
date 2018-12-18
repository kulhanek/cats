#ifndef CubeAssemblyH
#define CubeAssemblyH
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

#include "CubeAssemblyOptions.hpp"
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>
#include <vector>
#include <Point.hpp>
#include <XYZStructure.hpp>
#include <iostream>

//------------------------------------------------------------------------------

class CCubeAssembly {
public:
    // constructor
    CCubeAssembly(void);
    ~CCubeAssembly(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    void Finalize(void);

// section of private data ----------------------------------------------------
private:
    CCubeAssemblyOptions    Options;           // program options
    CTerminalStr            Console;
    CVerboseStr             vout;
    double*                 Properties;         // data properties
    double                  PosX,PosY,PosZ;     // position of corner - (SX,SY,SZ)/2
    CXYZStructure           Structure;

    // statistics
    int                     TotalPoints;
    int                     ReadPoints;
    int                     StoredPoints;
    int                     IllegalPoints;

    // read input properties
    bool ReadProperties(std::istream& fin);

    // register property
    void RegisterPoint(double x, double y, double z, double p);

    // save final data
    bool SaveCubeFile(void);
};

//------------------------------------------------------------------------------

#endif
