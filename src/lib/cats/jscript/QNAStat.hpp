#ifndef QNAStatH
#define QNAStatH
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
#include <QNAStatHelper.hpp>
#include <QCATsScriptable.hpp>
#include <QScriptable>
#include <ostream>
#include <map>
#include <set>

//------------------------------------------------------------------------------

class Qx3DNA;

//------------------------------------------------------------------------------

class CATS_PACKAGE QNAStat : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QNAStat(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// methods ---------------------------------------------------------------------
public slots:
    /// register new data
    /// addSample(x3DNA[,options])
    QScriptValue addSample(void);

    /// clear all data
    /// clear()
    QScriptValue clear(void);

    /// print results
    /// printResults(name[,options])
    QScriptValue printResults(void);

    /// set detection of flipped helical axis
    /// enableFlipHelAxis(set)
    QScriptValue enableFlipHelAxis(void);

    /// is flipped helical axis detected?
    /// bool isFlipHelAxisEnabled()
    QScriptValue isFlipHelAxisEnabled(void);

private:
    /// clear all data
    void ClearAll(void);

    /// print results
    void PrintBPParams(std::ofstream& vout);
    void PrintBPStepParams(std::ofstream& vout);
    void PrintBPHelParams(std::ofstream& vout);
    void PrintPParams(std::ofstream& vout);

    /// register data into the database
    void RegisterBPData(Qx3DNA* p_data);
    void RegisterBPStepData(Qx3DNA* p_data);
    void RegisterBPHelData(Qx3DNA* p_data);
    void RegisterPData(Qx3DNA* p_data);

    bool                                    FlipHelAxisEnabled;   // handle flipped axis cases
    std::set<CNABPID>                       BPIDs;
    std::set<CNABPStepID>                   BPStepIDs;

    int                                     NumOfSnapshots;
    std::map<CNABPID,CNABPStatPtr>          BPStat;
    std::map<CNABPStepID,CNABPStepStatPtr>  BPStepStat;
    std::map<CNABPStepID,CNABPHelStatPtr>   BPHelStat;
    std::map<CNABPStepID,CNAPStatPtr>       PStat;
};


//------------------------------------------------------------------------------

#endif
