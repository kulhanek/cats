#ifndef Qx3DNAStatisticsH
#define Qx3DNAStatisticsH
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
#include <Qx3DNAHelper.hpp>
#include <Qx3DNAStatisticsHelper.hpp>
#include <QCATsScriptable.hpp>
#include <QScriptable>
#include <ostream>
#include <map>
#include <set>

//------------------------------------------------------------------------------

class Qx3DNA;

//------------------------------------------------------------------------------

class Qx3DNAStatistics : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
    // constructor -----------------------------------------------------------------
        Qx3DNAStatistics(void);
        static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
        static void Register(QScriptEngine& engine);

// methods ---------------------------------------------------------------------
public slots:
    /// register new data
    /// registerData(x3DNA)
    QScriptValue registerData(void);

    /// clear all data
    /// clear()
    QScriptValue clear(void);

    /// print results
    /// printResults(name)
    QScriptValue printResults(void);

private:
    /// clear all data
    void ClearAll(void);

    /// print results
    void PrintLocalBPParams(std::ostream& vout);
    void PrintLocalBPStepParams(std::ostream& vout);
    void PrintLocalBPHelParams(std::ostream& vout);

    /// register data into the database
    void RegisterData(Qx3DNA* p_data);

    std::set<CDNABasePairID>                    BasePairIDs;
    std::set<CDNABasePairStepID>                BasePairStepIDs;

    std::map<CDNABasePairID,CLocalBPStatPtr>    LocalBPStat;
};


//------------------------------------------------------------------------------

#endif
