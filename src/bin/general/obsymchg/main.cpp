// =============================================================================
// This file is part of OBSymChg.
//    Copyright (C) 2021 Petr Kulhanek, kulhanek@chemi.muni.cz
//
// OBSymChg is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// OBSymChg is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OBSymChg. If not, see <http://www.gnu.org/licenses/>.
// =============================================================================

#include "options.hpp"
#include <openbabel/mol.h>
#include <openbabel/graphsym.h>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>
#include <ErrorSystem.hpp>
#include <SmallTimeAndDate.hpp>
#include <InfMol.hpp>
#include <SimpleVector.hpp>
#include <iomanip>

//------------------------------------------------------------------------------

using namespace std;
using namespace OpenBabel;

//------------------------------------------------------------------------------

COptions                    Options;
CTerminalStr                Console;
CVerboseStr                 vout;
CInfMol                     Mol;
CSimpleVector<double>       Charges;
CSimpleVector<int>          ICharges;
std::vector<unsigned int>   SymmClasses;

//------------------------------------------------------------------------------

class CSymStat{
public:
    CSymStat(void);
    double  Number;
    double  MCharge;    // mean value of the charge
    double  M2Charge;
};

//---------------------------

CSymStat::CSymStat(void)
{
     Number = 0;
     MCharge = 0.0;
     M2Charge = 0.0;
}

//---------------------------

