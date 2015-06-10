// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iomanip>
#include <ErrorSystem.hpp>
#include <PeriodicTable.hpp>
#include <FileName.hpp>

#include <SmallTimeAndDate.hpp>
#include <AmberMaskAtoms.hpp>

#include "TopCrd2Cpmd.hpp"

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopCrd2Cpmd::CTopCrd2Cpmd(void)
{
    NumberOfSoluteAtoms = 0;
    NumberOfSoluteResidues = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopCrd2Cpmd::Init(int argc,char* argv[])
{
    // encode program options
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    vout.Attach(Console);
    if( Options.GetOptVerbose() ) {
        vout.Verbosity(CVerboseStr::high);
    } else {
        vout.Verbosity(CVerboseStr::low);
    }

    // print header --------------------------------------------------------------
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << "" <<  endl;
    vout << "# ==============================================================================" <<  endl;
    vout << "# topcrd2cpmd (CATs utility) started at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" <<  endl;
    vout << "# Input AMBER topology    : " << Options.GetArgTopologyName() << endl;
    if( Options.GetArgCrdName() != "-" ) {
    vout << "# Input AMBER coordinates : " << Options.GetArgCrdName() << endl;
    } else {
    vout << "# Input AMBER coordinates : - (standard input)" <<  endl;
    }
    vout << "# QM zone specification   : " << Options.GetArgMaskSpec() << endl;
    vout << "# Output prefix           : " << Options.GetArgPrefixName() << endl;
    vout << "# ------------------------------------------------------------------------------" << endl;

    return( SO_CONTINUE );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2Cpmd::Run(void)
{
    if( LoadInputData() == false ) return(false);
    if( SystemInfo() == false ) return(false);
    if( PrepareSystem() == false ) return(false);
    if( QMZoneInfo() == false ) return(false);

    if( WriteCPMDFiles() == false ) return(false);
    if( WriteGromosFiles() == false ) return(false);

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Cpmd::LoadInputData(void)
{
    vout << endl;
    vout << "# ==============================================================================" <<  endl;
    vout << "# 1) Reading input data ..." << endl;
    vout << "# ==============================================================================" <<  endl;

    // load topology
    if( Topology.Load(Options.GetArgTopologyName()) == false ) {
        CSmallString error;
        error << "unable to load input AMBER topology: " << Options.GetArgTopologyName();
        ES_ERROR(error);
        return(false);
    }
    Topology.BuidListOfNeighbourAtoms();

    // load coordinates
    Coordinates.AssignTopology(&Topology);
    if( Coordinates.Load(Options.GetArgCrdName()) == false ) {
        CSmallString error;
        error << "unable to load input coordinates: " << Options.GetArgCrdName();
        ES_ERROR(error);
        return(false);
    }

    // init mask ----------------------------------------------
    Mask.AssignTopology(&Topology);
    Mask.AssignCoordinates(&Coordinates);

    if( Mask.SetMask(Options.GetArgMaskSpec()) == false ){
        CSmallString error;
        error << "unable to set mask defining QM zone";
        ES_ERROR(error);
        return(false);
    }

    vout << "   Success." << endl;
    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Cpmd::SystemInfo(void)
{
    vout << endl;
    vout << "# ==============================================================================" <<  endl;
    vout << "# 2) System info ..." << endl;
    vout << "# ==============================================================================" <<  endl;
    Topology.PrintInfo();

    vout << "Total number of atoms         : " << Topology.AtomList.GetNumberOfAtoms() << endl;
    NumberOfSoluteAtoms = Topology.BoxInfo.GetNumberOfSoluteAtoms();
    vout << "Solute atoms (no cappings)    : " << NumberOfSoluteAtoms << endl;
    vout << "Total number of residues      : " << Topology.ResidueList.GetNumberOfResidues() << endl;

    // count residues until the number
    int solatoms = 0;
    NumberOfSoluteResidues = 0;
    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++){
        CAmberResidue* p_res = Topology.ResidueList.GetResidue(i);
        solatoms += p_res->GetNumberOfAtoms();
        if( solatoms >= NumberOfSoluteAtoms ){
            NumberOfSoluteResidues = i + 1;
            break;
        }
    }

    vout << "Solute residues (no cappings) : " << NumberOfSoluteResidues << endl;

    // box info from restart file
    CPoint dimm = Coordinates.GetBox();
    CPoint angles = Coordinates.GetAngles();

    vout << endl;
    vout << "Box info from coordinates:" << endl;
    vout << fixed << setw(12) << setprecision(3);
    vout << "    a     = " << dimm.x << " [A]" << endl ;
    vout << "    b     = " << dimm.y << " [A]" << endl;
    vout << "    c     = " << dimm.z << " [A]" << endl;
    vout << "    alpha = " << angles.x << endl;
    vout << "    beta  = " << angles.y << endl;
    vout << "    gamma = " << angles.z << endl;

    if( (Topology.BoxInfo.GetType() != AMBER_BOX_STANDARD) ||
        (fabs(angles.x-90.0) > 2.0) || (fabs(angles.y-90.0) > 2.0) || (fabs(angles.z-90.0) > 2.0) ){
        CSmallString error;
        error << "system must have a rectangular box";
        ES_ERROR(error);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Cpmd::PrepareSystem(void)
{
    vout << endl;
    vout << "# ==============================================================================" <<  endl;
    vout << "# 3) Prepare system ..." << endl;
    vout << "# ==============================================================================" <<  endl;

    // center solute
    vout << "  Centering solute to box center ..." << endl;

    int nsolutemols = Topology.BoxInfo.GetNumberOfSoluteMolecules();
    vout << "    Number of solute molecules                             : " <<  nsolutemols << endl;

    for(int i=nsolutemols-1; i >=0; i--){
        if( Topology.BoxInfo.GetNumberOfAtomsInMolecule(i) == 1 ) nsolutemols--;
        if( Topology.BoxInfo.GetNumberOfAtomsInMolecule(i) > 1 ) break;
    }
    vout << "    Number of solute molecules (excluding monovalent ions) : " <<  nsolutemols << endl;

    int nsoluteatoms = 0;
    for(int i=0; i < nsolutemols; i++){
        nsoluteatoms += Topology.BoxInfo.GetNumberOfAtomsInMolecule(i);
    }

    vout << "    Number of solute atoms (excluding monovalent ions)     : " <<  nsoluteatoms << endl;

    // get center of mass
    double totmass = 0;
    CPoint com;

    for(int i=0; i < nsoluteatoms; i++){
        double mass = Topology.AtomList.GetAtom(i)->GetMass();
        CPoint pos  = Coordinates.GetPosition(i);
        com += pos*mass;
        totmass += mass;
    }
    if( totmass > 0 ){
        com /= totmass;
    }

    CPoint box = Coordinates.GetBox();
    CPoint move = - com + box/2.0;

    for(int i=0; i < Coordinates.GetNumberOfAtoms(); i++){
        CPoint pos = Coordinates.GetPosition(i);
        pos = pos + move;
        Coordinates.SetPosition(i,pos);
    }


    // image by molecules
    vout << "  Imaging the whole system (by molecules) ..." << endl;

    Topology.InitMoleculeIndexes();
    int start_index = 0;
    while( start_index < Topology.AtomList.GetNumberOfAtoms() ) {
        int mol_id = Topology.AtomList.GetAtom(start_index)->GetMoleculeIndex();

        // calculate molecule COM
        CPoint  com;
        double  tmass = 0.0;
        int     count = 0;
        for(int i=start_index; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
            if( mol_id != Topology.AtomList.GetAtom(i)->GetMoleculeIndex() ) break;
            double mass = Topology.AtomList.GetAtom(i)->GetMass();
            com += Coordinates.GetPosition(i)*mass;
            tmass += mass;
            count++;
        }
        if( tmass != 0.0 ) com /= tmass;

        // image com
        CPoint  icom;
        icom = Topology.BoxInfo.ImagePoint(com,0,0,0,false,false);
        icom = icom - com;

        // move molecule
        for(int i=start_index; i < start_index+count; i++) {
            CPoint pos = Coordinates.GetPosition(i);
            pos += icom;
            Coordinates.SetPosition(i,pos);
        }
        start_index += count;
    }


    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Cpmd::QMZoneInfo(void)
{
    vout << endl;
    vout << "# ==============================================================================" <<  endl;
    vout << "# 4) QM zone info ..." << endl;
    vout << "# ==============================================================================" <<  endl;

    // print mask and detect capping atoms -------------------------------------

    vout << endl;
    vout << "# ID Index  Name Type  Z  ResID ResN     x[A]         y[A]         z[A]    " << endl;
    vout << "# -- ------ ---- ---- --- ----- ---- ------------ ------------ ------------" << endl;
    int id = 0;
    int capid = 0;
    for(int i=0 ; i < Mask.GetNumberOfTopologyAtoms(); i++) {
        CAmberAtom* p_atm = Mask.GetSelectedAtom(i);
        if( p_atm == NULL ) continue;
        bool capping = false;
        for(int j=0; j < p_atm->GetNumberOfNeighbourAtoms(); j++){
            if( Mask.IsAtomSelected(p_atm->GetNeighbourAtomIndex(j)) == false ){
                CQMLink link;
                link.QMIndex = p_atm->GetAtomIndex();
                link.MMIndex = p_atm->GetNeighbourAtomIndex(j);
                link.HLinkAtmIndex = NumberOfSoluteAtoms + capid;
                link.HLinkResIndex = NumberOfSoluteResidues + capid;
                capid++;

                // calculate link atom position
                CPoint posA = Coordinates.GetPosition(link.QMIndex);
                CPoint posB = Coordinates.GetPosition(link.MMIndex);
                CPoint diff = posB - posA;
                diff.Normalize();
                CPoint posH = posA + diff;
                link.HLinkPos = posH;
                QMLinks.push_back(link);
                capping = true;
            }
        }

        if( capping == true ){
            vout << "*";
        } else {
            vout << " ";
        }
        vout << setw(3) << id + 1 << " ";
        vout << setw(6) << p_atm->GetAtomIndex() + 1 << " ";
        vout << setw(4) << p_atm->GetName() << " ";
        vout << setw(4) << p_atm->GetType() << " ";
        int z = p_atm->GetAtomicNumber();
        if( z == 0 ) z = p_atm->GuessZ();
        vout << setw(3) << z << " ";
        CAmberResidue* p_res = p_atm->GetResidue();
        vout << setw(5) << p_res->GetIndex() + 1 << " ";
        vout << setw(4) << p_res->GetName() << " ";
        CPoint pos = Coordinates.GetPosition(p_atm->GetAtomIndex());
        vout << fixed;
        vout << setw(12) << setprecision(4) << pos.x << " ";
        vout << setw(12) << setprecision(4) << pos.y << " ";
        vout << setw(12) << setprecision(4) << pos.z << endl;
        id++;
    }

    for(size_t i=0 ; i < QMLinks.size(); i++) {
        // "# -- ------ ---- ---- --- ----- ----"
        vout << "C";
        vout << setw(3) << id + 1 << " ";
        QMLinks[i].ID = id;
        vout << setw(6) << QMLinks[i].HLinkAtmIndex + 1 << " ";
        vout << "H    DUM    1 ";
        vout << setw(5) << QMLinks[i].HLinkResIndex + 1 << " ";
        vout << "DUM  ";
        CPoint pos = QMLinks[i].HLinkPos;
        vout << fixed;
        vout << setw(12) << setprecision(4) << pos.x << " ";
        vout << setw(12) << setprecision(4) << pos.y << " ";
        vout << setw(12) << setprecision(4) << pos.z << endl;
        id++;
    }

    vout << endl;
    vout << "Number of QM atoms      : " << Mask.GetNumberOfSelectedAtoms() << endl;
    vout << "Number of capping atoms : " << QMLinks.size() << endl;

    // write QM zone as xyz file -----------------------------------------------
    CFileName full_name;
    full_name = Options.GetArgPrefixName() + "_qmzone.xyz";

    vout << endl;
    vout << "# Writing " << full_name << " file ..." << endl;
    ofstream ofs;

    ofs.open(full_name);
    if( ! ofs ){
        CSmallString error;
        error << "unable to open file for writing " << full_name;
        ES_ERROR(error);
        return(false);
    }
    ofs << Mask.GetNumberOfSelectedAtoms() + QMLinks.size() << endl;
    ofs << "QM zone" << endl;
    for(int i=0 ; i < Mask.GetNumberOfTopologyAtoms(); i++) {
        CAmberAtom* p_atm = Mask.GetSelectedAtom(i);
        if( p_atm == NULL ) continue;
        CPoint pos = Coordinates.GetPosition(p_atm->GetAtomIndex());
        int z = p_atm->GetAtomicNumber();
        if( z == 0 ) z = p_atm->GuessZ();
        ofs << setw(2) << PeriodicTable.GetSymbol(z) << " ";
        ofs << fixed;
        ofs << setw(12) << setprecision(4) << pos.x << " ";
        ofs << setw(12) << setprecision(4) << pos.y << " ";
        ofs << setw(12) << setprecision(4) << pos.z << endl;
    }
    for(size_t i=0 ; i < QMLinks.size(); i++) {
        CPoint pos = QMLinks[i].HLinkPos;
        ofs << setw(2) << PeriodicTable.GetSymbol(1) << " ";
        ofs << fixed;
        ofs << setw(12) << setprecision(4) << pos.x << " ";
        ofs << setw(12) << setprecision(4) << pos.y << " ";
        ofs << setw(12) << setprecision(4) << pos.z << endl;
    }
    ofs.close();

    // write QM zone capping as xyz file -----------------------------------------------
    full_name = Options.GetArgPrefixName() + "_capping.xyz";

    vout << "# Writing " << full_name << " file ..." << endl;

    ofs.open(full_name);
    if( ! ofs ){
        CSmallString error;
        error << "unable to open file for writing " << full_name;
        ES_ERROR(error);
        return(false);
    }
    ofs << QMLinks.size() << endl;
    ofs << "QM capping atoms" << endl;
    for(size_t i=0 ; i < QMLinks.size(); i++) {
        CPoint pos = QMLinks[i].HLinkPos;
        ofs << setw(2) << PeriodicTable.GetSymbol(1) << " ";
        ofs << fixed;
        ofs << setw(12) << setprecision(4) << pos.x << " ";
        ofs << setw(12) << setprecision(4) << pos.y << " ";
        ofs << setw(12) << setprecision(4) << pos.z << endl;
    }
    ofs.close();

    // list link bonds ---------------------------------------------------------
    vout << endl;
    vout << "# Link bonds statistics ..." << endl;

    vout << endl;
    vout << "#  Capping              QM atom                         MM atom            " << endl;
    vout << "# --------- ------------------------------- -------------------------------" << endl;
    vout << "# ID Index  Index  Name Type  Z  ResID ResN Index  Name Type  Z  ResID ResN" << endl;
    vout << "# -- ------ ------ ---- ---- --- ----- ---- ------ ---- ---- --- ----- ----" << endl;
    for(size_t i=0 ; i < QMLinks.size(); i++) {
        vout << " ";
        vout << setw(3) << QMLinks[i].ID + 1 << " ";
        vout << setw(6) << QMLinks[i].HLinkAtmIndex + 1 << " ";
        CAmberAtom* p_atm;
        p_atm = Topology.AtomList.GetAtom(QMLinks[i].QMIndex);
        vout << setw(6) << p_atm->GetAtomIndex() + 1 << " ";
        vout << setw(4) << p_atm->GetName() << " ";
        vout << setw(4) << p_atm->GetType() << " ";
        int z = p_atm->GetAtomicNumber();
        if( z == 0 ) z = p_atm->GuessZ();
        vout << setw(3) << z << " ";
        CAmberResidue* p_res;
        p_res = p_atm->GetResidue();
        vout << setw(5) << p_res->GetIndex() + 1 << " ";
        vout << setw(4) << p_res->GetName() << " ";

        p_atm = Topology.AtomList.GetAtom(QMLinks[i].MMIndex);
        vout << setw(6) << p_atm->GetAtomIndex() + 1 << " ";
        vout << setw(4) << p_atm->GetName() << " ";
        vout << setw(4) << p_atm->GetType() << " ";
        z = p_atm->GetAtomicNumber();
        if( z == 0 ) z = p_atm->GuessZ();
        vout << setw(3) << z << " ";
        p_res = p_atm->GetResidue();
        vout << setw(5) << p_res->GetIndex() + 1 << " ";
        vout << setw(4) << p_res->GetName() << endl;
    }

    // test some bad boundaries
    // FIXME

    // detect QM zone total charge ---------------------------------------------
    vout << endl;
    vout << "# QM zone total charge ..." << endl;

// this does not work
//    OBMol mol;
//    OBAtomTyper typer;
//    for(int i=0 ; i < Mask.GetNumberOfTopologyAtoms(); i++) {
//        CAmberAtom* p_atm = Mask.GetSelectedAtom(i);
//        if( p_atm == NULL ) continue;
//        CPoint pos = Coordinates.GetPosition(p_atm->GetAtomIndex());
//        int z = p_atm->GetAtomicNumber();
//        if( z == 0 ) z = p_atm->GuessZ();
//        OBAtom* p_ob_atom = mol.NewAtom();
//        p_ob_atom->SetAtomicNum(z);
//        p_ob_atom->SetVector(pos.x, pos.y, pos.z);
//    }
//    for(size_t i=0 ; i < QMLinks.size(); i++) {
//        CPoint pos = QMLinks[i].HLinkPos;
//        OBAtom* p_ob_atom = mol.NewAtom();
//        p_ob_atom->SetAtomicNum(1);
//        p_ob_atom->SetVector(pos.x, pos.y, pos.z);
//    }
//    typer.AssignHyb(mol);
//    typer.AssignTypes(mol);
//    mol.ConnectTheDots();
//    mol.PerceiveBondOrders();

//    OBChargeModel* p_gasteiger = OBChargeModel::FindType("gasteiger");
//    if (p_gasteiger) {
//        p_gasteiger->ComputeCharges(mol);
//    }

//    double totcharge = 0.0;
//    for(unsigned int i = 1; i <= mol.NumAtoms(); i++) {
//        OBAtom* p_atom = mol.GetAtom(i);
//        totcharge += p_atom->GetPartialCharge();
//    }
//    DetectedQMZoneCharge = floor(totcharge + 0.5);

    QMZoneCharge = Options.GetOptQMCharge();
    vout << "  User provided charge : " << Options.GetOptQMCharge() << endl;
    vout << "  Used charge          : " << QMZoneCharge << endl;

    return(true);
}

//------------------------------------------------------------------------------




//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTopCrd2Cpmd::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# topcrd2cpmd terminated at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;

    if( ErrorSystem.IsError() || Options.GetOptVerbose() ){
        ErrorSystem.PrintErrors(vout);
    }
    vout << endl;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

