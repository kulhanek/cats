// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
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

#include <ErrorSystem.hpp>
#include <SmallTimeAndDate.hpp>
#include <SimpleList.hpp>
#include <ctype.h>
#include "TopRemixLJ.hpp"
#include <set>
#include <map>
#include <string>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopRemixLJ::CTopRemixLJ(void)
{
}

//------------------------------------------------------------------------------

CTopRemixLJ::~CTopRemixLJ(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopRemixLJ::Init(int argc,char* argv[])
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
        printf("# ==============================================================================\n");
        printf("# topremixlj (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        if( Options.GetArgInputTopologyName() == "-" ) {
            printf("# Input topology file name   : -stdin- (standard input)\n");
        } else {
            printf("# Input topology file name   : %s\n",(const char*)Options.GetArgInputTopologyName());
        }
        if( Options.GetArgOutputTopologyName() == "-" ) {
            printf("# Output topology file name  : -stdout- (standard output)\n");
        } else {
            printf("# Output topology file name  : %s\n",(const char*)Options.GetArgOutputTopologyName());
        }
        printf("# ------------------------------------------------------------------------------\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopRemixLJ::Run(void)
{
    // load topology
    if( LoadTopology() == false ) return(false);

    bool result = true;

    // print types
    WriteIITypes();
    WriteIJTypes();

    // remix parameters
    printf("\n");
    printf("# Applying combining rule: %s\n",(const char*)Options.GetOptCombRule());
    ApplyCombiningRule();

    // print types
    WriteIJTypes();
    printf("\n");

    // save topology
    if( SaveTopology() == false ) return(false);

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopRemixLJ::Finalize(void)
{
    if( Options.GetOptVerbose() ) {
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        fprintf(stdout,"\n");
        fprintf(stdout,"# ==============================================================================\n");
        fprintf(stdout,"# %s terminated at %s\n",(const char*)Options.GetProgramName(),(const char*)dt.GetSDateAndTime());
        fprintf(stdout,"# ==============================================================================\n");
    }

    if( Options.GetOptVerbose() || ErrorSystem.IsError() ) {
        ErrorSystem.PrintErrors(stderr);
        fprintf(stdout,"\n");
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopRemixLJ::LoadTopology(void)
{
    if( Topology.Load(Options.GetArgInputTopologyName(),true) == false ) {
        fprintf(stderr,"\n");
        fprintf(stderr,">>> ERROR: Unable to load input topology: %s\n",
                (const char*)Options.GetArgInputTopologyName());
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopRemixLJ::SaveTopology(void)
{
    if( Topology.Save(Options.GetArgOutputTopologyName()) == false ) {
        fprintf(stderr,"\n");
        fprintf(stderr,">>> ERROR: Unable to save output topology: %s\n",
                (const char*)Options.GetArgOutputTopologyName());
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTopRemixLJ::WriteIITypes(void)
{
    set<string> Types;

    // go through atom list

    printf("\n");
    printf("# Summary of II Types\n");
    printf("\n");
    printf("# Type   Ropt/2.0        Eps    \n");
    printf("# ---- ------------ ------------\n");

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {

        CAmberAtom* p_atom = Topology.AtomList.GetAtom(i);

        if( Types.find(p_atom->GetType()) == Types.end() ) {

            if( Topology.NonBondedList.GetNonBondedType(p_atom,p_atom) < 0 ) {
                fprintf(stderr,"\n");
                fprintf(stderr," ERROR: 10-12 potential is not supported!\n");
                fprintf(stderr,"\n");
                exit(1);
            }

            double a,b,eps,rii;

            a = Topology.NonBondedList.GetAParam(p_atom,p_atom);
            b = Topology.NonBondedList.GetBParam(p_atom,p_atom);

            eps = 0.0;
            rii = 0.0;
            if( a != 0.0 ) {
                eps = b*b / (4.0 * a);
                rii = pow(2*a/b,1.0/6.0) * 0.5;
            }

            printf("  %4s %12.7f %12.7f\n",p_atom->GetType(),rii,eps);
            Types.insert(p_atom->GetType());
        }
    }

}

//------------------------------------------------------------------------------

void CTopRemixLJ::WriteIJTypes(void)
{
    map<int,string> Types;

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        CAmberAtom* p_atom = Topology.AtomList.GetAtom(i);
        Types[p_atom->GetIAC()-1] = p_atom->GetType();
    }

    printf("\n");
    printf("# Summary of IJ Types\n");
    printf("\n");
    printf("# Type Type     Ropt          Eps    \n");
    printf("# ---- ---- ------------ ------------\n");

    double lbsc = 0;
    double whsc = 0;
    double kgsc = 0;

    for(int i=0; i < Topology.NonBondedList.GetNumberOfTypes(); i++){
        for(int j=i; j < Topology.NonBondedList.GetNumberOfTypes(); j++){

            // source parameters
            int icoii = Topology.NonBondedList.GetICOIndex(Topology.NonBondedList.GetNumberOfTypes()*i + i);
            int icojj = Topology.NonBondedList.GetICOIndex(Topology.NonBondedList.GetNumberOfTypes()*j + j);

            double aii = Topology.NonBondedList.GetAParam(icoii);
            double bii = Topology.NonBondedList.GetBParam(icoii);

            double epsii = 0.0;
            double rii = 0.0;
            if( aii != 0.0 ) {
                epsii = bii*bii / (4.0 * aii);
                rii = pow(2*aii/bii,1.0/6.0) * 0.5;
            }

            double ajj = Topology.NonBondedList.GetAParam(icojj);
            double bjj = Topology.NonBondedList.GetBParam(icojj);

            double epsjj = 0.0;
            double rjj = 0.0;
            if( ajj != 0.0 ) {
                epsjj = bjj*bjj / (4.0 * ajj);
                rjj = pow(2*ajj/bjj,1.0/6.0) * 0.5;
            }

            // ij parameters
            int icoij = Topology.NonBondedList.GetICOIndex(Topology.NonBondedList.GetNumberOfTypes()*i + j);

            double aij,bij,epsij,rij;

            aij = Topology.NonBondedList.GetAParam(icoij);
            bij = Topology.NonBondedList.GetBParam(icoij);

            epsij = 0.0;
            rij = 0.0;
            if( aij != 0.0 ) {
                epsij = bij*bij / (4.0 * aij);
                rij = pow(2*aij/bij,1.0/6.0) * 0.5;
            }

            printf("  %4s %4s %12.7f %12.7f\n",Types[i].c_str(),Types[j].c_str(),rij*2.0,epsij);

            // determine combining rules
            double rijn;

            rijn = (rii+rjj)*0.5;
            lbsc = lbsc + pow(rijn-rij,2.0);

            rijn = pow( (pow(rii,6)+pow(rjj,6))*0.5, 1.0/6.0);
            whsc = whsc + pow(rijn-rij,2.0);

            double k = sqrt(epsii*pow(rii,6)*epsjj*pow(rjj,6));
            double l = pow( (pow(epsii*pow(rii,12),1.0/13.0) + pow(epsjj*pow(rjj,12),1.0/13.0))*0.5, 13);
            rijn = pow(l/k,1.0/6.0);
            kgsc = kgsc + pow(rijn-rij,2.0);
        }
    }
    bool detected = false;
    if( (lbsc < whsc) && (lbsc < kgsc) ){
        printf("\n");
        printf("# Detected combining rule (based on Dijopt): LB\n");
        detected = true;
    }
    if( (whsc < lbsc) && (whsc < kgsc) ){
        printf("\n");
        printf("# Detected combining rule (based on Dijopt): WH\n");
        detected = true;
    }
    if( (kgsc < lbsc) && (kgsc < whsc) ){
        printf("\n");
        printf("# Detected combining rule (based on Dijopt): KG\n");
        detected = true;
    }
    if( detected == false ) {
        printf("\n");
        printf("# Detected combining rule (based on Dijopt): unknown\n");
    }
}

//------------------------------------------------------------------------------

enum {
    CR_UN=0,
    CR_LB,
    CR_WH,
    CR_KG
};

void CTopRemixLJ::ApplyCombiningRule(void)
{
    int rule = CR_UN;
    if( Options.GetOptCombRule() == "LB" ){
        rule = CR_LB;
    } else if( Options.GetOptCombRule() == "WH" ){
        rule = CR_WH;
    } else if( Options.GetOptCombRule() == "KG"  ){
        rule = CR_KG;
    } else {
        fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Unsupported combining rule!\n");
        fprintf(stderr,"\n");
        exit(1);
    }

    map<int,string> Types;

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        CAmberAtom* p_atom = Topology.AtomList.GetAtom(i);
        Types[p_atom->GetIAC()-1] = p_atom->GetType();
    }

    for(int i=0; i < Topology.NonBondedList.GetNumberOfTypes(); i++){
        for(int j=i+1; j < Topology.NonBondedList.GetNumberOfTypes(); j++){

            // source parameters
            int icoii = Topology.NonBondedList.GetICOIndex(Topology.NonBondedList.GetNumberOfTypes()*i + i);
            int icojj = Topology.NonBondedList.GetICOIndex(Topology.NonBondedList.GetNumberOfTypes()*j + j);

            double aii = Topology.NonBondedList.GetAParam(icoii);
            double bii = Topology.NonBondedList.GetBParam(icoii);

            double epsii = 0.0;
            double rii = 0.0;
            if( aii != 0.0 ) {
                epsii = bii*bii / (4.0 * aii);
                rii = pow(2*aii/bii,1.0/6.0) * 0.5;
            }

            double ajj = Topology.NonBondedList.GetAParam(icojj);
            double bjj = Topology.NonBondedList.GetBParam(icojj);

            double epsjj = 0.0;
            double rjj = 0.0;
            if( ajj != 0.0 ) {
                epsjj = bjj*bjj / (4.0 * ajj);
                rjj = pow(2*ajj/bjj,1.0/6.0) * 0.5;
            }

            // remixed parameters
            int icoij = Topology.NonBondedList.GetICOIndex(Topology.NonBondedList.GetNumberOfTypes()*i + j);

            double rij;
            double epsij;

            switch(rule){
                case CR_LB:
                    rij = (rii+rjj)*0.5;
                    epsij = sqrt(epsii*epsjj);
                    break;
                case CR_WH:
                    rij = pow( (pow(rii,6)+pow(rjj,6))*0.5, 1.0/6.0);
                    epsij = sqrt(epsii*pow(rii,6)*epsjj*pow(rjj,6))/pow(rij,6);
                    break;
                case CR_KG:{
                    double k = sqrt(epsii*pow(rii,6)*epsjj*pow(rjj,6));
                    double l = pow( (pow(epsii*pow(rii,12),1.0/13.0) + pow(epsjj*pow(rjj,12),1.0/13.0))*0.5, 13);
                    rij = pow(l/k,1.0/6.0);
                    epsij = k / pow(rij,6);
                    }
                    break;
                default:
                    fprintf(stderr,"\n");
                    fprintf(stderr," ERROR: Unsupported combining rule!\n");
                    fprintf(stderr,"\n");
                    exit(1);
            }

            double aij = epsij*pow(2.0*rij,12);
            double bij = 2.0*epsij*pow(2.0*rij,6);

            Topology.NonBondedList.SetAParam(aij,icoij);
            Topology.NonBondedList.SetBParam(bij,icoij);
        }
    }

}



//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


