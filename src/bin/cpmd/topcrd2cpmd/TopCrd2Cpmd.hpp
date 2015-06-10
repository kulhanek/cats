#ifndef TopCrd2CpmdH
#define TopCrd2CpmdH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "TopCrd2CpmdOptions.hpp"
#include <AmberTopology.hpp>
#include <AmberRestart.hpp>
#include <AmberMaskAtoms.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>
#include <Point.hpp>
#include <vector>

//------------------------------------------------------------------------------

/// link bond data
class CQMLink {
public:
    int     ID;
    int     QMIndex;
    int     MMIndex;
    int     HLinkAtmIndex;
    int     HLinkResIndex;
    CPoint  HLinkPos;
};

//------------------------------------------------------------------------------

/// main convertor
class CTopCrd2Cpmd {
public:
    // constructor
    CTopCrd2Cpmd(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    void Finalize(void);

// section of private data ----------------------------------------------------
private:
    CTopCrd2CpmdOptions     Options;            // program options
    CAmberTopology          Topology;
    CAmberRestart           Coordinates;
    CAmberMaskAtoms         Mask;
    CTerminalStr            Console;
    CVerboseStr             vout;
    std::vector<CQMLink>    QMLinks;
    int                     NumberOfSoluteAtoms;
    int                     NumberOfSoluteResidues;
    int                     QMZoneCharge;

    // load input data
    bool LoadInputData(void);
    bool SystemInfo(void);
    bool PrepareSystem(void);
    bool QMZoneInfo(void);

    // cpmd input file
    bool WriteCPMDFiles(void);
    bool WriteCPMDInputFile(void);

    bool WriteCPMDTitle(std::ostream& sout);
    bool WriteCPMDSystem(std::ostream& sout);
    bool WriteCPMDDFT(std::ostream& sout);
    bool WriteCPMDAtoms(std::ostream& sout);

    // gromos files
    bool WriteGromosFiles(void);
    bool WriteGromosCoordinates(void);
};

//------------------------------------------------------------------------------

#endif
