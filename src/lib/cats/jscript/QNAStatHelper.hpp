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

class CNABPStat {
public:
    // constructor
    CNABPStat(void);

public:
    int         NumOfSamples;
    int         RevertedCases;
    CNABPPar    Sum;
    CNABPPar    Sum2;

    void RegisterData(const CNABPPar& data,bool flipped=false);
};

//------------------------------------------------------------------------------

typedef boost::shared_ptr<CNABPStat>   CNABPStatPtr;

//------------------------------------------------------------------------------

class CNABPStepStat {
public:
    // constructor
    CNABPStepStat(void);

public:
    int             NumOfSamples;
    int             RevertedCases;
    CNABPStepPar    Sum;
    CNABPStepPar    Sum2;

    void RegisterData(const CNABPStepPar& data,bool flipped=false);
};

//------------------------------------------------------------------------------

typedef boost::shared_ptr<CNABPStepStat>   CNABPStepStatPtr;

//------------------------------------------------------------------------------

class CNABPHelStat {
public:
    // constructor
    CNABPHelStat(void);

public:
    int             NumOfSamples;
    int             RevertedCases;
    CNABPHelPar     Sum;
    CNABPHelPar     Sum2;

    void RegisterData(const CNABPHelPar& data,bool flipped=false);
};

//------------------------------------------------------------------------------

typedef boost::shared_ptr<CNABPHelStat>   CNABPHelStatPtr;

//------------------------------------------------------------------------------

class CNAPStat {
public:
    // constructor
    CNAPStat(void);

public:
    int             NumOfSamples;
    CNAPPar         Sum;
    CNAPPar         Sum2;
    int             NumOfAForm;
    int             NumOfBForm;
    int             NumOfAlikeForm;
    int             NumOfBlikeForm;
    int             NumOfTAlikeForm;

    void RegisterData(const CNAPPar& data,bool flipped=false);
};

//------------------------------------------------------------------------------

typedef boost::shared_ptr<CNAPStat>   CNAPStatPtr;

//------------------------------------------------------------------------------

#endif
