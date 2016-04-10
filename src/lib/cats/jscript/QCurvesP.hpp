#ifndef QCurvesPH
#define QCurvesPH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2016 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QObject>
#include <QScriptValue>
#include <QScriptContext>
#include <QScriptable>
#include <QCATsScriptable.hpp>
#include <QTopology.hpp>
#include <QSnapshot.hpp>
#include <QSelection.hpp>
#include <FileName.hpp>
#include <QNAHelper.hpp>
#include <Point.hpp>
#include <map>

//------------------------------------------------------------------------------

/// interface to Curves+

class CATS_PACKAGE QCurvesP : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QCurvesP(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue numOfBasePairs      READ getNumOfBasePairs WRITE setIsNotAllowed)

// methods ---------------------------------------------------------------------
public slots:   
    /// perform core analysis
    /// analyze(snapshot[,selection])
    QScriptValue analyze(void);

    /// get number of base pairs
    /// int getNumOfBasePairs()
    QScriptValue getNumOfBasePairs(void);

    /// get index from residA and residB - it returns -1 if the BP was not analyzed
    /// residA-residB
    /// int getBPIndex(residA,residB)
    QScriptValue getBPIndex(void);

    /// get validity of BP params
    /// bool areBPParamsValid(index)
    QScriptValue areBPParamsValid(void);

    /// get BP params
    /// double getBPXXX(index)
    QScriptValue getBPShear(void);
    QScriptValue getBPStretch(void);
    QScriptValue getBPStagger(void);
    QScriptValue getBPBuckle(void);
    QScriptValue getBPPropeller(void);
    QScriptValue getBPOpening(void);

// access methods --------------------------------------------------------------
private:
    CFileName                   WorkDir;            // scratch directory

    // topology residue index <-> local translation
    std::map<int,int>           ResIDMap;

    // data from analyzed snapshot
    std::map<int,CNABPID>       BPIDs;              // list of base pairs from find_pairs
    std::vector<CNABPPar>       BPPar;
    std::vector<CPoint>         HelAxisPositions;

    /// clear all parsed results
    void ClearAll(void);

    /// create input data
    bool WriteInputData(QSnapshot* p_qsnap,QSelection* p_qsel);

    /// run analysis
    bool RunAnalysis(void);

    /// parse find_pairs output data
    bool ParseFindPairOutputData(void);

    /// parse output data
    bool ParseOutputData(void);

    /// create PDB file from QSnapshot
    bool WritePDB(CAmberTopology* p_top,CAmberRestart* p_crd,CAmberMaskAtoms* p_mask,FILE* p_fout);

    /// read BP indexes
    bool ReadSectionBPIDs(std::ifstream& ifs,std::map<int,CNABPID>& bps);

    /// read  helical axis position
    bool ReadSectionHelPos(std::ifstream& ifs);

    friend class QNAStat;
    friend class QTinySpline;
};

//------------------------------------------------------------------------------

#endif
