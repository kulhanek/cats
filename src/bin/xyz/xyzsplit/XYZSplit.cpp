// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2017 Petr Kulhanek, kulhanek@chemi.muni.cz
//
// FFDevel is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// FFDevel is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with FFDevel. If not, see <http://www.gnu.org/licenses/>.
// =============================================================================

#include <ErrorSystem.hpp>
#include <SmallTimeAndDate.hpp>
#include <PeriodicTable.hpp>
#include "XYZSplit.hpp"
#include <algorithm>
#include <list>

//------------------------------------------------------------------------------

using namespace std;

MAIN_ENTRY(CXYZSplit)

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CXYZSplit::CXYZSplit(void)
{
    NumberOfMolecules = 0;
}

//------------------------------------------------------------------------------

CXYZSplit::~CXYZSplit(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CXYZSplit::Init(int argc,char* argv[])
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
    vout << "# xyzsplit (CATs utility)  started at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;

    return( SO_CONTINUE );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CXYZSplit::Run(void)
{
    // load structure
    if( LoadStructure() == false ) return(false);

    // detect bonds
    FindBonds();

    // detect molecules
    SplitStructure();

// save structures
    // submolecules
    vout << debug;
    char molid = 'A';
    for(int i=0; i < NumberOfMolecules; i++){
        CSmallString name;
        if( Options.GetOptEnableCP() ){
            name << molid << ".cp";
            vout << molid << " " << name << endl;
            SaveStructureCP(name,i);
        } else {
            name << molid << ".xyz";
            vout << molid << " " << name << endl;
            CopyMolecule(i);
            SaveStructure(name);
        }
        molid++;
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CXYZSplit::Finalize(void)
{    
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    vout << high;
    vout << "# ==============================================================================" << endl;
    vout << "# xyzsplit terminated at " << dt.GetSDateAndTime() << endl;
    vout << "# ==============================================================================" << endl;

    if( ErrorSystem.IsError() || Options.GetOptVerbose() ){
        ErrorSystem.PrintErrors(vout);
    }

    vout << endl;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CXYZSplit::LoadStructure(void)
{
    if(Options.GetArgStructureName() != "-") {
        vout << "# Input XYZ structure (in)       : " << Options.GetArgStructureName() << endl;
    } else {
        vout << "# Input XYZ structure (in)       : - (standard input)" << endl;
    }

    if( Structure.Load(Options.GetArgStructureName()) == false ) {
        vout << "<red>>>> ERROR: Unable to load the XYZ structure: " << Options.GetArgStructureName() << "</red>" << endl;
        return(false);
    }
    vout << "# Number of atoms                = " << Structure.GetNumberOfAtoms() <<  endl;

    return(true);
}

//------------------------------------------------------------------------------

bool CXYZSplit::SaveStructure(const CSmallString& name)
{
    if( OutStructure.Save(name) == false ) {
        vout << "<red>>>> ERROR: Unable to save the XYZ structure: " << name << "</red>" << endl;
        return(false);
    }
    return(true);
}

//------------------------------------------------------------------------------

bool CXYZSplit::SaveStructureCP(const CSmallString& name,int molid)
{
    FILE* p_fout = fopen(name,"w");
    if( p_fout == NULL ) {
        vout << "<red>>>> ERROR: Unable to save the CP structure: " << name << "</red>" << endl;
        return(false);
    }

    for(int i=0; i < Structure.GetNumberOfAtoms(); i++ ){
        char ghostflag;
        if( MoleculeId[i] == molid ){
            ghostflag = ' ';
        } else {
            ghostflag = ':';
        }
        if( fprintf(p_fout,"%3s %c %16.9f %16.9f %16.9f\n",Structure.GetSymbol(i),ghostflag,Structure.GetPosition(i).x,Structure.GetPosition(i).y,Structure.GetPosition(i).z) <= 0 ) {
            fclose(p_fout);
            return(false);
        }
    }
    fclose(p_fout);
    return(true);
}

//------------------------------------------------------------------------------

void CXYZSplit::FindBonds(void)
{
    vout << debug;
    for(int i=0; i < Structure.GetNumberOfAtoms(); i++){
        for(int j=i+1; j < Structure.GetNumberOfAtoms(); j++){
            CPoint pi = Structure.GetPosition(i);
            int    zi = PeriodicTable.SearchZBySymbol(Structure.GetSymbol(i));
            CPoint pj = Structure.GetPosition(j);
            int    zj = PeriodicTable.SearchZBySymbol(Structure.GetSymbol(j));
            double da = PeriodicTable.GetBondDistance(zi,zj);
            double d = Size(pi-pj);
            vout << "i=" << i << " zi=" << zi << " j=" << j << " zj=" << zj << " d=" << d << " td=" << da << endl;
            if( d <= da*1.2 ){
                // bond
                pair<int,int> bond;
                bond.first = i;
                bond.second = j;
                Bonds.push_back(bond);
            }
        }
    }

    vout << low;
    vout << "# Number of bonds                = " << Bonds.size() <<  endl;
}

//------------------------------------------------------------------------------

void CXYZSplit::SplitStructure(void)
{
    // init data
    list<int>    toprocess;

    MoleculeId.resize(Structure.GetNumberOfAtoms());
    for(int i=0; i < Structure.GetNumberOfAtoms(); i++){
        toprocess.push_back(i);
        MoleculeId[0] = 0;
    }

    int molid = 0;

    while( ! toprocess.empty() ){

        int seed_atom = toprocess.front();
        toprocess.pop_front();

        list<int>   atom_seeds;
        atom_seeds.push_back(seed_atom);

        list<int>::iterator it = atom_seeds.begin();
        list<int>::iterator ie = atom_seeds.end();

        while( it != ie ){
            int at = *it;
            MoleculeId[at] = molid;

            // get bonded atoms
            vector< pair<int,int> >::iterator bt = Bonds.begin();
            vector< pair<int,int> >::iterator be = Bonds.end();

            while( bt != be ){
                if( bt->first == at ){
                    list<int>::iterator pi = find(toprocess.begin(),toprocess.end(),bt->second);
                    if( pi != toprocess.end() ){
                        atom_seeds.push_back(bt->second);
                        toprocess.erase(pi);
                    }
                }
                if( bt->second == at ){
                    list<int>::iterator pi = find(toprocess.begin(),toprocess.end(),bt->first);
                    if( pi != toprocess.end() ){
                        atom_seeds.push_back(bt->first);
                        toprocess.erase(pi);
                    }
                }
                bt++;
            }
            it++;
        }

        molid++;
        NumberOfMolecules++;
    }

    vout << "# Number of molecules            = " << NumberOfMolecules <<  endl;
}

//------------------------------------------------------------------------------

void CXYZSplit::CopyMolecule(int molid)
{
    int natoms = 0;
    for(int i=0; i < Structure.GetNumberOfAtoms(); i++){
        if( MoleculeId[i] == molid ) natoms++;
    }

    OutStructure.SetNumberOfAtoms(natoms);
    int latidx = 0;
    for(int i=0; i < Structure.GetNumberOfAtoms(); i++){
        if( MoleculeId[i] == molid ){
            OutStructure.SetSymbol(latidx,Structure.GetSymbol(i));
            OutStructure.SetPosition(latidx,Structure.GetPosition(i));
            latidx++;
        }
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


