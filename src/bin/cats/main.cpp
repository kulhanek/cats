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

#include <stdio.h>
#include "CATs.hpp"
#include <QCoreApplication>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int main(int argc, char* argv[])
{
    QCoreApplication    app(argc,argv);

    CCATs object;

    int result = 0;
    switch(object.Init(argc,argv)) {
    case SO_CONTINUE:
        result = 0;
        if( object.Run() == false ){
            result = 1;
        } else {
            result = object.GetExitValue();
        }
        break;
    case SO_EXIT:
        return(0);
    case SO_OPTS_ERROR:
        return(1);
    case SO_USER_ERROR:
    default:
        result = 2;
        break;
    }
    object.Finalize();

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
