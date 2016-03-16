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

#include <Qx3DNAStatisticsHelper.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CLocalBPStat::CLocalBPStat(void)
{
    NumOfSamples = 0;
}

//------------------------------------------------------------------------------

void CLocalBPStat::RegisterData(const CLocalBP& data)
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

CDNABasePairID::CDNABasePairID(const CDNABasePair& bp)
{
    ResIDA = bp.ResIDA;
    ResIDB = bp.ResIDB;
    Name = bp.Name;
}

//------------------------------------------------------------------------------

bool CDNABasePairID::operator < (const CDNABasePairID& bp_id) const
{
    return( (ResIDA < bp_id.ResIDA) || (ResIDB < bp_id.ResIDB) );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CDNABasePairStepID::CDNABasePairStepID(const CDNABasePairStep& bp)
{
    ResIDA = bp.ResIDA;
    ResIDB = bp.ResIDB;
    ResIDC = bp.ResIDC;
    ResIDD = bp.ResIDD;
    Step = bp.Step;
}

//------------------------------------------------------------------------------

bool CDNABasePairStepID::operator < (const CDNABasePairStepID& bp_id) const
{
    return( (ResIDA < bp_id.ResIDA) || (ResIDB < bp_id.ResIDB) || (ResIDC < bp_id.ResIDC) || (ResIDD < bp_id.ResIDD) );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

