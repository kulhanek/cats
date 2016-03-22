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

#include <QNAHelper.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNALocalBPPar::CNALocalBPPar(void)
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

CNALocalBPStepPar::CNALocalBPStepPar(void)
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

CNALocalBPHelPar::CNALocalBPHelPar(void)
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

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNABPID::CNABPID(void)
{
    ID = -1;
    Name = "nd";
    ResIDA = -1;
    ResIDB = -1;
}

//------------------------------------------------------------------------------

bool CNABPID::operator < (const CNABPID& bp_id) const
{
    if (ResIDA < bp_id.ResIDA)  return(true);
    if (ResIDA > bp_id.ResIDA)  return(false);
    // Otherwise a are equal
    if (ResIDB < bp_id.ResIDB)  return(true);
    if (ResIDB > bp_id.ResIDB)  return(false);
    // Otherwise both are equal
    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNABPStepID::CNABPStepID(void)
{
    ID = -1;
    Step = "nd";
    ResIDA = -1;
    ResIDB = -1;
    ResIDC = -1;
    ResIDD = -1;
}

//------------------------------------------------------------------------------

bool CNABPStepID::operator < (const CNABPStepID& bp_id) const
{
    if (ResIDA < bp_id.ResIDA)  return(true);
    if (ResIDA > bp_id.ResIDA)  return(false);
    // Otherwise a are equal
    if (ResIDB < bp_id.ResIDB)  return(true);
    if (ResIDB > bp_id.ResIDB)  return(false);
    // Otherwise b are equal
    if (ResIDC < bp_id.ResIDC)  return(true);
    if (ResIDC > bp_id.ResIDC)  return(false);
    // Otherwise c are equal
    if (ResIDD < bp_id.ResIDD)  return(true);
    if (ResIDD > bp_id.ResIDD)  return(false);

    // Otherwise all are equal
    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