std::map<unsigned int,CSymStat>   SCStat;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int main(int argc,char* argv[])
{
    // encode program options
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

// attach verbose stream to cout and set desired verbosity level
    vout.Attach(Console);
    if( Options.GetOptVerbose() ) {
        vout.Verbosity(CVerboseStr::debug);
    } else {
        vout.Verbosity(CVerboseStr::high);
    }

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# obsymchg started at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;

    vout << "# Input molecule       : " << Options.GetArgMolName() << endl;
    vout << "# Input charges        : " << Options.GetArgQIn() << endl;
    vout << "# Output charges       : " << Options.GetArgQOut() << endl;
    
// read data
    vout << endl;
    vout << ":::::::::::::::::::::::::::::::: Reading Data ::::::::::::::::::::::::::::::::" << endl;

    // read molecules --------------------------------
    vout << endl;
    vout << "1) Reading molecule ..." << endl;
    if(Mol.ReadMol(Options.GetArgMolName(),Options.GetOptMolFormat()) == false) return(false);
    vout << "   Number of atoms = " << Mol.NumAtoms() << endl;

    vout << endl;
    vout << "2) Reading charges ..." << endl;
    Charges.CreateVector(Mol.NumAtoms());
    ifstream qin;
    qin.open(Options.GetArgQIn());
    for(unsigned int i=0; i < Mol.NumAtoms(); i++){
        qin >> Charges[i];
    }
    qin.close();

// process data
    vout << endl;
    vout << "::::::::::::::::::::::::::::::: Processing Data ::::::::::::::::::::::::::::::" << endl;

    vout << endl;
    vout << "1) Molecule info ..." << endl;
    OBGraphSym  graph_sym(&Mol);
    graph_sym.GetSymmetry(SymmClasses);

    double tot_charge = 0.0;

    vout << "# ID  Sym  Z       X          Y         Z       charge     SC  " << endl;
    vout << "# --- --- --- ---------- ---------- ---------- ---------- -----" << endl;
    for(unsigned int i=0; i < Mol.NumAtoms(); i++){
        OBAtom* p_atom = Mol.GetAtom(i+1);
        vout << setw(5) << i+1 << " ";
        vout << setw(3) << etab.GetSymbol(p_atom->GetAtomicNum()) << " ";
        vout << setw(3) << p_atom->GetAtomicNum() << " ";
        vout << fixed << setw(10) << setprecision(4) << p_atom->GetX() << " ";
        vout << setw(10) << setprecision(4) << p_atom->GetY() << " ";
        vout << setw(10) << setprecision(4) << p_atom->GetZ() << " ";
        vout << setw(10) << setprecision(6) << Charges[i] << " ";
        vout << setw(5) << SymmClasses[i] << endl;
        SCStat[SymmClasses[i]] = CSymStat();
        tot_charge += Charges[i];
    }
    vout << "# --- --- --- ---------- ---------- ---------- ---------- -----" << endl;
    int itchg = round(tot_charge);
    vout << "# Total charge = " << setw(2) << itchg;
    vout << " (" << fixed << setw(10) << setprecision(6) << tot_charge << ")" << endl;

    vout << endl;
    vout << "2) Symmentry classes statistics ..." << endl;
    vout << "   Number of symmetry classes = " << SCStat.size() << endl;

    for(unsigned int i=0; i < Mol.NumAtoms(); i++){
        int sc = SymmClasses[i];
        SCStat[sc].Number++;
        double n = SCStat[sc].Number;
        double m1 = SCStat[sc].MCharge;
        double d1 = Charges[i]-m1;
        SCStat[sc].MCharge = m1 + d1/n;
        double m2 = SCStat[sc].MCharge;
        double d2 = Charges[i]-m2;
        SCStat[sc].M2Charge = SCStat[sc].M2Charge + d1*d2;
    }

    vout << endl;
    vout << "# SC    N    <<charge>   s(charge)" << endl;
    vout << "# --- ----- ---------- ----------" << endl;
    std::map<unsigned int,CSymStat>::iterator it = SCStat.begin();
    std::map<unsigned int,CSymStat>::iterator ie = SCStat.end();

    while( it != ie ){
        unsigned int sc = it->first;
        CSymStat stat = it->second;
        vout << setw(5) << sc << " " << fixed;
        vout << setw(5) << setprecision(0) << stat.Number << " ";
        vout << setw(10) << setprecision(6) << stat.MCharge << " ";
        double sigc = sqrt(stat.M2Charge /  stat.Number);
        vout << scientific << setw(10) << setprecision(4) << sigc << endl;
        it++;
    }

    // fix residual
    // get int representation of charges
    ICharges.CreateVector(Mol.NumAtoms());
    int itot = 0;
    for(unsigned int i=0; i < Mol.NumAtoms(); i++){
        CSymStat stat = SCStat[SymmClasses[i]];
        ICharges[i] = stat.MCharge * 1000000;
        itot += ICharges[i];
    }

    int residuals = itchg*1000000 - itot;
    vout << endl;
    vout << "# Sum of residuals (10^-6) = " << residuals << endl;
    vout << endl;

    while( residuals != 0 ){
        for(unsigned int i=0; i < Mol.NumAtoms(); i++){
            if( residuals > 0 ){
                ICharges[i]++;
                residuals--;
            } else {
                ICharges[i]--;
                residuals++;
            }
            if( residuals == 0 ) break;
        }
    }

    tot_charge = 0.0;
    vout << "# ID  Sym  Z       X          Y         Z       charge     SC  " << endl;
    vout << "# --- --- --- ---------- ---------- ---------- ---------- -----" << endl;
    for(unsigned int i=0; i < Mol.NumAtoms(); i++){
        OBAtom* p_atom = Mol.GetAtom(i+1);
        vout << setw(5) << i+1 << " ";
        vout << setw(3) << etab.GetSymbol(p_atom->GetAtomicNum()) << " ";
        vout << setw(3) << p_atom->GetAtomicNum() << " ";
        vout << fixed << setw(10) << setprecision(4) << p_atom->GetX() << " ";
        vout << setw(10) << setprecision(4) << p_atom->GetY() << " ";
        vout << setw(10) << setprecision(4) << p_atom->GetZ() << " ";
        double ch = (double)ICharges[i] / 1000000.0;
        vout << setw(10) << setprecision(6) << ch << " ";
        vout << setw(5) << SymmClasses[i] << endl;
        SCStat[SymmClasses[i]] = CSymStat();
        tot_charge += ch;
    }
    vout << "# --- --- --- ---------- ---------- ---------- ---------- -----" << endl;
    vout << "# Total charge = " << fixed << setw(10) << setprecision(6) << tot_charge << endl;

// writing data
    vout << endl;
    vout << "::::::::::::::::::::::::::::::::: Writing Data :::::::::::::::::::::::::::::::" << endl;

    vout << endl;
    vout << "1) Writing charges ..." << endl;
    ofstream qout;
    qout.open(Options.GetArgQOut());

    for(unsigned int i=0; i < Mol.NumAtoms(); i++){
        double ch = (double)ICharges[i] / 1000000.0;
        qout << " " << fixed << setw(9) << setprecision(6) << ch;
        if( i%8 == 7 ) qout << endl;
    }
    qout << endl;
    qout.close();

// finalize
    dt.GetActualTimeAndDate();

    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# obsymchg terminated at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;

    if( ErrorSystem.IsError() || Options.GetOptVerbose() ){
        ErrorSystem.PrintErrors(vout);
    }

    vout << endl;    
    
    return( 0 );
}

/*

    // convert to OBMol
    OBMol  obmol;

    int at_lid = 1;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++){
        CAmberAtom* p_atom = Topology.AtomList.GetAtom(i);
        OBAtom* p_ob_atom = obmol.NewAtom();
        p_ob_atom->SetAtomicNum(p_atom->GetAtomicNumber());
        CPoint pos = Coords.GetPosition(i);
        p_ob_atom->SetVector(pos.x, pos.y, pos.z);
        at_lid++;
    }

    // add bonds
    for(int i=0; i < Topology.BondList.GetNumberOfBonds(); i++){
        CAmberBond*  p_bond = Topology.BondList.GetBond(i);
        int ob_a1_id = p_bond->GetIB() + 1;
        int ob_a2_id = p_bond->GetJB() + 1;
        int order = 1;
        // create bond
        obmol.AddBond(ob_a1_id, ob_a2_id, order);
    }

    // generate symmetry classes
    OBGraphSym  graph_sym(&obmol);
    graph_sym.GetSymmetry(SymmClasses);

    return(true);*/


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


