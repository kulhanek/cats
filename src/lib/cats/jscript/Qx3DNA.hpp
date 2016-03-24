#ifndef Qx3DNAH
#define Qx3DNAH
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
#include <map>

//------------------------------------------------------------------------------

/// type of parameters

enum EX3DNAParams {
    E3DP_LOCAL=1,
    E3DP_SIMPLE=1
};

//------------------------------------------------------------------------------

/// 3D x3DNA

class CATS_PACKAGE Qx3DNA : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    Qx3DNA(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue numOfBasePairs  READ getNumOfBasePairs WRITE setIsNotAllowed)
    Q_PROPERTY(QScriptValue numOfSteps      READ getNumOfSteps WRITE setIsNotAllowed)

// methods ---------------------------------------------------------------------
public slots:   
    /// set autoreference mode
    /// setAutoreferenceMode(set)
    QScriptValue setAutoReferenceMode(void);

    /// perform analysis on reference structure
    /// analyzeReference(snapshot[,selection])
    QScriptValue analyzeReference(void);

    /// perform core analysis
    /// analyze(snapshot[,selection])
    QScriptValue analyze(void);

    /// get number of base pairs
    /// int getNumOfBasePairs()
    QScriptValue getNumOfBasePairs(void);

    /// get number of steps
    /// int getNumOfSteps()
    QScriptValue getNumOfSteps(void);

    /// get index from residA and residB - it returns -1 if the BP was not analyzed
    /// residA-residB
    /// int getBPIndex(residA,residB)
    QScriptValue getBPIndex(void);

    /// get index from residA, residB and residC, residD - it returns -1 if the BP Step was not analyzed
    /// (residA-residB)/(residC-residD)
    /// int getBPStepIndex(residA,residB,residC,residD)
    QScriptValue getBPStepIndex(void);

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

    /// get validity of BP Step params
    /// bool areBPStepParamsValid(index)
    QScriptValue areBPStepParamsValid(void);

    /// get BP Step params
    /// double getBPStepXXX(index)
    QScriptValue getBPStepShift(void);
    QScriptValue getBPStepSlide(void);
    QScriptValue getBPStepRise(void);
    QScriptValue getBPStepTilt(void);
    QScriptValue getBPStepRoll(void);
    QScriptValue getBPStepTwist(void);

    /// get validity of BP Helical params
    /// bool areBPHelParamsValid(index)
    QScriptValue areBPHelParamsValid(void);

    /// get BP Helical params
    /// double getBPHelXXX(index)
    QScriptValue getBPHelXdisp(void);
    QScriptValue getBPHelYdisp(void);
    QScriptValue getBPHelHrise(void);
    QScriptValue getBPHelIncl(void);
    QScriptValue getBPHelTip(void);
    QScriptValue getBPHelHtwist(void);

// access methods --------------------------------------------------------------
private:
    CFileName                   WorkDir;            // scratch directory
    bool                        AutoReferenceMode;  // is autoreference mode enabled?

    // topology residue index <-> local translation
    std::map<int,int>           ResIDMap;

    // these two items are set by analyzeReference()
    std::map<int,CNABPID>       ReferenceBPIDs;     // list of base pairs from find_pairs
    std::map<int,CNABPStepID>   ReferenceBPStepIDs; // list of base pair steps from analyze

    // data from analyzed snapshot
    std::map<int,CNABPID>       BPIDs;              // list of base pairs from find_pairs
    std::map<int,CNABPStepID>   BPStepIDs;          // list of base pair steps from analyze
    std::vector<CNABPPar>       BPPar;
    std::vector<CNABPStepPar>   BPStepPar;
    std::vector<CNABPHelPar>    BPHelPar;

    /// clear all parsed results
    void ClearAll(void);

    /// create input data
    bool WriteInputData(QSnapshot* p_qsnap,QSelection* p_qsel);

    /// run analysis
    bool RunAnalysis(void);

    /// parse reference data
    bool ParseReferenceData(void);

    /// parse output data
    bool ParseOutputData(void);

    /// create PDB file from QSnapshot
    bool WritePDB(CAmberTopology* p_top,CAmberRestart* p_crd,CAmberMaskAtoms* p_mask,FILE* p_fout);

    /// write PDB remark
    bool WritePDBRemark(FILE* p_file,const CSmallString& string);

    /// get PDB res name
    const char* GetPDBResName(CAmberAtom* p_atom,CAmberResidue* p_res);

    /// get PDB atom name
    const char* GetPDBAtomName(CAmberAtom* p_atom,CAmberResidue* p_res);

    /// read reference BP
    bool ReadSectionBPIDs(std::ifstream& ifs,std::map<int,CNABPID>& bps);

    /// read reference BP Step
    bool ReadSectionBPStepIDs(std::ifstream& ifs,std::map<int,CNABPID>& bps,std::map<int,CNABPStepID>& bpsteps);

    /// read  BP
    bool ReadSectionBPPar(std::ifstream& ifs);

    /// read  BP Step
    bool ReadSectionBPStepPar(std::ifstream& ifs);

    /// read  BP Helical
    bool ReadSectionBPHelPar(std::ifstream& ifs);

    friend class QNAStat;
};

//------------------------------------------------------------------------------

#endif
