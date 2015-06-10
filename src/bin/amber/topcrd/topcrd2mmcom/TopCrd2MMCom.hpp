#ifndef TopCrd2MMComH
#define TopCrd2MMComH
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

#include "TopCrd2MMComOptions.hpp"
#include <AmberTopology.hpp>
#include <AmberRestart.hpp>
#include <AmberMaskAtoms.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>
#include <map>

//------------------------------------------------------------------------------

class CTopCrd2MMCom {
public:
    // constructor
    CTopCrd2MMCom(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    bool Finalize(void);

// section of private data ----------------------------------------------------
private:
    CTopCrd2MMComOptions    Options;            // program options
    CAmberTopology          Topology;
    CAmberRestart           Coordinates;
    CAmberMaskAtoms         HighAmberMask;
    FILE*                   OutputFile;

    bool                    Oniom;

    bool                    PrintHrmBndParamsForWater;
    bool                    CalcEb;
    bool                    CalcEa;
    bool                    CalcEd;
    bool                    CalcEvdw;
    bool                    CalcEel;

    CSimpleList<CAmberAtom>             LowLinkAtoms;
    CSmallString                        LowLinkAtomName;
    std::map<CSmallString,CSmallString> TypeMap;

    bool DetermineBoundaries(void);
    bool MakeTypeMap(void);
    bool WriteDataToMMCom(void);
    bool WriteHeader(void);
    bool WriteCoords(void);
    bool WriteConnect(void);
    bool WriteBondTypes(void);
    bool WriteAngleTypes(void);
    bool WriteDihedralTypes(void);
    bool WriteImproperDihedralTypes(void);
    bool WriteNBTypes(void);
    bool WriteTypes(void);
};

//---------------------------------------------------------------------------

class CBondMMType {
public:
    CBondMMType(void);
public:
    char            AType[5];
    char            BType[5];
    CAmberBondType* BondType;
};

//---------------------------------------------------------------------------

class CAngleMMType {
public:
    CAngleMMType(void);
public:
    char            AType[5];
    char            BType[5];
    char            CType[5];
    CAmberAngleType*    AngleType;
};

//---------------------------------------------------------------------------

class CDihedralMMType {
public:
    CDihedralMMType(void);
public:
    char            AType[5];
    char            BType[5];
    char            CType[5];
    char            DType[5];
    CAmberDihedralType* DihedralType[4];
};

//---------------------------------------------------------------------------

class CImproperDihedralMMType {
public:
    CImproperDihedralMMType(void);
public:
    char            AType[5];
    char            BType[5];
    char            CType[5];
    char            DType[5];
    CAmberDihedralType* DihedralType;
};

//------------------------------------------------------------------------------

#endif
