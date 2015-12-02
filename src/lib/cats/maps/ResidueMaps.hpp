#ifndef ResidueMapsH
#define ResidueMapsH
// =============================================================================
// CATs - Conversion and Analysis Tools
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

#include <CATsMainHeader.hpp>

#define MAXRESLIST 24

// translation rules for N and C terminal residues

extern const char* ResidueNameConversion[MAXRESLIST][2];

//---------------------------------------------------------------------------

bool CATS_PACKAGE ConvertResnameToCTerminal(CSmallString& resname,bool silent=false);
bool CATS_PACKAGE ConvertResnameToNTerminal(CSmallString& resname,bool silent=false);
bool CATS_PACKAGE ConvertNCResnameToResname(CSmallString& resname);

//---------------------------------------------------------------------------

#define MAXRESLISTFASTA 47

extern const char* ResidueToFASTA[MAXRESLISTFASTA][2];

//---------------------------------------------------------------------------

bool CATS_PACKAGE ConvertResnameToFASTA(CSmallString& resname);

//---------------------------------------------------------------------------

#endif
