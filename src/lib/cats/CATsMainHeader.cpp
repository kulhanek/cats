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
#include <CATsMainHeaderConfig.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const char* LibBuildVersion_CATs = CATS_VERSION "(" CATS_BUILDTIME ") [CATs - Conversion And Analysis Tools]";
const char* LibVersion_CATs      = CATS_VERSION "(" CATS_BUILDTIME ")";

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

#ifdef WIN32

#include <windows.h>

// under windows we should define entry point for library

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
    return 1;
}

#else

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



#endif

//--------------------------------------------------------------------------
