// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2024 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <stdio.h>
#include <ErrorSystem.hpp>
#include <SmallTimeAndDate.hpp>
#include <AmberMaskAtoms.hpp>
#include <AmberMaskResidues.hpp>

#include "QMFixHX.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CQMFixHX::CQMFixHX(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CQMFixHX::Init(int argc,char* argv[])
{
    // encode program options
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    // print header --------------------------------------------------------------
    if( Options.GetOptVerbose() ) {
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        printf("\n");
        printf("// ==============================================================================\n");
        printf("// top2qmfixhx (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("// ==============================================================================\n");
        printf("//\n");
        if( Options.GetArgTopologyName() == "-" ) {
            printf("// Topology name      : -stdin- (standard input)\n");
        } else {
            printf("// Topology name      : %s\n",(const char*)Options.GetArgTopologyName());
        }
            printf("// QM atom spec       : %s\n",(const char*)Options.GetArgQMMaskSpec());
            printf("// No fix atom spec   : %s\n",(const char*)Options.GetArgNoFixMaskSpec());
        printf("// ------------------------------------------------------------------------------\n");
            printf("// Offset             : %f\n",Options.GetOptOffset());
            printf("// Force constant     : %f\n",Options.GetOptForceConstant());
        printf("// ------------------------------------------------------------------------------\n");
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CQMFixHX::Run(void)
{
// load topology
    if( Topology.Load(Options.GetArgTopologyName(),true) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified topology: %s\n",
                (const char*)Options.GetArgTopologyName());
        return(false);
    }

// assign topology and optionally coordinates


// init masks
    QMAtoms.AssignTopology(&Topology);
    if( QMAtoms.SetMask(Options.GetArgQMMaskSpec()) == false ) {
        fprintf(stderr,">>> ERROR: Unable to set specified QM mask!\n");
        return(false);
    }

    NoFixAtoms.AssignTopology(&Topology);
    if( Options.GetArgNoFixMaskSpec() != NULL ){
        if( NoFixAtoms.SetMask(Options.GetArgNoFixMaskSpec()) == false ) {
            fprintf(stderr,">>> ERROR: Unable to set specified NoFix mask!\n");
            return(false);
        }
    }

    for(int i=0; i < Topology.BondList.GetNumberOfBondsWithHydrogen(); i++){
        CAmberBond* p_bond = Topology.BondList.GetBondWithHydrogen(i);
        if( (QMAtoms.IsAtomSelected(p_bond->GetIB()) == false) && (QMAtoms.IsAtomSelected(p_bond->GetJB()) == false) ) continue;
        if( (NoFixAtoms.IsAtomSelected(p_bond->GetIB()) == true) || (NoFixAtoms.IsAtomSelected(p_bond->GetJB()) == true) ) continue;
        CAmberBondType* p_btype = Topology.BondList.GetBondType(p_bond->GetICB());
        double len = p_btype->GetREQ();
        CAmberAtom* p_at1 = Topology.AtomList.GetAtom(p_bond->GetIB());
        CAmberResidue* p_res1 = p_at1->GetResidue();
        CAmberAtom* p_at2 = Topology.AtomList.GetAtom(p_bond->GetJB());
        CAmberResidue* p_res2 = p_at1->GetResidue();
        printf("// :%d@%s - :%d@%s (%s/%s) REQ=%5.3f\n",p_bond->GetIB()+1,p_at1->GetName(),p_bond->GetJB()+1,p_at2->GetName(),
                                                          p_res1->GetName(),p_res2->GetName(),len);
        printf("D RW K @%d @%d %5.3f %10.2f\n",p_bond->GetIB()+1,p_bond->GetJB()+1,len+Options.GetOptOffset(),Options.GetOptForceConstant());
    }


    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CQMFixHX::Finalize(void)
{
    if( Options.GetOptVerbose() ) {
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        fprintf(stdout,"\n");
        fprintf(stdout,"// ==============================================================================\n");
        fprintf(stdout,"// %s terminated at %s\n",(const char*)Options.GetProgramName(),(const char*)dt.GetSDateAndTime());
        fprintf(stdout,"// ==============================================================================\n");
    }

    if( Options.GetOptVerbose() || ErrorSystem.IsError() ) {
        ErrorSystem.PrintErrors(stderr);
        fprintf(stdout,"\n");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

