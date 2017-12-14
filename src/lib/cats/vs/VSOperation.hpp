#ifndef VSOperationH
#define VSOperationH
// =============================================================================
// ChemInfo - Chemoinformatics Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <Operation.hpp>

//------------------------------------------------------------------------------

//! get data from the server
DECLARE_OPERATION(CATS_PACKAGE,Operation_GetVSData);

//! write data to the server
DECLARE_OPERATION(CATS_PACKAGE,Operation_WriteVSData);

//! load structure from the server
DECLARE_OPERATION(CATS_PACKAGE,Operation_LoadStructure);

//! save structure to the server
DECLARE_OPERATION(CATS_PACKAGE,Operation_SaveStructure);

//! install client package
DECLARE_OPERATION(CATS_PACKAGE,Operation_InstallPkg);

//! get client application name
DECLARE_OPERATION(CATS_PACKAGE,Operation_GetAppName);

//------------------------------------------------------------------------------

#endif
