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
    RevertedCases = 0;
}

//------------------------------------------------------------------------------

void CNABPStat::RegisterData(const CNABPPar& data,bool flipped)
{
    NumOfSamples++;

    double shear = data.Shear;
    double buckle = data.Buckle;

    if( flipped ){
        shear *= -1.0;
        buckle *= -1.0;
        RevertedCases++;
    }

    Sum.Shear += shear;
    Sum.Stretch += data.Stretch;
    Sum.Stagger += data.Stagger;
    Sum.Buckle += buckle;
    Sum.Propeller += data.Propeller;
    Sum.Opening += data.Opening;

    Sum2.Shear += shear*shear;
    Sum2.Stretch += data.Stretch*data.Stretch;
    Sum2.Stagger += data.Stagger*data.Stagger;
    Sum2.Buckle += buckle*buckle;
    Sum2.Propeller += data.Propeller*data.Propeller;
    Sum2.Opening += data.Opening*data.Opening;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNABPStepStat::CNABPStepStat(void)
{
    NumOfSamples = 0;
    RevertedCases = 0;
}

//------------------------------------------------------------------------------

void CNABPStepStat::RegisterData(const CNABPStepPar& data,bool flipped)
{
    NumOfSamples++;

    double shift = data.Shift;
    double tilt = data.Tilt;

    if( flipped ){
        shift *= -1.0;
        tilt *= -1.0;
        RevertedCases++;
    }

    Sum.Shift += shift;
    Sum.Slide += data.Slide;
    Sum.Rise += data.Rise;
    Sum.Tilt += tilt;
    Sum.Roll += data.Roll;
    Sum.Twist += data.Twist;

    Sum2.Shift += shift*shift;
    Sum2.Slide += data.Slide*data.Slide;
    Sum2.Rise += data.Rise*data.Rise;
    Sum2.Tilt += tilt*tilt;
    Sum2.Roll += data.Roll*data.Roll;
    Sum2.Twist += data.Twist*data.Twist;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNABPHelStat::CNABPHelStat(void)
{
    NumOfSamples = 0;
    RevertedCases = 0;
}

//------------------------------------------------------------------------------

void CNABPHelStat::RegisterData(const CNABPHelPar& data,bool flipped)
{
    NumOfSamples++;

    double ydisp = data.Ydisp;
    double tip = data.Tip;

    if( flipped ){
        ydisp *= -1.0;
        tip *= -1.0;
        RevertedCases++;
    }

    Sum.Xdisp += data.Xdisp;
    Sum.Ydisp += ydisp;
    Sum.Hrise += data.Hrise;
    Sum.Incl += data.Incl;
    Sum.Tip += tip;
    Sum.Htwist += data.Htwist;

    Sum2.Xdisp += data.Xdisp*data.Xdisp;
    Sum2.Ydisp += ydisp*ydisp;
    Sum2.Hrise += data.Hrise*data.Hrise;
    Sum2.Incl += data.Incl*data.Incl;
    Sum2.Tip += tip*tip;
    Sum2.Htwist += data.Htwist*data.Htwist;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

