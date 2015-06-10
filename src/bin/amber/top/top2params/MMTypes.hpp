#ifndef MMTypesH
#define MMTypesH
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

#include <AmberTopology.hpp>

//---------------------------------------------------------------------------

class CBondMMType {
public:
    CBondMMType(void);
public:
    char            AType[5];
    char            BType[5];
    CAmberBondType*    BondType;
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
    CAmberDihedralType*    DihedralType[5];
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
    CAmberDihedralType*    DihedralType;
};

//---------------------------------------------------------------------------


#endif
