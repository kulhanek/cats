#ifndef QNAStatHelperH
#define QNAStatHelperH
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

#include <CATsMainHeader.hpp>
#include <QNAHelper.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

//------------------------------------------------------------------------------

class CNALocalBPStat {
public:
    // constructor
    CNALocalBPStat(void);

public:
    int             NumOfSamples;
    CNALocalBPPar   Sum;
    CNALocalBPPar   Sum2;

    void RegisterData(const CNALocalBPPar& data);
};

//------------------------------------------------------------------------------

typedef boost::shared_ptr<CNALocalBPStat>   CNALocalBPStatPtr;

//------------------------------------------------------------------------------

class CNALocalBPStepStat {
public:
    // constructor
    CNALocalBPStepStat(void);

public:
    int                 NumOfSamples;
    CNALocalBPStepPar   Sum;
    CNALocalBPStepPar   Sum2;

    void RegisterData(const CNALocalBPStepPar& data);
};

//------------------------------------------------------------------------------

typedef boost::shared_ptr<CNALocalBPStepStat>   CNALocalBPStepStatPtr;

//------------------------------------------------------------------------------

class CNALocalBPHelStat {
public:
    // constructor
    CNALocalBPHelStat(void);

public:
    int                 NumOfSamples;
    CNALocalBPHelPar    Sum;
    CNALocalBPHelPar    Sum2;

    void RegisterData(const CNALocalBPHelPar& data);
};

//------------------------------------------------------------------------------

typedef boost::shared_ptr<CNALocalBPHelStat>   CNALocalBPHelStatPtr;

//------------------------------------------------------------------------------

#endif
