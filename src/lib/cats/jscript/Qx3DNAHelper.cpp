// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2015 Michal Ruzicka, michalruz@mail.muni.cz
//    Copyright (C) 2015 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <Qx3DNAHelper.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CLocalBP::CLocalBP(void)
{
    Valid = false;
    ID = -1;
    Name = "nd";
    Shear = 0.0;
    Stretch = 0.0;
    Stagger = 0.0;
    Buckle = 0.0;
    Propeller = 0.0;
    Opening = 0.0;
}

//------------------------------------------------------------------------------

CLocalBPStep::CLocalBPStep(void)
{
    Valid = false;
    ID = -1;
    Step = "nd";
    Shift = 0.0;
    Slide = 0.0;
    Rise = 0.0;
    Tilt = 0.0;
    Roll = 0.0;
    Twist = 0.0;
}

//------------------------------------------------------------------------------

CLocalBPHel::CLocalBPHel(void)
{
    Valid = false;
    ID = -1;
    Step = "nd";
    Xdisp = 0.0;
    Ydisp = 0.0;
    Hrise = 0.0;
    Incl = 0.0;
    Tip = 0.0;
    Htwist = 0.0;
}

//------------------------------------------------------------------------------

CDNABasePair::CDNABasePair(void)
{
    ID = -1;
    Name = "nd";
    ResIDA = 0;
    ResIDB = 0;
}

//------------------------------------------------------------------------------

CDNABasePairStep::CDNABasePairStep(void)
{
    ID = -1;
    Step = "nd";
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
