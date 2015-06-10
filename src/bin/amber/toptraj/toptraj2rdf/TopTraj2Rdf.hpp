#ifndef TopTraj2RdfH
#define TopTraj2RdfH
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

#include "TopTraj2RdfOptions.hpp"
#include <AmberTopology.hpp>
#include <AmberTrajectory.hpp>
#include <AmberRestart.hpp>
#include <Matrix.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

class CTopTraj2Rdf {
public:
    // constructor
    CTopTraj2Rdf(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    bool Finalize(void);

    /// main part of program - atom based
    bool RunAtoms(void);

    /// main part of program - residue based
    bool RunResidues(void);

// executive methods ----------------------------------------------------------
    /// calculate distance
    bool CalculateAtoms(int atomA,int atomB);

    /// calculate distance
    bool CalculateResidues(int residA,int residB);

// section of private data ----------------------------------------------------
private:
    CTopTraj2RdfOptions     Options;            // program options
    CAmberTopology          Topology;
    CAmberRestart           Coordinates;
    CAmberTrajectory        Trajectory;
    CVector                 Histogram;
    int                     TotalNumberOfSnapshots;
    int                     NumberOfSnapshots;
    int                     NumberOfSoluteMols;

    void PrintHeader(FILE* p_fout);
};

//------------------------------------------------------------------------------

#endif
