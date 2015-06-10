#ifndef TopCrd2CrdH
#define TopCrd2CrdH
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

#include "TopCrd2CrdOptions.hpp"
#include <AmberTopology.hpp>
#include <AmberRestart.hpp>
#include <AmberMaskAtoms.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>

//------------------------------------------------------------------------------

class CTopCrd2Crd {
public:
    // constructor
    CTopCrd2Crd(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    bool Finalize(void);

// section of private data ----------------------------------------------------
private:
    CTopCrd2CrdOptions      Options;            // program options
    CAmberTopology          Topology;
    CAmberRestart           Coordinates;
    CAmberMaskAtoms         Mask;

    // input methods
    bool ReadCRD(FILE* p_fin);
    bool ReadXYZ(FILE* p_fin);

    // output methods
    bool WriteCRD(FILE* p_fout);
    bool WritePDB(FILE* p_fout);
    bool WritePQR(FILE* p_fout);
    bool WriteXYZ(FILE* p_fout);
    bool WriteXYZR(FILE* p_fout);
    bool WriteCOM(FILE* p_fout);
    bool WriteRBOX(FILE* p_fout);
    bool WriteTRAJ(FILE* p_fout);
    bool WriteADF(FILE* p_fout);
    bool WriteADFFrag(FILE* p_fout);
    bool WriteADF3DRISM(FILE* p_fout);

    // helper methods
    bool WritePDBRemark(FILE* p_file,const CSmallString& string);
    // get PDBName
    const char* GetPDBAtomName(CAmberAtom* p_atom,CAmberResidue* p_res);
    const char* GetPDBResName(CAmberAtom* p_atom,CAmberResidue* p_res);
};

//------------------------------------------------------------------------------

#endif
