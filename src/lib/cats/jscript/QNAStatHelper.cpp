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

#include <QNAStatHelper.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNABPStat::CNABPStat(void)
{
    NumOfSamples = 0;
}

//------------------------------------------------------------------------------

void CNABPStat::RegisterData(const CNABPPar& data)
{
    NumOfSamples++;

    Sum.Shear += data.Shear;
    Sum.Stretch += data.Stretch;
    Sum.Stagger += data.Stagger;
    Sum.Buckle += data.Buckle;
    Sum.Propeller += data.Propeller;
    Sum.Opening += data.Opening;

    Sum2.Shear += data.Shear*data.Shear;
    Sum2.Stretch += data.Stretch*data.Stretch;
    Sum2.Stagger += data.Stagger*data.Stagger;
    Sum2.Buckle += data.Buckle*data.Buckle;
    Sum2.Propeller += data.Propeller*data.Propeller;
    Sum2.Opening += data.Opening*data.Opening;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNABPStepStat::CNABPStepStat(void)
{
    NumOfSamples = 0;
}

//------------------------------------------------------------------------------

void CNABPStepStat::RegisterData(const CNABPStepPar& data)
{
    NumOfSamples++;

    Sum.Shift += data.Shift;
    Sum.Slide += data.Slide;
    Sum.Rise += data.Rise;
    Sum.Tilt += data.Tilt;
    Sum.Roll += data.Roll;
    Sum.Twist += data.Twist;

    Sum2.Shift += data.Shift*data.Shift;
    Sum2.Slide += data.Slide*data.Slide;
    Sum2.Rise += data.Rise*data.Rise;
    Sum2.Tilt += data.Tilt*data.Tilt;
    Sum2.Roll += data.Roll*data.Roll;
    Sum2.Twist += data.Twist*data.Twist;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNABPHelStat::CNABPHelStat(void)
{
    NumOfSamples = 0;
}

//------------------------------------------------------------------------------

void CNABPHelStat::RegisterData(const CNABPHelPar& data)
{
    NumOfSamples++;

    Sum.Xdisp += data.Xdisp;
    Sum.Ydisp += data.Ydisp;
    Sum.Hrise += data.Hrise;
    Sum.Incl += data.Incl;
    Sum.Tip += data.Tip;
    Sum.Htwist += data.Htwist;

    Sum2.Xdisp += data.Xdisp*data.Xdisp;
    Sum2.Ydisp += data.Ydisp*data.Ydisp;
    Sum2.Hrise += data.Hrise*data.Hrise;
    Sum2.Incl += data.Incl*data.Incl;
    Sum2.Tip += data.Tip*data.Tip;
    Sum2.Htwist += data.Htwist*data.Htwist;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

