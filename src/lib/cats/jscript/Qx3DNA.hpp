#ifndef Qx3DNAH
#define Qx3DNAH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2015 Michal Ruzicka, DOPSAT
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

#include <QObject>
#include <QScriptValue>
#include <QScriptContext>
#include <QScriptable>
#include <QCATsScriptable.hpp>
#include <QTopology.hpp>
#include <QSnapshot.hpp>
#include <QSelection.hpp>
#include <FileName.hpp>
#include <map>

//------------------------------------------------------------------------------

class CLocalBP {
public:
    // constructor
    CLocalBP(void);         // basic data initialization

public:
    bool        Valid;      // determine if data are valid
    int         ID;
    std::string Name;       // one base pair (A-A)
    double      Shear;
    double      Stretch;
    double      Stagger;
    double      Buckle;
    double      Propeller;
    double      Opening;
};

class CLocalBPStep {
public:
    // TODO
    int         ID;
    std::string Step; // two base pairs (AT/AT)
    double      Shift;
    double      Slide;
    double      Rise;
    double      Tilt;
    double      Roll;
    double      Twist;
};

class CLocalBPHel {
public:
    // TODO
    int         ID;
    std::string Step; // two base pairs (AT/AT)
    double      Xdisp;
    double      Ydisp;
    double      Hrise;
    double      Incl;
    double      Tip;
    double      Htwist;
};

//------------------------------------------------------------------------------

class CDNABasePair {
public:
    // TODO
    int         ID;
    std::string Name;
    int         ResIDA;
    int         ResIDB;
};

//------------------------------------------------------------------------------

class CDNABasePairStep {
public:
    // TODO
    int         ID;
    std::string Step;
};

//------------------------------------------------------------------------------

/// 3D x3DNA

class Qx3DNA : public QObject, protected QScriptable, protected QCATsScriptable {
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
    QScriptValue setAutoreferenceMode(void);

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

    /// get local BP
    QScriptValue areLocalBPParamsValid();   // TODO - return true if data are valid, see CLocalBP::Valid
    QScriptValue getLocalBPShear(void);
    QScriptValue getLocalBPStretch(void);
    QScriptValue getLocalBPStagger(void);
    QScriptValue getLocalBPBuckle(void);
    QScriptValue getLocalBPPropeller(void);
    QScriptValue getLocalBPOpening(void);

    /// get local BP Step
    QScriptValue areLocalBPStepParamsValid();   // TODO
    QScriptValue getLocalBPStepShift(void);
    QScriptValue getLocalBPStepSlide(void);
    QScriptValue getLocalBPStepRise(void);
    QScriptValue getLocalBPStepTilt(void);
    QScriptValue getLocalBPStepRoll(void);
    QScriptValue getLocalBPStepTwist(void);

    /// get local BP Helical
    QScriptValue areLocalBPHelParamsValid();   // TODO
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

    // these two items should be set by analyzeReference()
    std::map<int,CDNABasePair>      ReferenceBasePairs;      // list of base pairs form find_pairs
    std::map<int,CDNABasePairStep>  ReferenceBasePairSteps;  // list of base pair steps form find_pairs

    // data from analyzed snapshot
    std::vector<CLocalBP>           LocalBP;
    std::vector<CLocalBPStep>       LocalBPStep;
    std::vector<CLocalBPHel>        LocalBPHel;

    /// clear all parsed results
    void ClearAll(void);

    /// create input data
    bool WriteInputData(QSnapshot* p_qsnap,QSelection* p_qsel);

    /// run analysis
    bool RunAnalysis(void);

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

    /// read Local BP
    bool ReadSectionLocalBP(std::ifstream& ifs);

    /// read Local BP Step
    bool ReadSectionLocalBPStep(std::ifstream& ifs);

    /// read Local BP Helical
    bool ReadSectionLocalBPHel(std::ifstream& ifs);
};

//------------------------------------------------------------------------------

#endif
