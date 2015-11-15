#ifndef Qx3DNAH
#define Qx3DNAH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

//------------------------------------------------------------------------------

class CLocalBPParams {
public:
    int         ID;
    std::string Name; // one base pair (A-A)
    double      Shear;
    double      Stretch;
    double      Stagger;
    double      Buckle;
    double      Propeller;
    double      Opening;
};

class CLocalBPStepParams {
public:
    int         ID;
    std::string Step; // two base pairs (AT/AT)
    double      Shift;
    double      Slide;
    double      Rise;
    double      Tilt;
    double      Roll;
    double      Twist;
};

class CLocalBPHelParams {
public:
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

/// 3D x3DNA

class Qx3DNA : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    Qx3DNA(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// methods ---------------------------------------------------------------------
public slots:
    /// perform core analysis
    /// analyze(snapshot[,selection])
    QScriptValue analyze(void);

    /// get local BP shear
    QScriptValue getLocalBPShear(void);

// access methods --------------------------------------------------------------
private:
    CFileName                       WorkDir;    // scratch directory
    CFileName                       x3dnaDir;   // 3DNA program directory
    std::vector<CLocalBPParams>     LocalBPParams;
    std::vector<CLocalBPStepParams> LocalBPStepParams;
    std::vector<CLocalBPHelParams>  LocalBPHelParams;

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

    /// read Local BP Params
    bool ReadSectionLocalBPParams(std::ifstream& ifs);

    /// read Local BP Step Params
    bool ReadSectionLocalBPStepParams(std::ifstream& ifs);

    /// read Local BP Helical Params
    bool ReadSectionLocalBPHelParams(std::ifstream& ifs);
};

//------------------------------------------------------------------------------

#endif
