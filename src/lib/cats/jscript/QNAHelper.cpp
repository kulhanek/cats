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
#include <algorithm>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>

//------------------------------------------------------------------------------

using namespace std;
using namespace boost;
using namespace boost::algorithm;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CNABPPar::CNABPPar(void)
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

CNABPStepPar::CNABPStepPar(void)
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

CNABPHelPar::CNABPHelPar(void)
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

CNAPPar::CNAPPar(void)
{
    Valid = false;
    ID = -1;
    Step = "nd";
    Xp = 0.0;
    Yp = 0.0;
    Zp = 0.0;
    XpH = 0.0;
    YpH = 0.0;
    ZpH = 0.0;
    Form = -1;
}

//------------------------------------------------------------------------------

CNAHelAxisVec::CNAHelAxisVec(void)
{
    Valid = false;
    ID = -1;
    Step = "nd";
    Hx = 0.0;
    Hy = 0.0;
    Hz = 0.0;
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

bool CNABPID::MakeCanonical(void)
{
    if( ResIDA < ResIDB ) return(false); // already canonical

    // revert order
    int tmp = ResIDB;
    ResIDB = ResIDA;
    ResIDA = tmp;

    // reverse string
    std::reverse(Name.begin(), Name.end());

    // helical axis is flipped
    return(true);
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

bool CNABPStepID::MakeCanonical(void)
{
    if( ResIDA < ResIDD ) return(false); // already canonical

    // revert order
    int tmp = ResIDA;
    ResIDA = ResIDC;
    ResIDC = tmp;

    tmp = ResIDB;
    ResIDB = ResIDD;
    ResIDD = tmp;

    // reverse string
    vector<string> list;
    split(list,Step,is_any_of("/"));
    reverse(list.begin(), list.end());
    Step = join(list,"/");

    // helical axis is flipped
    return(true);
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
