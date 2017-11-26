#ifndef XYZSplitH
#define XYZSplitH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2017 Petr Kulhanek, kulhanek@chemi.muni.cz
//
// FFDevel is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// FFDevel is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with FFDevel. If not, see <http://www.gnu.org/licenses/>.
// =============================================================================

#include <stdio.h>
#include "XYZSplitOptions.hpp"
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>
#include <XYZStructure.hpp>
#include <vector>
#include <map>

//------------------------------------------------------------------------------

class CXYZSplit {
public:
    // constructor
    CXYZSplit(void);
    ~CXYZSplit(void);

// main methods ----------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    void Finalize(void);

// section of private data -----------------------------------------------------
private:
    CXYZSplitOptions    Options;    // program options
    CXYZStructure       Structure;  // input structure
    std::vector<int>                    MoleculeId;
    std::vector< std::pair<int,int> >   Bonds;
    int                                 NumberOfMolecules;
    CXYZStructure       OutStructure;  // output structure

    // output ------------------------------------
    CTerminalStr        Console;
    CVerboseStr         vout;

    /// load/save structure
    bool LoadStructure(void);
    bool SaveStructure(const CSmallString& name);
    bool SaveStructureCP(const CSmallString& name,int molid);

    /// detect bonds - use simple criteria
    void FindBonds(void);

    /// split structure into individual molecules
    void SplitStructure(void);

    /// copy molecule
    void CopyMolecule(int molid);
};

//------------------------------------------------------------------------------

#endif
