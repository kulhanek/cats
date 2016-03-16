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
#include <Qx3DNAHelper.hpp>
#include <map>

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
    /// int getBPIndex(residA,residB)
    QScriptValue getBPIndex(void);

    /// get validity of local BP params
    /// bool areLocalBPParamsValid(index)
    QScriptValue areLocalBPParamsValid(void);

    /// get local BP params
    /// double getLocalBPXXX(index)
    QScriptValue getLocalBPShear(void);
    QScriptValue getLocalBPStretch(void);
    QScriptValue getLocalBPStagger(void);
    QScriptValue getLocalBPBuckle(void);
    QScriptValue getLocalBPPropeller(void);
    QScriptValue getLocalBPOpening(void);

    /// get validity of local BP Step params
    /// bool areLocalBPStepParamsValid(index)
    QScriptValue areLocalBPStepParamsValid(void);

    /// get local BP Step params
    /// double getLocalBPStepXXX(index)
    QScriptValue getLocalBPStepShift(void);
    QScriptValue getLocalBPStepSlide(void);
    QScriptValue getLocalBPStepRise(void);
    QScriptValue getLocalBPStepTilt(void);
    QScriptValue getLocalBPStepRoll(void);
    QScriptValue getLocalBPStepTwist(void);

    /// get validity of local BP Helical params
    /// bool areLocalBPHelParamsValid(index)
    QScriptValue areLocalBPHelParamsValid(void);

    /// get local BP Helical params
    /// double getLocalBPHelXXX(index)
    QScriptValue getLocalBPHelXdisp(void);
    QScriptValue getLocalBPHelYdisp(void);
    QScriptValue getLocalBPHelHrise(void);
    QScriptValue getLocalBPHelIncl(void);
    QScriptValue getLocalBPHelTip(void);
    QScriptValue getLocalBPHelHtwist(void);

// access methods --------------------------------------------------------------
private:
    CFileName                       WorkDir;            // scratch directory
    bool                            AutoReferenceMode;  // is autoreference mode enabled?

    // topology residue index <-> local translation
    std::map<int,int>               ResIDMap;

    // these two items are set by analyzeReference()
    std::map<int,CDNABasePair>      ReferenceBasePairs;     // list of base pairs from find_pairs
    std::map<int,CDNABasePairStep>  ReferenceBasePairSteps; // list of base pair steps from analyze

    // data from analyzed snapshot
    std::map<int,CDNABasePair>      BasePairs;              // list of base pairs from find_pairs
    std::map<int,CDNABasePairStep>  BasePairSteps;          // list of base pair steps from analyze
    std::vector<CLocalBP>           LocalBP;
    std::vector<CLocalBPStep>       LocalBPStep;
    std::vector<CLocalBPHel>        LocalBPHel;

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
    bool ReadSectionBP(std::ifstream& ifs,std::map<int,CDNABasePair>& bps);

    /// read reference BP Step
    bool ReadSectionBPStep(std::ifstream& ifs,std::map<int,CDNABasePairStep>& bpsteps);

    /// read Local BP
    bool ReadSectionLocalBP(std::ifstream& ifs);

    /// read Local BP Step
    bool ReadSectionLocalBPStep(std::ifstream& ifs);

    /// read Local BP Helical
    bool ReadSectionLocalBPHel(std::ifstream& ifs);

    friend class Qx3DNAStatistics;
};

//------------------------------------------------------------------------------

#endif
