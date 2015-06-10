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

#include "MMTypes.hpp"
#include <string.h>

//---------------------------------------------------------------------------

CBondMMType::CBondMMType(void)
{
    memset(AType,0,5);
    memset(BType,0,5);
    BondType=NULL;
}

//---------------------------------------------------------------------------

CAngleMMType::CAngleMMType(void)
{
    memset(AType,0,5);
    memset(BType,0,5);
    memset(CType,0,5);
    AngleType=NULL;
}

//---------------------------------------------------------------------------

CDihedralMMType::CDihedralMMType(void)
{
    memset(AType,0,5);
    memset(BType,0,5);
    memset(CType,0,5);
    memset(DType,0,5);
    for(int i=0; i<5; i++) DihedralType[i] = NULL;
}

//---------------------------------------------------------------------------

CImproperDihedralMMType::CImproperDihedralMMType(void)
{
    memset(AType,0,5);
    memset(BType,0,5);
    memset(CType,0,5);
    memset(DType,0,5);
    DihedralType = NULL;
}

//---------------------------------------------------------------------------
