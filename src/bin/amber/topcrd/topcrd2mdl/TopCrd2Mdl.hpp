#ifndef TopCrd2MdlH
#define TopCrd2MdlH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2022 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu
//    Copyright (C) 2005 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include "TopCrd2MdlOptions.hpp"
#include <AmberTopology.hpp>
#include <AmberRestart.hpp>
#include <AmberMaskAtoms.hpp>
#include <VerboseStr.hpp>
#include <TerminalStr.hpp>
#include <vector>
#include <map>

//------------------------------------------------------------------------------

struct CRISMType {
    CSmallString    Name;
    std::string     Type;
    double          Mass;
    double          Charge;
    double          Epsilon;
    double          Sigma;
};

//------------------------------------------------------------------------------
namespace std
{
    template<> struct less<CRISMType>
    {
       bool operator() (const CRISMType& lhs, const CRISMType& rhs) const
       {
           // 0.0001 some tolerance in comparison
           return( (lhs.Type < rhs.Type) && ( (abs(lhs.Charge) - abs(rhs.Charge)) < 0.0001 ) );
       }
    };
}

//------------------------------------------------------------------------------

class CTopCrd2Mdl {
public:
    // constructor
    CTopCrd2Mdl(void);

// main methods ---------------------------------------------------------------
    /// init options
    int Init(int argc,char* argv[]);

    /// main part of program
    bool Run(void);

    /// finalize program
    bool Finalize(void);

// section of private data ----------------------------------------------------
private:
    CTopCrd2MdlOptions      Options;            // program options
    CAmberTopology          Topology;
    CAmberRestart           Coordinates;

    // input methods
    bool ReadCRD(FILE* p_fin);
    bool ReadCRD(const CSmallString& name);

    // output formats
    int                         NTypes;
    std::map<CRISMType,int>     RISMTypes;
    std::map<std::string,int>   AMBTypes;
    std::vector<int>            AtomTypes;

    bool WriteMDL(FILE* p_fout);
    bool InitTypes(void);
    bool SavePointers(FILE* p_file,const char* p_format);
    bool SaveAtomType(FILE* p_file,const char* p_format);
    bool SaveAtomName(FILE* p_file,const char* p_format);
    bool SaveAtomMass(FILE* p_file,const char* p_format);
    bool SaveAtomCharge(FILE* p_file,const char* p_format);
    bool SaveAtomEpsilon(FILE* p_file,const char* p_format);
    bool SaveAtomSigma(FILE* p_file,const char* p_format);
    bool SaveAtomMulti(FILE* p_file,const char* p_format);
    bool SaveAtomCoords(FILE* p_file,const char* p_format);
    bool SaveSectionHeader(FILE* p_top,const char* p_section_name,
                           const char* p_section_format);
};

//------------------------------------------------------------------------------

#endif
